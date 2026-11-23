#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm.h>
#include <drm_mode.h>

#include <cserver/display.h>

int cserver_display_fd = -1;

enum cserver_display_backend cserver_display_backend =
    CSERVER_DISPLAY_NONE;

uint32_t cserver_display_width;
uint32_t cserver_display_height;
uint32_t cserver_display_pitch;
uint32_t *cserver_display_pixels;

static void *display_map = MAP_FAILED;
static size_t display_map_size;

static uint32_t fb_red_offset;
static uint32_t fb_red_length;
static uint32_t fb_green_offset;
static uint32_t fb_green_length;
static uint32_t fb_blue_offset;
static uint32_t fb_blue_length;

static uint32_t drm_handle;
static uint32_t drm_fb;
static uint32_t drm_crtc_id;
static uint32_t drm_connector_id;

static uint32_t drm_pitch;
static uint64_t drm_size;

static void *drm_map = MAP_FAILED;
static drmModeCrtcPtr drm_saved_crtc;

static int drm_modeset_active;

static uint32_t pack_component(uint8_t value, uint32_t offset,
                               uint32_t length)
{
    if (!length)
        return 0;

    uint64_t max = (1ULL << length) - 1;
    uint64_t scaled = ((uint64_t)value * max) / 255;

    return (uint32_t)(scaled << offset);
}

static uint32_t display_pack_color(uint32_t color)
{
    uint8_t red = (color >> 16) & 0xff;
    uint8_t green = (color >> 8) & 0xff;
    uint8_t blue = color & 0xff;

    if (cserver_display_backend != CSERVER_DISPLAY_FB)
        return color & 0x00ffffff;

    return pack_component(red, fb_red_offset, fb_red_length) |
           pack_component(green, fb_green_offset, fb_green_length) |
           pack_component(blue, fb_blue_offset, fb_blue_length);
}

void cserver_display_put_pixel(
    uint32_t x,
    uint32_t y,
    uint32_t color
)
{
    if (!cserver_display_pixels)
        return;

    if (x >= cserver_display_width ||
        y >= cserver_display_height)
        return;

    uint8_t *row = (uint8_t *)cserver_display_pixels +
                   (size_t)y * cserver_display_pitch;

    uint32_t *pixel = (uint32_t *)(row + (size_t)x * 4);

    *pixel = display_pack_color(color);
}

void cserver_display_clear(uint32_t color)
{
    cserver_display_fill_rect(
        0,
        0,
        cserver_display_width,
        cserver_display_height,
        color
    );
}

void cserver_display_fill_rect(
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height,
    uint32_t color
)
{
    if (!cserver_display_pixels || !width || !height)
        return;

    if (x >= cserver_display_width ||
        y >= cserver_display_height)
        return;

    if (width > cserver_display_width - x)
        width = cserver_display_width - x;

    if (height > cserver_display_height - y)
        height = cserver_display_height - y;

    uint32_t packed_color = display_pack_color(color);

    for (uint32_t row = 0; row < height; row++) {
        uint8_t *address =
            (uint8_t *)cserver_display_pixels +
            (size_t)(y + row) * cserver_display_pitch +
            (size_t)x * 4;

        uint32_t *pixels = (uint32_t *)address;

        for (uint32_t column = 0; column < width; column++)
            pixels[column] = packed_color;
    }
}

static void display_reset(void)
{
    cserver_display_width = 0;
    cserver_display_height = 0;
    cserver_display_pitch = 0;
    cserver_display_pixels = NULL;

    display_map = MAP_FAILED;
    display_map_size = 0;

    fb_red_offset = 0;
    fb_red_length = 0;
    fb_green_offset = 0;
    fb_green_length = 0;
    fb_blue_offset = 0;
    fb_blue_length = 0;
}

static void drm_cleanup(void)
{
    if (cserver_display_fd < 0)
        return;

    if (drm_modeset_active && drm_saved_crtc) {
        if (drm_saved_crtc->mode_valid) {
            drmModeSetCrtc(
                cserver_display_fd,
                drm_saved_crtc->crtc_id,
                drm_saved_crtc->buffer_id,
                drm_saved_crtc->x,
                drm_saved_crtc->y,
                &drm_connector_id,
                1,
                &drm_saved_crtc->mode
            );
        }

        drm_modeset_active = 0;
    }

    if (drm_saved_crtc) {
        drmModeFreeCrtc(drm_saved_crtc);
        drm_saved_crtc = NULL;
    }

    if (drm_fb) {
        drmModeRmFB(cserver_display_fd, drm_fb);
        drm_fb = 0;
    }

    if (drm_map != MAP_FAILED) {
        munmap(drm_map, drm_size);
        drm_map = MAP_FAILED;
    }

    if (drm_handle) {
        struct drm_mode_destroy_dumb destroy = {
            .handle = drm_handle
        };

        drmIoctl(
            cserver_display_fd,
            DRM_IOCTL_MODE_DESTROY_DUMB,
            &destroy
        );

        drm_handle = 0;
    }

    drm_pitch = 0;
    drm_size = 0;

    display_reset();
}

