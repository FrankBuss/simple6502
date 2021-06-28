#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "application.h"
#include "hardware.h"

#define SCREEN_WIDTH 736
#define SCREEN_HEIGHT 496

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define DISPLAY_SCALE 4

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *bitmapTex = NULL;
SDL_Surface *bitmapSurface = NULL;

TTF_Font *vera;

uint8_t display_font[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c,
    0x1c, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1f, 0x1f, 0x1f, 0x1f,
    0x1f, 0x1f, 0x1f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x04, 0x06, 0x05, 0x05, 0x0d,
    0x1c, 0x18, 0x18, 0x18, 0x00, 0x07, 0x08, 0x08, 0x07, 0x18, 0x18, 0x00,
    0x0f, 0x08, 0x0e, 0x08, 0x00, 0x00, 0x1f, 0x0e, 0x04, 0x00, 0x00, 0x00,
    0x08, 0x0c, 0x0e, 0x0c, 0x08, 0x00, 0x00, 0x02, 0x06, 0x0e, 0x06, 0x02,
    0x00, 0x00, 0x00, 0x04, 0x0e, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x04, 0x0a, 0x0a,
    0x0a, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x1f, 0x0a, 0x1f, 0x0a, 0x0a,
    0x04, 0x0f, 0x14, 0x0e, 0x05, 0x1e, 0x04, 0x18, 0x19, 0x02, 0x04, 0x08,
    0x13, 0x03, 0x0c, 0x12, 0x14, 0x08, 0x15, 0x12, 0x0d, 0x0c, 0x04, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02, 0x08,
    0x04, 0x02, 0x02, 0x02, 0x04, 0x08, 0x00, 0x04, 0x15, 0x0e, 0x15, 0x04,
    0x00, 0x00, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0c, 0x04, 0x08, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0c, 0x0c, 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x00,
    0x0e, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0e, 0x04, 0x0c, 0x04, 0x04, 0x04,
    0x04, 0x0e, 0x0e, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1f, 0x1f, 0x02, 0x04,
    0x02, 0x01, 0x11, 0x0e, 0x02, 0x06, 0x0a, 0x12, 0x1f, 0x02, 0x02, 0x1f,
    0x10, 0x1e, 0x01, 0x01, 0x11, 0x0e, 0x06, 0x08, 0x10, 0x1e, 0x11, 0x11,
    0x0e, 0x1f, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08, 0x0e, 0x11, 0x11, 0x0e,
    0x11, 0x11, 0x0e, 0x0e, 0x11, 0x11, 0x0f, 0x01, 0x02, 0x0c, 0x00, 0x0c,
    0x0c, 0x00, 0x0c, 0x0c, 0x00, 0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x04, 0x08,
    0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00, 0x1f, 0x00, 0x1f,
    0x00, 0x00, 0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x0e, 0x11, 0x01,
    0x02, 0x04, 0x00, 0x04, 0x0e, 0x11, 0x01, 0x0d, 0x15, 0x15, 0x0e, 0x0e,
    0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x1e, 0x11, 0x11, 0x1e, 0x11, 0x11,
    0x1e, 0x0e, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0e, 0x1c, 0x12, 0x11, 0x11,
    0x11, 0x12, 0x1c, 0x1f, 0x10, 0x10, 0x1e, 0x10, 0x10, 0x1f, 0x1f, 0x10,
    0x10, 0x1e, 0x10, 0x10, 0x10, 0x0e, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0f,
    0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11, 0x0e, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x0e, 0x07, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0c, 0x11, 0x12, 0x14,
    0x18, 0x14, 0x12, 0x11, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f, 0x11,
    0x1b, 0x15, 0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x19, 0x15, 0x13, 0x11,
    0x11, 0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e, 0x1e, 0x11, 0x11, 0x1e,
    0x10, 0x10, 0x10, 0x0e, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0d, 0x1e, 0x11,
    0x11, 0x1e, 0x14, 0x12, 0x11, 0x0f, 0x10, 0x10, 0x0e, 0x01, 0x01, 0x1e,
    0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0a, 0x04, 0x11, 0x11, 0x11,
    0x15, 0x15, 0x15, 0x0a, 0x11, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x0a, 0x04, 0x04, 0x04, 0x1f, 0x01, 0x02, 0x04, 0x08, 0x10,
    0x1f, 0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e, 0x11, 0x0a, 0x1f, 0x04,
    0x1f, 0x04, 0x04, 0x0e, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0e, 0x04, 0x0a,
    0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f,
    0x08, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x01, 0x0f,
    0x11, 0x0f, 0x10, 0x10, 0x16, 0x19, 0x11, 0x11, 0x1e, 0x00, 0x00, 0x0e,
    0x10, 0x10, 0x11, 0x0e, 0x01, 0x01, 0x0d, 0x13, 0x11, 0x11, 0x0f, 0x00,
    0x00, 0x0e, 0x11, 0x1f, 0x10, 0x0e, 0x06, 0x09, 0x08, 0x1c, 0x08, 0x08,
    0x08, 0x00, 0x0f, 0x11, 0x11, 0x0f, 0x01, 0x0e, 0x10, 0x10, 0x16, 0x19,
    0x11, 0x11, 0x11, 0x04, 0x00, 0x0c, 0x04, 0x04, 0x04, 0x0e, 0x02, 0x00,
    0x06, 0x02, 0x02, 0x12, 0x0c, 0x08, 0x08, 0x09, 0x0a, 0x0c, 0x0a, 0x09,
    0x0c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0e, 0x00, 0x00, 0x1a, 0x15, 0x15,
    0x11, 0x11, 0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11, 0x00, 0x00, 0x0e,
    0x11, 0x11, 0x11, 0x0e, 0x00, 0x00, 0x1e, 0x11, 0x1e, 0x10, 0x10, 0x00,
    0x00, 0x0d, 0x13, 0x0f, 0x01, 0x01, 0x00, 0x00, 0x16, 0x19, 0x10, 0x10,
    0x10, 0x00, 0x00, 0x0e, 0x10, 0x0e, 0x01, 0x1e, 0x08, 0x08, 0x1c, 0x08,
    0x08, 0x09, 0x06, 0x00, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0d, 0x00, 0x00,
    0x11, 0x11, 0x11, 0x0a, 0x04, 0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0a,
    0x00, 0x00, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x00, 0x00, 0x11, 0x11, 0x0f,
    0x01, 0x0e, 0x00, 0x00, 0x1f, 0x02, 0x04, 0x08, 0x1f, 0x02, 0x04, 0x04,
    0x08, 0x04, 0x04, 0x02, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x08,
    0x04, 0x04, 0x02, 0x04, 0x04, 0x08, 0x00, 0x04, 0x02, 0x1f, 0x02, 0x04,
    0x00, 0x00, 0x04, 0x08, 0x1f, 0x08, 0x04, 0x00, 0x0a, 0x00, 0x04, 0x0a,
    0x11, 0x1f, 0x11, 0x04, 0x00, 0x04, 0x0a, 0x11, 0x1f, 0x11, 0x04, 0x0a,
    0x04, 0x0a, 0x11, 0x1f, 0x11, 0x02, 0x04, 0x0e, 0x01, 0x0f, 0x11, 0x0f,
    0x04, 0x00, 0x0e, 0x01, 0x0f, 0x11, 0x0f, 0x1f, 0x18, 0x18, 0x1f, 0x18,
    0x18, 0x1f, 0x11, 0x0e, 0x11, 0x11, 0x11, 0x11, 0x0e, 0x00, 0x0a, 0x00,
    0x0e, 0x11, 0x11, 0x0e, 0x01, 0x0e, 0x13, 0x15, 0x19, 0x0e, 0x10, 0x00,
    0x02, 0x0e, 0x15, 0x15, 0x0e, 0x08, 0x11, 0x00, 0x11, 0x11, 0x11, 0x11,
    0x0e, 0x00, 0x0a, 0x00, 0x11, 0x11, 0x13, 0x0d, 0x00, 0x10, 0x08, 0x04,
    0x02, 0x01, 0x00, 0x01, 0x02, 0x1f, 0x04, 0x1f, 0x08, 0x10, 0x00, 0x08,
    0x15, 0x15, 0x02, 0x00, 0x00, 0x0f, 0x10, 0x0e, 0x11, 0x0e, 0x01, 0x1e,
    0x0f, 0x14, 0x14, 0x1f, 0x14, 0x14, 0x17, 0x00, 0x00, 0x1a, 0x05, 0x1f,
    0x14, 0x0b, 0x06, 0x09, 0x08, 0x1e, 0x08, 0x08, 0x1f, 0x1c, 0x12, 0x1c,
    0x12, 0x17, 0x12, 0x13, 0x00, 0x0e, 0x1f, 0x1f, 0x1f, 0x0e, 0x00, 0x00,
    0x0e, 0x11, 0x11, 0x11, 0x0e, 0x00, 0x00, 0x04, 0x0e, 0x1f, 0x0e, 0x04,
    0x00, 0x00, 0x04, 0x0a, 0x11, 0x0a, 0x04, 0x00, 0x04, 0x04, 0x04, 0x00,
    0x04, 0x04, 0x04, 0x0f, 0x10, 0x10, 0x10, 0x0f, 0x04, 0x08, 0x00, 0x01,
    0x03, 0x05, 0x09, 0x11, 0x1f, 0x02, 0x04, 0x08, 0x04, 0x02, 0x00, 0x1f,
    0x08, 0x04, 0x02, 0x04, 0x08, 0x00, 0x1f, 0x01, 0x01, 0x05, 0x09, 0x1f,
    0x08, 0x04, 0x04, 0x0e, 0x15, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x15, 0x0e, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1c, 0x14, 0x1c, 0x07, 0x04, 0x04, 0x04, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x1c, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x08, 0x04, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x1f,
    0x01, 0x1f, 0x01, 0x02, 0x04, 0x00, 0x00, 0x1f, 0x01, 0x06, 0x04, 0x08,
    0x00, 0x00, 0x02, 0x04, 0x0c, 0x14, 0x04, 0x00, 0x00, 0x04, 0x1f, 0x11,
    0x01, 0x06, 0x00, 0x00, 0x00, 0x1f, 0x04, 0x04, 0x1f, 0x00, 0x00, 0x02,
    0x1f, 0x06, 0x0a, 0x12, 0x00, 0x00, 0x08, 0x1f, 0x09, 0x0a, 0x08, 0x00,
    0x00, 0x00, 0x0e, 0x02, 0x02, 0x1f, 0x00, 0x00, 0x1e, 0x02, 0x1e, 0x02,
    0x1e, 0x00, 0x00, 0x00, 0x15, 0x15, 0x01, 0x06, 0x00, 0x00, 0x00, 0x1f,
    0x00, 0x00, 0x00, 0x1f, 0x01, 0x05, 0x06, 0x04, 0x04, 0x08, 0x01, 0x02,
    0x04, 0x0c, 0x14, 0x04, 0x04, 0x04, 0x1f, 0x11, 0x11, 0x01, 0x02, 0x04,
    0x00, 0x1f, 0x04, 0x04, 0x04, 0x04, 0x1f, 0x02, 0x1f, 0x02, 0x06, 0x0a,
    0x12, 0x02, 0x08, 0x1f, 0x09, 0x09, 0x09, 0x09, 0x12, 0x04, 0x1f, 0x04,
    0x1f, 0x04, 0x04, 0x04, 0x00, 0x0f, 0x09, 0x11, 0x01, 0x02, 0x0c, 0x08,
    0x0f, 0x12, 0x02, 0x02, 0x02, 0x04, 0x00, 0x1f, 0x01, 0x01, 0x01, 0x01,
    0x1f, 0x0a, 0x1f, 0x0a, 0x0a, 0x02, 0x04, 0x08, 0x00, 0x18, 0x01, 0x19,
    0x01, 0x02, 0x1c, 0x00, 0x1f, 0x01, 0x02, 0x04, 0x0a, 0x11, 0x08, 0x1f,
    0x09, 0x0a, 0x08, 0x08, 0x07, 0x00, 0x11, 0x11, 0x09, 0x01, 0x02, 0x0c,
    0x00, 0x0f, 0x09, 0x15, 0x03, 0x02, 0x0c, 0x02, 0x1c, 0x04, 0x1f, 0x04,
    0x04, 0x08, 0x00, 0x15, 0x15, 0x15, 0x01, 0x02, 0x04, 0x0e, 0x00, 0x1f,
    0x04, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x0c, 0x0a, 0x08, 0x08, 0x04,
    0x04, 0x1f, 0x04, 0x04, 0x08, 0x10, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00,
    0x1f, 0x00, 0x1f, 0x01, 0x0a, 0x04, 0x0a, 0x10, 0x04, 0x1f, 0x02, 0x04,
    0x0e, 0x15, 0x04, 0x02, 0x02, 0x02, 0x02, 0x02, 0x04, 0x08, 0x00, 0x04,
    0x02, 0x11, 0x11, 0x11, 0x11, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x10, 0x0f,
    0x00, 0x1f, 0x01, 0x01, 0x01, 0x02, 0x0c, 0x00, 0x08, 0x14, 0x02, 0x01,
    0x01, 0x00, 0x04, 0x1f, 0x04, 0x04, 0x15, 0x15, 0x04, 0x00, 0x1f, 0x01,
    0x01, 0x0a, 0x04, 0x02, 0x00, 0x0e, 0x00, 0x0e, 0x00, 0x0e, 0x01, 0x00,
    0x04, 0x08, 0x10, 0x11, 0x1f, 0x01, 0x00, 0x01, 0x01, 0x0a, 0x04, 0x0a,
    0x10, 0x00, 0x1f, 0x08, 0x1f, 0x08, 0x08, 0x07, 0x08, 0x08, 0x1f, 0x09,
    0x0a, 0x08, 0x08, 0x00, 0x0e, 0x02, 0x02, 0x02, 0x02, 0x1f, 0x00, 0x1f,
    0x01, 0x1f, 0x01, 0x01, 0x1f, 0x0e, 0x00, 0x1f, 0x01, 0x01, 0x02, 0x04,
    0x12, 0x12, 0x12, 0x12, 0x02, 0x04, 0x08, 0x00, 0x04, 0x14, 0x14, 0x15,
    0x15, 0x16, 0x00, 0x10, 0x10, 0x11, 0x12, 0x14, 0x18, 0x00, 0x1f, 0x11,
    0x11, 0x11, 0x11, 0x1f, 0x00, 0x1f, 0x11, 0x11, 0x01, 0x02, 0x04, 0x00,
    0x18, 0x00, 0x01, 0x01, 0x02, 0x1c, 0x04, 0x12, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x1c, 0x14, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x15,
    0x12, 0x12, 0x0d, 0x0a, 0x00, 0x0e, 0x01, 0x0f, 0x11, 0x0f, 0x00, 0x0e,
    0x11, 0x1e, 0x11, 0x1e, 0x10, 0x00, 0x00, 0x0e, 0x10, 0x0c, 0x11, 0x0e,
    0x00, 0x11, 0x11, 0x13, 0x1d, 0x10, 0x10, 0x00, 0x00, 0x0f, 0x14, 0x12,
    0x11, 0x0e, 0x00, 0x06, 0x09, 0x11, 0x1e, 0x10, 0x10, 0x00, 0x00, 0x0f,
    0x11, 0x0f, 0x01, 0x0e, 0x00, 0x00, 0x07, 0x04, 0x04, 0x14, 0x08, 0x02,
    0x1a, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x02, 0x12,
    0x0c, 0x14, 0x08, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x0e, 0x14,
    0x15, 0x0e, 0x04, 0x08, 0x08, 0x1c, 0x08, 0x1c, 0x08, 0x0f, 0x0e, 0x00,
    0x16, 0x19, 0x11, 0x11, 0x11, 0x0a, 0x00, 0x0e, 0x11, 0x11, 0x11, 0x0e,
    0x00, 0x16, 0x19, 0x11, 0x1e, 0x10, 0x10, 0x00, 0x0d, 0x13, 0x11, 0x0f,
    0x01, 0x01, 0x00, 0x0e, 0x11, 0x1f, 0x11, 0x11, 0x0e, 0x00, 0x00, 0x00,
    0x0b, 0x15, 0x1a, 0x00, 0x00, 0x00, 0x0e, 0x11, 0x11, 0x0a, 0x1b, 0x0a,
    0x00, 0x11, 0x11, 0x11, 0x13, 0x0d, 0x1f, 0x10, 0x08, 0x04, 0x08, 0x10,
    0x1f, 0x00, 0x00, 0x1f, 0x0a, 0x0a, 0x0a, 0x13, 0x1f, 0x00, 0x11, 0x0a,
    0x04, 0x0a, 0x11, 0x00, 0x00, 0x11, 0x11, 0x0f, 0x01, 0x0e, 0x00, 0x01,
    0x1e, 0x04, 0x1f, 0x04, 0x04, 0x00, 0x00, 0x1f, 0x08, 0x0f, 0x09, 0x11,
    0x00, 0x00, 0x1f, 0x15, 0x1f, 0x11, 0x11, 0x00, 0x04, 0x00, 0x1f, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f,
    0x1f, 0x1f, 0x1f, 0x1f,
};

