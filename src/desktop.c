#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>
#include <limits.h>

#include <cserver/desktop/component.h>
#include <cserver/desktop/loader.h>

struct loaded_component {
    void *handle;
    const struct cserver_desktop_component *component;
    struct loaded_component *next;
};

static struct loaded_component *components;
static struct loaded_component *components_tail;

static int has_so_extension(const char *name)
{
    size_t length = strlen(name);

    return length > 3 &&
           strcmp(name + length - 3, ".so") == 0;
}

static int load_component(const char *path)
{
    void *handle;
    void *symbol;

    cserver_desktop_get_component_fn get_component;

    const struct cserver_desktop_component *component;

    struct loaded_component *loaded;

    printf("cserver: loading component %s\n", path);

    handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);

    if (!handle) {
        fprintf(
            stderr,
            "cserver: dlopen failed: %s\n",
            dlerror()
        );
        return -1;
    }

    dlerror();

    symbol = dlsym(handle, "cserver_desktop_get_component");

    const char *error = dlerror();

    if (error || !symbol) {
        fprintf(
            stderr,
            "cserver: missing component entry point: %s\n",
            error ? error : "symbol not found"
        );

        dlclose(handle);
        return -1;
    }

    get_component =
        (cserver_desktop_get_component_fn)symbol;

    component = get_component();

    if (!component) {
        fprintf(stderr, "cserver: invalid component descriptor\n");
        dlclose(handle);
        return -1;
    }

    if (component->abi_version != CSERVER_DESKTOP_ABI) {
        fprintf(
            stderr,
            "cserver: incompatible component ABI: %s\n",
            component->name ? component->name : "unknown"
        );

        dlclose(handle);
        return -1;
    }

    if (!component->name || !component->init) {
        fprintf(stderr, "cserver: incomplete component descriptor\n");
        dlclose(handle);
        return -1;
    }

    loaded = calloc(1, sizeof(*loaded));

    if (!loaded) {
        perror("cserver: component allocation failed");
        dlclose(handle);
        return -1;
    }

    if (component->init() < 0) {
        fprintf(
            stderr,
            "cserver: component initialization failed: %s\n",
            component->name
        );

        free(loaded);
        dlclose(handle);
        return -1;
    }

    loaded->handle = handle;
    loaded->component = component;

    if (components_tail)
        components_tail->next = loaded;
    else
        components = loaded;

    components_tail = loaded;

    printf("cserver: loaded component: %s\n", component->name);

    return 0;
}

int cserver_desktop_load(const char *path)
{
    DIR *directory;
    struct dirent *entry;

    int loaded_count = 0;
    int failed_count = 0;

    if (!path)
        return -1;

    directory = opendir(path);

    if (!directory) {
        perror("cserver: cannot open component directory");
        return -1;
    }

    while ((entry = readdir(directory)) != NULL) {
        char *full_path;
        size_t path_length;
        size_t name_length;

        if (!has_so_extension(entry->d_name))
            continue;

        path_length = strlen(path);
        name_length = strlen(entry->d_name);

        full_path = malloc(path_length + name_length + 2);

        if (!full_path) {
            perror("cserver: path allocation failed");
            failed_count++;
            continue;
        }

        snprintf(
            full_path,
            path_length + name_length + 2,
            "%s/%s",
            path,
            entry->d_name
        );

        if (load_component(full_path) == 0)
            loaded_count++;
        else
            failed_count++;

        free(full_path);
    }

    closedir(directory);

    printf(
        "cserver: components loaded: %d, failed: %d\n",
        loaded_count,
        failed_count
    );

    return loaded_count;
}

void cserver_desktop_update(void)
{
    for (struct loaded_component *item = components;
         item;
         item = item->next) {
        if (item->component->update)
            item->component->update();
    }
}

void cserver_desktop_draw(void)
{
    for (struct loaded_component *item = components;
         item;
         item = item->next) {
        if (item->component->draw)
            item->component->draw();
    }
}

void cserver_desktop_unload(void)
{
    struct loaded_component *item = components;

    while (item) {
        struct loaded_component *next = item->next;

        if (item->component->shutdown)
            item->component->shutdown();

        printf(
            "cserver: unloading component: %s\n",
            item->component->name
        );

        dlclose(item->handle);
        free(item);

        item = next;
    }

    components = NULL;
    components_tail = NULL;
}
