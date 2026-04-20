#ifndef RENDERER_H
#define RENDERER_H

#include <unistd.h>

void renderer_init(void);
void renderer_refresh(void);
void renderer_clear(void);
void renderer_shutdown(void);

void render_bar(float height, size_t x_offset);

#endif
