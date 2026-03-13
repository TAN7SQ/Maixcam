#include "_easyLog.hpp"

std::chrono::steady_clock::time_point Log::start_time;
Log::LogLevel Log::current_level = Log::LOG_LEVEL_TRACE;

void Log::init()
{
    start_time = std::chrono::steady_clock::now();
}

uint32_t Log::get_elapsed_ms()
{
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
    return static_cast<uint32_t>(duration.count());
}

void Log::log_print(LogLevel level, const char *TAG, const char *format, va_list args)
{
    if (level > current_level) {
        return;
    }
    const char *level_char = "";
    const char *color = ANSI_RESET;
    switch (level) {
    case LOG_LEVEL_INFO:
        level_char = "I";
        color = ANSI_GREEN;
        break;
    case LOG_LEVEL_WARN:
        level_char = "W";
        color = ANSI_YELLOW;
        break;
    case LOG_LEVEL_ERROR:
        level_char = "E";
        color = ANSI_RED;
        break;
    case LOG_LEVEL_DEBUG:
        level_char = "D";
        color = ANSI_BLUE;
        break;
    case LOG_LEVEL_TRACE:
        level_char = "T";
        color = ANSI_PURPLE;
        break;
    default:
        level_char = "I";
        color = ANSI_GREEN;
        break;
    }

    char full_format[512] = {0};
    uint32_t elapsed_ms = get_elapsed_ms();
    snprintf(full_format,
             sizeof(full_format),
             "%s%s (%u) %s: %s%s", //
             color,
             level_char,
             elapsed_ms,
             TAG,
             format,
             ANSI_RESET);

    vprintf(full_format, args);
    printf("\n");
}

void Log::info(const char *TAG, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_print(LOG_LEVEL_INFO, TAG, format, args);
    va_end(args);
}

void Log::warn(const char *TAG, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_print(LOG_LEVEL_WARN, TAG, format, args);
    va_end(args);
}

void Log::error(const char *TAG, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_print(LOG_LEVEL_ERROR, TAG, format, args);
    va_end(args);
}

void Log::debug(const char *TAG, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_print(LOG_LEVEL_DEBUG, TAG, format, args);
    va_end(args);
}

void Log::trace(const char *TAG, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_print(LOG_LEVEL_TRACE, TAG, format, args);
    va_end(args);
}