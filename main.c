#include <math.h>
#include <raylib.h>
#include <stdint.h>


#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#endif

#ifndef uint
#define uint unsigned int
#endif /* uint */

#define ASPEED 0.02
#define BSPEED 0.02

typedef struct {
    /* radius - distance between center of donut to donut */
    uint radius;
    /* width - width of donut */
    uint width;
    uint circles_cnt;
    uint dots_per_circle;
} donut_t;

const donut_t donut = {
    .radius = 200,
    .width = 100,
    .circles_cnt = 360,
    .dots_per_circle = 360
};

#define win_width 600.0f
#define win_height 600.0f
#define center_x (win_width / 2)
#define center_y (win_height / 2)
#define K2 800.0f
#define K1 win_width*K2*3/(8*(donut.radius + donut.width))

float A = 0;
float B = 0;

void main_loop()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Donut Moment - sleepntsheep", 10, 10, 20, GRAY);

    /* theta is for drawing small circle
     * unit circle, x = cos(theta)
     *              y = cos(theta)
     * using this we can draw circle with a lot of points
     * */

    float cosA = cos(A),
          sinA = sin(A);
    float cosB = cos(B),
          sinB = sin(B);
    for (int theta = 0; theta < donut.dots_per_circle; theta++)
    {
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);
        Vector3 pt = {
            .x = donut.radius + donut.width * cosTheta,
            .y = donut.width * sinTheta,
            .z = 0,
        };
        /* To get a donut, we can sweep that circle
         * so we will rotate the circle by phi degree counter clockwise
         * over y axis
         * */
        for (int phi = 0; phi < donut.circles_cnt; phi++)
        {
            float cosPhi = cos(phi);
            float sinPhi = sin(phi);
            /* rotate over y axis to get donut
             * then rotate the previous point by A degree over X axis
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
            Vector3 spinned = {
                .x = pt.x * (cosB*cosPhi + sinA*sinB*sinPhi) - pt.y * cosA*sinB,
                .y = pt.x * (cosPhi*sinB - cosB*sinA*sinPhi) + pt.y * cosA*cosB,
                .z = K2 + (cosA * pt.x * sinPhi) + (pt.y * sinA),
            };
            /* projected into 2d plane for drawing */
            Vector2 projected = {
                .x = center_x + spinned.x * K1 / spinned.z,
                .y = center_y - spinned.y * K1 / spinned.z
            };
            DrawPixelV(projected, (Color){0,0,0,255});
        }
    }

    A += ASPEED;
    B += BSPEED;
    EndDrawing();

}

int main
(int argc, char** argv)
{
    InitWindow(win_width, win_height, "DONUTO!!!");
    SetTargetFPS(60);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 60, 1);
#else
    while (!WindowShouldClose())
    {
        main_loop();
    }
#endif

    CloseWindow();

    return 0;
}
