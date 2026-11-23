#ifndef CSERVER_DESKTOP_COMPONENT_H
#define CSERVER_DESKTOP_COMPONENT_H

#define CSERVER_DESKTOP_ABI 1

struct cserver_desktop_component {
    const char *name;
    const char *description;

    unsigned int abi_version;

    int (*init)(void);
    void (*update)(void);
    void (*draw)(void);
    void (*shutdown)(void);
};

typedef const struct cserver_desktop_component *
    (*cserver_desktop_get_component_fn)(void);

#define CSERVER_DESKTOP_COMPONENT(symbol) \
    const struct cserver_desktop_component * \
    cserver_desktop_get_component(void) \
    { \
        return &(symbol); \
    }

#endif
