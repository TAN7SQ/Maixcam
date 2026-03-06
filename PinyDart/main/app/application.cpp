#include "application.hpp"

using namespace maix;

#include "_easyLog.hpp"
#include "vision/_vision.hpp"
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
