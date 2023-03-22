#pragma once
#ifndef NATIVE
#include "esp_camera.h"
#endif
#include <array>
#include <cmath>

#define FRAME_SIZE FRAMESIZE_SVGA
#define WIDTH 800
#define HEIGHT 600
#define BLOCK_SIZE 8
#define W (WIDTH / BLOCK_SIZE)
#define H (HEIGHT / BLOCK_SIZE)

class Cam {
    private:
        std::array<uint16_t, H * W> current_frame; 
        std::array<uint8_t, H * W> previous_frame;
        // std::array<uint8_t, std::ceil(H * W / 8)> changed_pixels_bits; // bits encoded as bytes in uint8_t
        float block_diff_threshold, image_diff_threshold;

    public:
        Cam(float block_diff_threshold, float image_diff_threshold);
        #ifndef NATIVE
        bool setup_camera(framesize_t framesize);
        #endif
        bool capture_still();
        bool motion_detect();
        void update_frame();
        std::array<uint16_t, H * W> &get_current_frame();
};