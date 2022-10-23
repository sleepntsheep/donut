#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#define SHEEP_RENDERER_IMPLEMENTATION
#include "renderer.h"
#include <SDL2/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#define WIDTH 600
#define HEIGHT 600
#define FPS 60

typedef struct {
    int radius; /* distance between center of donut to donut */
    int width; /* width of donut */
    int circles_cnt;
    int dots_per_circle;
    float A;
    float B;
    float AVel;
    float BVel;
} Donut;

typedef struct {
    float x, y;
} Vec2f;

typedef struct {
    float x, y, z;
} Vec3f;

static SDL_Window *sdl_win = NULL;
static SDL_Texture *sdl_tex = NULL;
static SDL_Renderer *sdl_rend = NULL;
static uint64_t tick_now = 0, tick_prev = 0;

static Donut donut = {
    .radius = 200,
    .width = 100,
    .circles_cnt = 360,
    .dots_per_circle = 360,
    .AVel = 0.02,
    .BVel = 0.02,
};

static Renderer rend = { 0 };

void draw_circle(Renderer *rend, Donut *donut, int x, int y)
{
    /* theta is for drawing small circle
     * unit circle, x = cos(theta) and y = cos(theta)
     * using this we can draw circle with a lot of points */
    float cosA = cos(donut->A), sinA = sin(donut->A);
    float cosB = cos(donut->B), sinB = sin(donut->B);
    for (int theta = 0; theta < donut->dots_per_circle; theta++)
    {
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);
        Vec3f pt = {
            .x = donut->radius + donut->width * cosTheta,
            .y = donut->width * sinTheta,
            .z = 0,
        };
        /* To get a donut, we can sweep that circle around one axis
         * so we will rotate the circle by phi degree counter clockwise over y axis */
        for (int phi = 0; phi < donut->circles_cnt; phi++)
        {
            float cosPhi = cos(phi);
            float sinPhi = sin(phi);
            /* after rotate over y axis to get donut
             * we rotate the previous point by A degree over X axis
             * and by B degree over Z axis
             * to get a nice looking spinning animation
             *
             * So basically we are rotating the Vector pt over all axis
             *
             * Y axis rotation matrix (Y deg)
             * x'    cosY    0       sinY   x
             * y'    0       1       0      y 
             * z'    -sinY   0       cosY   z
             *
             * X axis rotation matrix (X deg)
             *  x'   1       0       0      x
             *  y' = 0       cosX    sinX   y
             *  z'   0       -sinX   cosX   z
             *
             * Z axis rotation matrix (Z deg)
             *  x''  cosZ       sinZ        0     x'
             *  y''  -sinZ      cosZ        0     y'
             *  z''  0          0           1     z'
             *
             * So you can find spinned point (x', y', z')
             * by multiplying (x, y, z) with all these 3 matrix
             * */
            const float K2 = 800.0f;
            const float K1 = WIDTH * K2 * 3 / (8 * (donut->radius + donut->width));
            const float center_x = WIDTH / 2.0f;
            const float center_y = HEIGHT / 2.0f;;
            Vec3f spinned = {
                .x = pt.x * (cosB*cosPhi + sinA*sinB*sinPhi) - pt.y * cosA*sinB,
                .y = pt.x * (cosPhi*sinB - cosB*sinA*sinPhi) + pt.y * cosA*cosB,
                .z = K2 + (cosA * pt.x * sinPhi) + (pt.y * sinA),
            };
            Vec2f projected = {
                .x = spinned.x * K1 / spinned.z,
                .y = spinned.y * K1 / spinned.z
            };
            renderer_put_point(rend, x + projected.x, y + projected.y, 0x000000FF);
        }
    }
    donut->A += donut->AVel;
    donut->B += donut->AVel;
}

void loop(void)
{
    renderer_fill(&rend, 0xFFFFFFFF);
    tick_now = SDL_GetTicks64();
    if (tick_now - tick_prev <= 1000.0f / FPS) {
        SDL_Delay(tick_now - tick_prev);
    }
    tick_prev = tick_now;

    for (SDL_Event ev; SDL_PollEvent(&ev); )
        if (ev.type == SDL_QUIT) exit(0);

    draw_circle(&rend, &donut, WIDTH / 2, HEIGHT / 2);

    SDL_UpdateTexture(sdl_tex, NULL, rend.fb, WIDTH * sizeof(uint32_t));
    SDL_RenderCopy(sdl_rend, sdl_tex, NULL, NULL);
    SDL_RenderPresent(sdl_rend);
}

void init(void)
{
    renderer_init(&rend, WIDTH, HEIGHT);
    sdl_win = SDL_CreateWindow("Donut", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    sdl_rend = SDL_CreateRenderer(sdl_win, -1, 0);
    sdl_tex = SDL_CreateTexture(sdl_rend, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    SDL_SetTextureBlendMode(sdl_tex, SDL_BLENDMODE_BLEND);
}

void cleanup(void)
{
    renderer_cleanup(&rend);
}

int main(void)
{
    init();
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(loop, 0, 1);
#else
    for (;;)
    {
        loop();
    }
#endif
    cleanup();
    return 0;
}

