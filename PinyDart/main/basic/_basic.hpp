#pragma once

#include <cstdint>
#include <cstring>

//================================================================

#include "opencv2/freetype.hpp"
#include "opencv2/opencv.hpp"

#include "maix_basic.hpp"
#include "maix_camera.hpp"
#include "maix_display.hpp"
#include "maix_image_cv.hpp"
#include "maix_thread.hpp"

#include <thread>

//================================================================
#include "_easyLog.hpp"

class FPSCount
{
public:
    FPSCount()
    {
        last_time = maix::time::ticks_ms();
    }
    void tick()
    {
        frame_cnt++;
        uint64_t now = maix::time::ticks_ms();
        uint64_t delta = now - last_time;
        if (delta >= 1000) {
            fps = (float)frame_cnt / (delta / 1000.0f);
            snprintf(fps_str, sizeof(fps_str), "%.2f FPS", fps);
            frame_cnt = 0;
            last_time = now;
        }
    }
    float get() const
    {
        return fps;
    }
    const char *str() const
    {
        return fps_str;
    }

private:
    uint32_t frame_cnt = 0;
    uint64_t last_time = 0;
    float fps = 0.0f;
    char fps_str[32] = {0};
};