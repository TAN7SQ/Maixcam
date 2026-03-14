#pragma once

#include "_basic.hpp"

#include "_configJson.hpp"

class App
{
public:
    App() = default;
    ~App() = default;

    void appSchedule(int argc, char *argv[]);

private:
    AppConfig config;
    void appInit(int argc, char *argv[]);
};