static int drm_find_display(
    drmModeResPtr resources,
    drmModeModeInfo *selected_mode
)
{
    for (int i = 0; i < resources->count_connectors; i++) {
        drmModeConnectorPtr connector;

        connector = drmModeGetConnector(
            cserver_display_fd,
            resources->connectors[i]
        );

        if (!connector)
            continue;

        if (connector->connection != DRM_MODE_CONNECTED ||
            connector->count_modes == 0) {
            drmModeFreeConnector(connector);
            continue;
        }

        int mode_index = 0;

        for (int m = 0; m < connector->count_modes; m++) {
            if (connector->modes[m].type &
                DRM_MODE_TYPE_PREFERRED) {
                mode_index = m;
                break;
            }
        }

        *selected_mode = connector->modes[mode_index];

        for (int e = 0; e < connector->count_encoders; e++) {
            drmModeEncoderPtr encoder;

            encoder = drmModeGetEncoder(
                cserver_display_fd,
                connector->encoders[e]
            );

            if (!encoder)
                continue;

            for (int c = 0; c < resources->count_crtcs; c++) {
                if (!(encoder->possible_crtcs & (1U << c)))
                    continue;

                drm_crtc_id = resources->crtcs[c];
                drm_connector_id = connector->connector_id;

                drmModeFreeEncoder(encoder);
                drmModeFreeConnector(connector);

                return 0;
            }

            drmModeFreeEncoder(encoder);
        }

        drmModeFreeConnector(connector);
    }

    return -1;
}

static int drm_create_buffer(uint32_t width, uint32_t height)
{
    struct drm_mode_create_dumb create = {
        .width = width,
        .height = height,
        .bpp = 32
    };

    if (drmIoctl(
        cserver_display_fd,
        DRM_IOCTL_MODE_CREATE_DUMB,
        &create
    ) < 0) {
        perror("cserver: DRM buffer creation failed");
        return -1;
    }

    drm_handle = create.handle;
    drm_pitch = create.pitch;
    drm_size = create.size;

    if (drmModeAddFB(
        cserver_display_fd,
        width,
        height,
        24,
        32,
        drm_pitch,
        drm_handle,
        &drm_fb
    ) < 0) {
        perror("cserver: DRM framebuffer creation failed");
        return -1;
    }

    struct drm_mode_map_dumb map = {
        .handle = drm_handle
    };

    if (drmIoctl(
        cserver_display_fd,
        DRM_IOCTL_MODE_MAP_DUMB,
        &map
    ) < 0) {
        perror("cserver: DRM buffer mapping failed");
        return -1;
    }

    drm_map = mmap(
        NULL,
        drm_size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        cserver_display_fd,
        map.offset
    );

    if (drm_map == MAP_FAILED) {
        perror("cserver: DRM mmap failed");
        return -1;
    }

    memset(drm_map, 0, drm_size);

    display_map = drm_map;
    display_map_size = drm_size;

    cserver_display_width = width;
    cserver_display_height = height;
    cserver_display_pitch = drm_pitch;
    cserver_display_pixels = drm_map;

    return 0;
}

static int drm_init(void)
{
    uint64_t has_dumb = 0;

    printf("cserver: opening DRM device\n");

    cserver_display_fd = open(
        "/dev/dri/card0",
        O_RDWR | O_CLOEXEC
    );

    if (cserver_display_fd < 0)
        return -1;

    if (!drmIsKMS(cserver_display_fd)) {
        fprintf(stderr, "cserver: device does not support KMS\n");
        return -1;
    }

    if (drmGetCap(
        cserver_display_fd,
        DRM_CAP_DUMB_BUFFER,
        &has_dumb
    ) < 0 || !has_dumb) {
        fprintf(stderr, "cserver: dumb buffers unavailable\n");
        return -1;
    }

    drmModeResPtr resources =
        drmModeGetResources(cserver_display_fd);

    if (!resources) {
        fprintf(stderr, "cserver: cannot get DRM resources\n");
        return -1;
    }

    drmModeModeInfo mode;

    if (drm_find_display(resources, &mode) < 0) {
        fprintf(stderr, "cserver: no usable display found\n");
        drmModeFreeResources(resources);
        return -1;
    }

    printf(
        "cserver: selected mode %ux%u\n",
        mode.hdisplay,
        mode.vdisplay
    );

    drm_saved_crtc = drmModeGetCrtc(
        cserver_display_fd,
        drm_crtc_id
    );

    if (!drm_saved_crtc) {
        fprintf(stderr, "cserver: cannot save current CRTC\n");
        drmModeFreeResources(resources);
        return -1;
    }

    if (drm_create_buffer(mode.hdisplay, mode.vdisplay) < 0) {
        drmModeFreeResources(resources);
        return -1;
    }

    if (drmModeSetCrtc(
        cserver_display_fd,
        drm_crtc_id,
        drm_fb,
        0,
        0,
        &drm_connector_id,
        1,
        &mode
    ) < 0) {
        perror("cserver: DRM modeset failed");
        drmModeFreeResources(resources);
        return -1;
    }

    drm_modeset_active = 1;

    drmModeFreeResources(resources);

    printf("cserver: DRM/KMS initialized\n");

    return 0;
}

