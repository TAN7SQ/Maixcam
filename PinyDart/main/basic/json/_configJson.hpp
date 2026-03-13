#pragma once
#include "_basic.hpp"
#include "nlohmann/json.hpp"

struct VisionConfig
{
    struct
    {
        std::vector<std::vector<int>> green_thresholds;
        int min_area;
        int max_area;
        int led_brightness_threshold;
    } find_blobs;

    struct
    {
        bool is_enabled;
        std::string udp_ip;
        int udp_port;
    } udp;
};

struct ControlConfig
{
    float kp;
    float ki;
    float kd;
    float max_speed;
};

struct NetworkConfig
{
    std::string wifi_ssid;
    std::string wifi_password;
};

struct AppConfig
{
    VisionConfig vision;
    // ControlConfig control;
    // NetworkConfig network;
};

class ConfigJson
{
public:
    static constexpr const char *TAG = "json";

    static bool load(const std::string &filename, AppConfig &config);

    static void print_vision(const VisionConfig &config);
};