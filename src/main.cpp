// ----- INCLUDES --------------------
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

#include <sys/time.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "math_vector.h"
#include <vector>
#include <math.h>
#include <limits>

using namespace std;
// _____ INCLUDES _____

// ----- STRUCTURES --------------------
struct Object {
    double (*dist)(const double3 &pos);
    double3 color = {1, 1, 1};

    Object(double (*dist)(const double3 &pos)) : dist(dist) {}
    Object(double (*dist)(const double3 &pos), const double3 &color) : dist(dist), color(color) {}
};
// _____ STRUCTURES _____

// ----- GLOBAL VARIABLES --------------------
SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;

struct {
    const int screen_width  = 320;//640;
    const int screen_height = 240;//480;

    const bool fullscreen = 0;

    const double FOV   = 3.14159265358979323846 / 3;
    const double zNear = 1;
    const double zFar  = 50;

    const uint maxMarchingIter = 50;

    const double collisionDistance = 0.00001;     // min dist to count collision

    const double moveSpeed = 5;
} settings;

double3 viewpoint = {0, 0, 0};
// double3 front = {0, 0, -1};
// double3 up    = {0, 1,  0};
// double3 side  = {1, 0,  0};

bool run = 1;
SDL_Event event;

vector<Object> objs;
// _____ GLOBAL VARIABLES _____

// ----- CODE --------------------
struct timeval tp;
long long time() {
    gettimeofday(&tp, NULL);
    return (tp.tv_sec * 1e6 + tp.tv_usec);
}

double sq(const double &n) {
    return n * n;
}

void quit() {
    SDL_DestroyWindow(win);
    win = NULL;

    SDL_DestroyRenderer(ren);
    ren = NULL;

    SDL_Quit();
}

void init() {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        quit();
        exit(1);
    }

    win = SDL_CreateWindow("Ray Marching", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
          settings.screen_width, settings.screen_height,
          SDL_WINDOW_SHOWN | (settings.fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
    if (win == NULL) {
        quit();
        exit(2);
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) {
        quit();
        exit(3);
    }
}

double3 getNormal(const Object &obj, const double3 &pos) {
    const double eps = 0.0001;
    double d  = obj.dist(pos);
    double x = obj.dist(pos + double3({eps, 0, 0})) - d;
    double y = obj.dist(pos + double3({0, eps, 0})) - d;
    double z = obj.dist(pos + double3({0, 0, eps})) - d;
    return double3({x, y, z}).normalize();
}

double3 ray_marching(double3 pos, double3 dir) {
    dir = dir.normalize();
    double dist = 0;

    for (int i = 0; i < settings.maxMarchingIter && dist < settings.zFar; ++i) {
        double mindist = 1e100;
        for (const Object &obj : objs) {
            mindist = min(mindist, obj.dist(pos));
            if (mindist <= settings.collisionDistance) {
                //return obj.color * max(0.4, (getNormal(obj, pos) * (double3({5, 5, 0}) - pos).normalize()));
                return obj.color * max(0.4, (getNormal(obj, pos) * (viewpoint - pos).normalize()));
            }
        }

        pos = pos + dir * mindist;
        dist += mindist;
    }

    return {0, 0, 0};
}

int eventthread(void *data) {
    long long lastTimeStamp = time();

    while (run) {
        long long nowTimeStamp = time();
        double deltaTime = (nowTimeStamp - lastTimeStamp) / 1.e6;
        lastTimeStamp = nowTimeStamp;

        // close event
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = 0;
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(NULL);

        // ---- keyboard events ------------
        if (keys[SDL_SCANCODE_Q] || keys[SDL_SCANCODE_ESCAPE]) {
            run = 0;
        }

        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_K] || keys[SDL_SCANCODE_UP   ]) viewpoint[2] += settings.moveSpeed * deltaTime;
        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_J] || keys[SDL_SCANCODE_DOWN ]) viewpoint[2] -= settings.moveSpeed * deltaTime;
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_H] || keys[SDL_SCANCODE_LEFT ]) viewpoint[0] -= settings.moveSpeed * deltaTime;
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_L] || keys[SDL_SCANCODE_RIGHT]) viewpoint[0] += settings.moveSpeed * deltaTime;
        // ____ keyboard events ____
    }

    return 0;
}

double mod(const double &lhs, const double &rhs) {
    return lhs - floor(lhs / rhs) * rhs;
}

int main() {
    init();

    // ----- OBJECTS ---------------
    objs = {
        {[](const double3 &pos) -> double {
                return sqrt(max(0., abs(pos[0] + 3 ) - 1) * max(0., abs(pos[0] + 3 ) - 1) +
                            max(0., abs(pos[1] + 2 ) - 1) * max(0., abs(pos[1] + 2 ) - 1) +
                            max(0., abs(pos[2] - 10) - 1) * max(0., abs(pos[2] - 10) - 1));
            }, {1, 1, 1}},
        {[](const double3 &pos) -> double {
            return (pos - double3({0, 0, 15})).len() - 2;
        }, {1, 1, 1}},
        {[](const double3 &pos) -> double {
            double x = pos[0] - 2;
            double y = pos[1] - 0;
            double z = pos[2] - 15;
            // x = mod(x, 1.);
            return double3({x, y, z}).len() - 2;
        }, {1, 1, 1}},
        {[](const double3 &pos) -> double {
            return pos[1] + 3 - sin(pos[0]) * sin(pos[2]);
        }, {1, 1, 1}},
        // {[](const double3 &pos) -> double {
        //     const double x = pos[0];
        //     const double y = pos[1] - 3;
        //     const double z = pos[2] - 20;
        //     const double r = 10;
        //     const double a = 2;
        //     return sq(sq(x) + sq(y) + sq(z) + sq(r) - sq(a)) - 4 * sq(r) * (sq(x) + sq(y));
        // }, {1, 1, 1}},
    };
    // _____ OBJECTS _____

    SDL_CreateThread(eventthread, NULL, NULL);

    // ----- MAINLOOP --------------------
    while (run) {
        // ---- drawing ------------
        double realx, realy;
        if (settings.screen_width == settings.screen_height) {
            realx = realy = 1;
        } else if (settings.screen_width < settings.screen_height) {
            realx = 1;
            realy = settings.screen_height / settings.screen_width;
        } else {
            realy = 1;
            realx = settings.screen_width / settings.screen_height;
        }

        for (uint x = 0; x < settings.screen_width; ++x) {
            for (uint y = 0; y < settings.screen_height; ++y) {
                double3 color = ray_marching(viewpoint, 
                    {((1. * x / settings.screen_width) - 0.5) * realx,
                     ((-1. * y / settings.screen_height) + 0.5) * realy,
                     settings.zNear});

                SDL_SetRenderDrawColor(ren, color[0] * 255, color[1] * 255, color[2] * 255, 0xff);
                SDL_RenderDrawPoint(ren, x, y);
            }
        }

        SDL_RenderPresent(ren);
        // SDL_Delay(100);
        // ____ drawing ____
    }
    // _____ MAINLOOP _____

    quit();
}
// _____ CODE _____