static int fb_init(void)
{
    struct fb_fix_screeninfo fixed;
    struct fb_var_screeninfo variable;

    if (ioctl(
        cserver_display_fd,
        FBIOGET_FSCREENINFO,
        &fixed
    ) < 0) {
        perror("cserver: FBIOGET_FSCREENINFO failed");
        return -1;
    }

    if (ioctl(
        cserver_display_fd,
        FBIOGET_VSCREENINFO,
        &variable
    ) < 0) {
        perror("cserver: FBIOGET_VSCREENINFO failed");
        return -1;
    }

    if (variable.bits_per_pixel != 32 ||
        variable.red.length != 8 ||
        variable.green.length != 8 ||
        variable.blue.length != 8 ||
        variable.red.msb_right ||
        variable.green.msb_right ||
        variable.blue.msb_right ||
        variable.red.offset + variable.red.length > 32 ||
        variable.green.offset + variable.green.length > 32 ||
        variable.blue.offset + variable.blue.length > 32) {
        fprintf(stderr, "cserver: unsupported framebuffer format\n");
        return -1;
    }

    if (!variable.xres || !variable.yres ||
        fixed.line_length < variable.xres * 4 ||
        !fixed.smem_len ||
        (uint64_t)fixed.line_length * variable.yres >
            fixed.smem_len) {
        fprintf(stderr, "cserver: invalid framebuffer geometry\n");
        return -1;
    }

    display_map_size = fixed.smem_len;

    display_map = mmap(
        NULL,
        display_map_size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        cserver_display_fd,
        0
    );

    if (display_map == MAP_FAILED) {
        perror("cserver: framebuffer mmap failed");
        return -1;
    }

    cserver_display_width = variable.xres;
    cserver_display_height = variable.yres;
    cserver_display_pitch = fixed.line_length;
    cserver_display_pixels = display_map;

    fb_red_offset = variable.red.offset;
    fb_red_length = variable.red.length;
    fb_green_offset = variable.green.offset;
    fb_green_length = variable.green.length;
    fb_blue_offset = variable.blue.offset;
    fb_blue_length = variable.blue.length;

    printf(
        "cserver: framebuffer initialized: %ux%u\n",
        cserver_display_width,
        cserver_display_height
    );

    return 0;
}

int cserver_display_init(void)
{
    printf("cserver: initializing display...\n");
    printf("cserver: trying DRM/KMS\n");

    if (drm_init() == 0) {
        cserver_display_backend = CSERVER_DISPLAY_DRM;
        return 0;
    }

    if (cserver_display_fd >= 0) {
        drm_cleanup();
        close(cserver_display_fd);
        cserver_display_fd = -1;
    }

    printf("cserver: DRM initialization failed\n");
    printf("cserver: trying framebuffer (/dev/fb0)\n");

    cserver_display_fd = open(
        "/dev/fb0",
        O_RDWR | O_CLOEXEC
    );

    if (cserver_display_fd < 0) {
        perror("cserver: framebuffer unavailable");
        return -1;
    }

    if (fb_init() < 0) {
        if (display_map != MAP_FAILED)
            munmap(display_map, display_map_size);

        display_reset();

        close(cserver_display_fd);
        cserver_display_fd = -1;

        return -1;
    }

    cserver_display_backend = CSERVER_DISPLAY_FB;

    return 0;
}

void cserver_display_close(void)
{
    if (cserver_display_fd >= 0) {
        if (cserver_display_backend == CSERVER_DISPLAY_DRM) {
            drm_cleanup();
        } else if (cserver_display_backend == CSERVER_DISPLAY_FB) {
            if (display_map != MAP_FAILED)
                munmap(display_map, display_map_size);

            display_reset();
        }

        close(cserver_display_fd);
        cserver_display_fd = -1;
    }

    cserver_display_backend = CSERVER_DISPLAY_NONE;
}
