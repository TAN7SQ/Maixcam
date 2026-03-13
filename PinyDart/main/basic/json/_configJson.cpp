#include "_configJson.hpp"
#include "_basic.hpp"

using json = nlohmann::json;

bool ConfigJson::load(const std::string &filename, AppConfig &config)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file " << filename << std::endl;
        return false;
    }

    try {
        json j;
        file >> j;

        // Parse vision config
        if (!j.contains("vision")) {
            Log::error(TAG, "config missing vision section");
            return false;
        }
        auto v = j["vision"];
        auto fb = v["find_blobs"];

        config.vision.find_blobs.green_thresholds =
            fb.value("green_thresholds", std::vector<std::vector<int>>{{60, 100, -80, -10, -30, 10}});

        config.vision.find_blobs.min_area = fb.value("min_area", 50);
        config.vision.find_blobs.max_area = fb.value("max_area", 5000);
        config.vision.find_blobs.led_brightness_threshold = fb.value("led_brightness_threshold", 120);

        config.vision.udp.udp_ip = v.value("udp_ip", "192.168.1.100");
        config.vision.udp.udp_port = v.value("udp_port", 5000);
    } catch (std::exception &e) {
        std::cerr << "Error: Failed to parse config file " << filename << std::endl;
        return false;
    }

    printf("--------------------------------------------\n");
    Log::trace(TAG, "Loaded config: %s", filename.c_str());
    printf("--------------------------------------------\n");
    return true;
}

void ConfigJson::print_vision(const VisionConfig &config)
{
    printf("--------------------------------------------\n");
    Log::trace(TAG, "green_thresholds: ");
    for (size_t i = 0; i < config.find_blobs.green_thresholds.size(); i++) {
        Log::trace(TAG,
                   "threshold[%d %d %d %d %d %d]: ",
                   config.find_blobs.green_thresholds[0][0],
                   config.find_blobs.green_thresholds[0][1],
                   config.find_blobs.green_thresholds[0][2],
                   config.find_blobs.green_thresholds[0][3],
                   config.find_blobs.green_thresholds[0][4],
                   config.find_blobs.green_thresholds[0][5]);
    }
    Log::trace(TAG, "min_area: %d", config.find_blobs.min_area);
    Log::trace(TAG, "max_area: %d", config.find_blobs.max_area);
    Log::trace(TAG, "led_brightness_threshold: %d", config.find_blobs.led_brightness_threshold);
    Log::trace(TAG, "udp_ip: %s", config.udp.udp_ip.c_str());
    Log::trace(TAG, "udp_port: %d", config.udp.udp_port);
    printf("--------------------------------------------\n");
}