// ----- INCLUDES -----
#include <SDL2/SDL.h>

#include <iostream>
#include <sys/time.h>
struct timeval tp;
double time() {
    gettimeofday(&tp, NULL);
    return (tp.tv_sec + tp.tv_usec / 1e6);
}

using namespace std;
// _____ INCLUDES _____

// ----- GLOBAL VARIABLES -----
struct {
    const int wid = 640;
    const int hei = 480;
} SCREEN;

SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;
// _____ GLOBAL VARIABLES _____

// ----- CODE -----
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
          SCREEN.wid, SCREEN.hei, SDL_WINDOW_SHOWN/* | SDL_WINDOW_FULLSCREEN*/);
    if (win == NULL) {
        quit();
        exit(2);
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) {
        quit();
        exit(3);
    }

    SDL_SetRenderDrawColor(ren, 0x44, 0x44, 0x44, 0xff);
}

void mainloop() {
    bool run = 1;
    SDL_Event event;

    while (run) {
        // get keyboard state
        const Uint8* keys = SDL_GetKeyboardState(NULL);

        // close event
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = 0;
            }
        }

        if (keys[SDL_SCANCODE_Q] || keys[SDL_SCANCODE_ESCAPE]) {
            run = 0;
        }
    }
}

int main() {
    init();

    mainloop();

    quit();
}
// _____ CODE _____