bool display[5 * 7 * 16 * 2];
int sample_number = 0;
int sin_frequency = 0;

static int quit = 0;

#define LED_SIZE 28

bool red_led = false;

// hardware implementation

void set_red_led(bool state) { red_led = state; }

const float AMPLITUDE = 0.5f;
const int SAMPLE_RATE = 44100;
const int AUDIO_BUFFER_SIZE = 2048;

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes) {
    float *buffer = (float *)raw_buffer;
    int length = bytes / sizeof(float);

    if (sin_frequency == 0) {
        for (int i = 0; i < length;) {
            buffer[i++] = 0;
            buffer[i++] = 0;
        }
    } else {
        for (int i = 0; i < length; sample_number++) {
            double time = (double)sample_number / (double)SAMPLE_RATE;
            float sample =
                AMPLITUDE * sinf(2.0f * M_PI * float(sin_frequency) * time);
            buffer[i++] = sample;
            buffer[i++] = sample;
        }
    }
}

void buzzer(int frequency) {
    sin_frequency = frequency;
    sample_number = 0;
}

void audio_init() {
    // open audio device
    SDL_AudioSpec want;
    want.freq = SAMPLE_RATE;
    want.format = AUDIO_F32SYS;
    want.channels = 2;
    want.samples = AUDIO_BUFFER_SIZE;
    want.callback = audio_callback;
    if (SDL_OpenAudio(&want, NULL) != 0)
        fprintf(stderr, "Failed to open audio: %s\n", SDL_GetError());

    // start playing sound
    SDL_PauseAudio(0);
}

