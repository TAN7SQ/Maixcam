#include "main.h"
#include "maix_basic.hpp"

#include "_fusion.hpp"
#include "_vision.hpp"

namespace InstallErr
{
constexpr float CAMERA_PITCH_DEG = -20.0f; // 向下为负，向上为正
constexpr float CAMERA_YAW_DEG = 0.0f;     // 向右偏为正，向左偏为负
constexpr float CAMERA_ROLL_DEG = 0.0f;    // 顺时针滚为正，逆时针为负

constexpr float DEG2RAD = M_PI / 180.0f;

constexpr float PITCH_RAD = CAMERA_PITCH_DEG * DEG2RAD;
constexpr float YAW_RAD = CAMERA_YAW_DEG * DEG2RAD;
constexpr float ROLL_RAD = CAMERA_ROLL_DEG * DEG2RAD;

float CP = std::cos(PITCH_RAD);
float SP = std::sin(PITCH_RAD);
float CY = std::cos(YAW_RAD);
float SY = std::sin(YAW_RAD);
float CR = std::cos(ROLL_RAD);
float SR = std::sin(ROLL_RAD);

constexpr float TX = 0.0f; // 左右偏移
constexpr float TY = 0.0f; // 上下偏移
constexpr float TZ = 0.0f; // 前后偏移
} // namespace InstallErr

using namespace maix;

void Fusion::deThread()
{
    if (pFusionThread && pFusionThread->joinable()) {
        pFusionThread->join();
        delete pFusionThread;
        pFusionThread = nullptr;
    }
}

void Fusion::fusionSchedule(const FusionConfig &config)
{
    this->_config = config;

    if (pFusionThread == nullptr) {
        pFusionThread = new std::thread(&Fusion::fusionThread, this);
        pthread_setname_np(pFusionThread->native_handle(), "FusionThread");
    }
}

void Fusion::fusionThread(void)
{
    log::info("fusion thread start");

    uint64_t now;
    ControlCmd cmd;

    while (!app::need_exit()) {

        maix::thread::sleep_ms(10);
        if (!Shared::gTargetQueue.empty())
            camAim = Shared::gTargetQueue.pop_non_blocking();

        now = maix::time::ticks_ms();
        float dt = (parms.last_time == 0) ? 0.01f : (now - parms.last_time) * 1e-3f;
        parms.last_time = now;

        if (camAim.valid) {

            bodyT = cam2body(camAim);

            float yaw_error = bodyT.yaw_error;
            float pitch_error = bodyT.pitch_error;

            // LOS角速度
            float yaw_rate = (yaw_error - parms.last_yaw_error) / dt;
            float pitch_rate = (pitch_error - parms.last_pitch_error) / dt;

            parms.last_yaw_error = yaw_error;
            parms.last_pitch_error = pitch_error;

            parms.yaw_rate_lpf = 0.7f * parms.yaw_rate_lpf + 0.3f * yaw_rate;
            parms.pitch_rate_lpf = 0.7f * parms.pitch_rate_lpf + 0.3f * pitch_rate;

            parms.yaw_rate_lpf = std::clamp(parms.yaw_rate_lpf, -parms.max_rate, parms.max_rate);
            parms.pitch_rate_lpf = std::clamp(parms.pitch_rate_lpf, -parms.max_rate, parms.max_rate);

            // 混合导引
            cmd.yaw_rate_cmd = parms.Kp_angle * yaw_error + parms.Kpn * parms.yaw_rate_lpf;
            cmd.pitch_rate_cmd = parms.Kp_angle * pitch_error + parms.Kpn * parms.pitch_rate_lpf;

            cmd.yaw_rate_cmd = std::clamp(cmd.yaw_rate_cmd, -parms.max_rate, parms.max_rate);
            cmd.pitch_rate_cmd = std::clamp(cmd.pitch_rate_cmd, -parms.max_rate, parms.max_rate);

            cmd.valid = 1;
        }
        else {
            // 丢目标
            cmd.yaw_rate_cmd = 0;
            cmd.pitch_rate_cmd = 0;
            cmd.valid = 0;
        }

        // ===== 发送 =====
        Shared::gControlQueue.push_latest(cmd);

        Log::info(TAG, "cmd yaw_rate:%.2f pitch_rate:%.2f", cmd.yaw_rate_cmd, cmd.pitch_rate_cmd);
    }

    log::info("fusion thread exit");
}

BodyTarget Fusion::cam2body(const CamTargetData cam)
{
    using namespace InstallErr;

    BodyTarget result;
    result.valid = true;

    float x_c = cam.normX;
    float y_c = -cam.normY; // cam与body的坐标定义相反
    float z_c = 1.0f;

    // Pitch-Yaw-Roll
    float x1 = x_c;
    float y1 = y_c * CP - z_c * SP;
    float z1 = y_c * SP + z_c * CP;

    float x2 = x1 * CY + z1 * SY;
    float y2 = y1;
    float z2 = -x1 * SY + z1 * CY;

    float x3 = x2 * CR - y2 * SR;
    float y3 = x2 * SR + y2 * CR;
    float z3 = z2;
    // 平移补偿
    result.x = x3 + TX;
    result.y = y3 + TY;
    result.z = z3 + TZ;

    result.yaw_error = atan2(result.x, result.z);
    result.pitch_error = atan2(result.y, result.z);
    result.roll_error = atan2(result.y, sqrt(result.x * result.x + result.z * result.z));

    result.valid = true;

    return result;
}

ControlErr Fusion::body2world(const BodyTarget body, const IMUAttitude imu)
{
    ControlErr control;

    Vec3 bodyVec(body.x, body.y, body.z);
    Vec3 world = Tools::quat_rotate(imu.quat, bodyVec);

    control.yaw = atan2(world.x, world.z);
    control.pitch = atan2(world.y, sqrt(world.x * world.x + world.z * world.z));
    return control;
}
