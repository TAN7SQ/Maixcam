
#include "main.h"

#include "_basic.hpp"
#include "application.hpp"

using namespace maix;

int _main(int argc, char *argv[])
{

    log::info("Program start");

    App app = App();
    app.appSchedule(argc, argv);

    log::info("Program exit");
    return 0;
}

int main(int argc, char *argv[])
{
    sys::register_default_signal_handle();
    CATCH_EXCEPTION_RUN_RETURN(_main, -1, argc, argv);
}