// SDL painting functions

void draw_text(int x, int y, char *text) {
    SDL_Color black = {0, 0, 0};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(vera, text, black);
    SDL_Texture *m = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect r;
    r.x = x;
    r.y = y;
    TTF_SizeText(vera, text, &r.w, &r.h);
    SDL_RenderCopy(renderer, m, NULL, &r);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(m);
}

void draw_led(int x, int y, int r, int g, int b) {
    SDL_Rect r_scr;
    r_scr.x = x;
    r_scr.y = y;
    r_scr.w = LED_SIZE;
    r_scr.h = LED_SIZE;
    SDL_SetRenderDrawColor(renderer, r, g, b, 0x00);
    SDL_RenderFillRect(renderer, &r_scr);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0x00);
    SDL_RenderDrawRect(renderer, &r_scr);
}

void draw_display_text(int x0, int y0, char text) {
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x0 > 15) x0 = 15;
    if (y0 > 1) y0 = 1;
    int start = x0 * 5 * 7 + y0 * 5 * 7 * 16;
    for (int y = 0; y < 7; y++) {
        int i = 16;
        for (int x = 0; x < 5; x++) {
            if (display_font[7*text + y] & i) {
                display[start] = 1;
            } else {
                display[start] = 0;
            }
            start++;
            i /= 2;
        }
    }
}

