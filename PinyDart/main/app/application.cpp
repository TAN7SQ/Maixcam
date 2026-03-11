#include "application.hpp"

using namespace maix;

#include "_easyLog.hpp"
#include "vision/_vision.hpp"

#include <signal.h>

// static std::atomic<bool> g_exit_flag(false);

// void signal_handler(int sig)
// {
//     printf("\nSIGINT received, exiting...\n");
//     g_exit_flag = true;
// }

void App::appInit()
{

    Log::init();
    Log::info("App", "App init");
}

void App::appSchedule()
{
    App::appInit();

    Vision vision = Vision();
    vision.visionSchedule();

    while (!app::need_exit()) {
        maix::thread::sleep_ms(1000);
    }
}
