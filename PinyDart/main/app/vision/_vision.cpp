#include "_vision.hpp"

#include "_easyLog.hpp"

using namespace maix;

void Vision::visionSchedule(void)
{
    camRecord =
        new camera::Camera(IMG_WIDTH / 2, IMG_HEIGHT / 2, image::Format::FMT_RGB888, nullptr, 30, 3, true, false);

    cam = camRecord->add_channel(IMG_WIDTH, IMG_HEIGHT, image::Format::FMT_RGB888, 80, 3, true);

    if (pCameraThread == nullptr) {
        pCameraThread = new std::thread(&Vision::cameraThread, this, cam);
        pthread_setname_np(pCameraThread->native_handle(), "cameraThread");
    }
    // if (pVisionThread == nullptr) {
    //     pVisionThread = new std::thread(&Vision::visionThread, this);
    //     pthread_setname_np(pVisionThread->native_handle(), "visionThread");
    // }
    if (pRecoderThread == nullptr) {
        pRecoderThread = new std::thread(&Vision::recoderThread, this, camRecord);
        pthread_setname_np(pRecoderThread->native_handle(), "recoderThread");
    }
}
void Vision::cameraThread(camera::Camera *cam)
{

    Log::info(TAG, "camera thread start");

    // camera::Camera vcam(IMG_WIDTH, IMG_HEIGHT, image::Format::FMT_YVU420SP, nullptr, 80, 3, true, false);

    while (!app::need_exit()) {
        try {
            maix::image::Image *img = cam->read();
            if (!img)
                continue;

            // std::shared_ptr<image::Image> frame = std::shared_ptr<image::Image>(img, [](image::Image *p) {
            //     delete p;
            // });

            // frameQueue.push(frame);

            delete img;
        } catch (...) {
            Log::error(TAG, "camera read error");
        }
    }
}
void Vision::visionThread()
{
    Log::info(TAG, "vision thread start");

    while (!app::need_exit()) {

        std::shared_ptr<image::Image> img = frameQueue.pop();
        if (!img) {
            continue;
        }
        // video::Frame *vis_img = img->copy();

        // recordQueue.push();
    }
}

void Vision::recoderThread(camera::Camera *recordCam)
{

    maix::thread::sleep_ms(100);
    Log::info(TAG, "recoder thread start");

    // static int skip = 0;

    video::Encoder enc("/root/test.mp4",
                       IMG_WIDTH / 2, // 这个是编码格式，调用下方的resize函数调整到这个尺寸，编码效率更高
                       IMG_HEIGHT / 2,
                       image::Format::FMT_YVU420SP,
                       video::VideoType::VIDEO_H264,
                       20, // fps
                       50,
                       1200 * 1000, // bitrate
                       1000,
                       false,
                       true);

    while (!app::need_exit()) {

        image::Image *img = recordCam->read();
        // auto vimg = img->to_format(image::Format::FMT_RGB888);
        img->draw_string(10, 10, fps.str(), image::Color(255, 0, 0));
        img->draw_string(10, 10, fps.str(), image::Color(255, 0, 0), 0.1);
        img->draw_string(10, 10, fps.str(), image::Color(255, 0, 0), 0.5);
        auto vimg2 = img->to_format(image::Format::FMT_YVU420SP);
        video::Frame *frame = enc.encode(vimg2);
        // delete vimg;
        delete vimg2;

        delete frame;
        delete img;
        fps.tick();
        Log::info(TAG, "%s", fps.str());
    }
}

Vision::~Vision()
{
    if (pVisionThread) {
        if (pVisionThread->joinable()) {
            pVisionThread->join();
        }
        delete pVisionThread;
        pVisionThread = nullptr;
    }
    if (pRecoderThread) {
        if (pRecoderThread->joinable()) {
            pRecoderThread->join();
        }
        delete pRecoderThread;
        pRecoderThread = nullptr;
    }
    if (pCameraThread) {
        if (pCameraThread->joinable())
            pCameraThread->join();
        delete pCameraThread;
    }

    Log::warn(TAG, "vision thread destroy");
}
