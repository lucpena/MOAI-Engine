#ifndef CONFIG_H
#define CONFIG_H

#include "stb_image.h"

constexpr auto MAX_POINT_LIGHTS = 16;
constexpr auto MAX_SPOT_LIGHTS = 16;
constexpr auto MAX_LIGHTS = MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS;

constexpr auto WINDOW_WIDTH = 1280;
constexpr auto WINDOW_HEIGHT = 720;

#endif