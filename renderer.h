#ifndef SHEEP_RENDERER_H
#define SHEEP_RENDERER_H
#include <stdint.h>

typedef struct {
    uint32_t *fb;
    int width;
    int height;
} Renderer;

void renderer_init(Renderer *, int width, int height);
void renderer_cleanup(Renderer *);
void renderer_put_point(Renderer *, int x, int y, uint32_t color);
void renderer_put_rect(Renderer *, int x, int y, int w, int h, uint32_t color);
void renderer_fill(Renderer *, uint32_t color);
void renderer_put_circle(Renderer *, int x, int y, int r, uint32_t color);
void renderer_put_line(Renderer *, int x1, int y1, int x2, int y2, int size, uint32_t color);

#endif /* SHEEP_RENDERER_H */

#ifdef SHEEP_RENDERER_IMPLEMENTATION

void renderer_init(Renderer *r, int width, int height)
{
    r->fb = calloc(sizeof *r->fb, height * width);
    r->width = width;
    r->height = height;
}

void renderer_cleanup(Renderer *r)
{
    free(r->fb);
}

void renderer_put_point(Renderer *r, int x, int y, uint32_t color)
{
    if (x < 0 || y < 0 || x >= r->width || y >= r->height) return;
    r->fb[y * r->width + x] = color;
}

void renderer_put_rect(Renderer *r, int x, int y, int w, int h, uint32_t color)
{
    for (int i = y; i < y + h; i++)
    {
        for (int j = x; j < x + w; j++)
        {
            r->fb[i * r->width + j] = color;
        }
    }
}

void renderer_fill(Renderer *r, uint32_t color)
{
    renderer_put_rect(r, 0, 0, r->width, r->height, color);
}

#ifndef ABS
#define ABS(a) ((a) < 0 ? (-(a)) : (a))
#endif /* ABS */

void renderer_put_line(Renderer *r, int x1, int y1, int x2, int y2, int size, uint32_t color) {
    int dx = ABS(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -ABS(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;
    for (;;) {
        for (int i = -size; i < size; i++) {
            for (int j = -size; j < size; j++) {
                if (x1 + i < 0 || x1 + i >= r->width ||
                        y1 + j < 0 || y1 + j >= r->height)
                    continue;
                renderer_put_point(r, x1 + i, y1 + j, color);
            }
        }
        if (x1 == x2 && y1 == y2)
            break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void renderer_put_circle(Renderer *r, int x, int y, int radius, uint32_t color) {

}

#endif /* SHEEP_RENDERER_IMPLEMENTATION */

