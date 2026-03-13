#pragma once

#include "_basic.hpp"

class App
{
public:
    App() = default;
    ~App() = default;

    void appSchedule(int argc, char *argv[]);

private:
    void appInit(int argc, char *argv[]);
};