void draw_display() {
    int i = 0;
    int xofs = 150;
    int yofs = 150;
    for (int l = 0; l < 2; l++) {
        for (int c = 0; c < 16; c++) {
            for (int y = 0; y < 7; y++) {
                for (int x = 0; x < 5; x++) {
                    SDL_Rect r_scr;
                    r_scr.x = x * 3 + c * (5 * 3 + 3) + xofs;
                    r_scr.y = y * 3 + l * (7 * 3 + 3) + yofs;
                    r_scr.w = 2;
                    r_scr.h = 2;
                    if (display[i++]) {
                        SDL_SetRenderDrawColor(renderer, 0x20, 0xff, 0x20,
                                               0x00);
                    } else {
                        SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40,
                                               0x00);
                    }
                    SDL_RenderFillRect(renderer, &r_scr);
                }
            }
        }
    }
}

void render() {
    // clear background
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    // copy background image
    SDL_RenderCopy(renderer, bitmapTex, NULL, NULL);

    // render LED
    int x = 76;
    int y = 147;
    if (red_led) {
        draw_led(x, y, 0x5f, 0, 0);
    } else {
        draw_led(x, y, 0xff, 0, 0);
    }

    // render display
    draw_display();

    // show CPU information
    char buf[100];
    x = 32;
    y = 300;
    int ofs = 20;

    sprintf(buf, "total cycles: %" PRIu64, total_cycles);
    draw_text(x, y, buf);
    y += ofs;

    sprintf(buf, "PC: %04x", theCPU.pc);
    draw_text(x, y, buf);
    y += ofs;

    sprintf(buf, "A: %02x", theCPU.A);
    draw_text(x, y, buf);
    y += ofs;

    sprintf(buf, "X: %02x", theCPU.X);
    draw_text(x, y, buf);
    y += ofs;

    sprintf(buf, "Y: %02x", theCPU.Y);
    draw_text(x, y, buf);
    y += ofs;

    sprintf(buf, "SR: %02x", theCPU.status);
    draw_text(x, y, buf);
    y += ofs;

    sprintf(buf, "SP: %02x", theCPU.sp);
    draw_text(x, y, buf);
    y += ofs;

    // update screen
    SDL_RenderPresent(renderer);
}

#if __EMSCRIPTEN__
void main_tick() {
#else
int main_tick() {
#endif

    static uint32_t last_tick_time = 0;
    uint32_t delta = 0;
    uint32_t tick_time = SDL_GetTicks();
    delta = tick_time - last_tick_time;
    last_tick_time = tick_time;

    SDL_Event event;

#if __EMSCRIPTEN__
    while (SDL_PollEvent(&event)) {
#else
    SDL_WaitEvent(&event);
#endif
        switch (event.type) {
            case SDL_USEREVENT: {
#if __EMSCRIPTEN__
                delta = 50;
#endif
                tick(delta);
                break;
            }

            case SDL_QUIT: {
                quit = 1;
                break;
            }
            case SDL_KEYDOWN: {
                onKeyDown(event.key.keysym.sym);
                break;
            }
            case SDL_KEYUP: {
                onKeyUp(event.key.keysym.sym);
                break;
            }
        }
#if __EMSCRIPTEN__
        SDL_Delay(10);
    }
#endif

    render();
    SDL_UpdateWindowSurface(window);

#if !__EMSCRIPTEN__
    return 0;
#endif
}

void main_loop() {
#if __EMSCRIPTEN__
    emscripten_set_main_loop(main_tick, -1, 1);
#else
    while (0 == quit) {
        main_tick();
    }
#endif
}

Uint32 my_callbackfunc(Uint32 interval, void *param) {
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return (interval);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    window =
        SDL_CreateWindow("Simple 6502 Emulator", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                         SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

    IMG_Init(IMG_INIT_PNG);
    bitmapSurface = IMG_Load("background.png");
    bitmapTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
    SDL_FreeSurface(bitmapSurface);

    TTF_Init();
    vera = TTF_OpenFont("Vera.ttf", 18);

    audio_init();

    start();

    Uint32 delay = 50;
    SDL_AddTimer(delay, my_callbackfunc, 0);

    main_loop();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}