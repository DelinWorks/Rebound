#include "EmptyScene.h"

USING_NS_CC;

Scene* EmptyScene::createScene()
{
    return EmptyScene::create();
}

bool EmptyScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    scheduleUpdate();

    //auto update_fps_action = CallFunc::create([&]() {
    //    f32 deltaTime = Director::getInstance()->getDeltaTime();
    //    char buff[16];
    //    char buffDt[16];
    //    char buffTime[16];
    //    snprintf(buff, sizeof(buff), "%.1lf", 1.0F / deltaTime);
    //    snprintf(buffDt, sizeof(buffDt), "%.1lf", deltaTime * 1000);
    //    std::string buffAsStdStr = buff;
    //    std::string buffAsStdStrDt = buffDt;
    //    sprintf_s<16Ui64>(buffTime, "%01.0f:%02.0f:%04.1f", floor(el_time / 3600.0), floor(fmod(el_time, 3600.0) / 60.0), fmod(el_time, 60.0));
    //    std::cout << "\r" << buffTime << "s | No map selected | 0 Players | TPS: " << buffAsStdStr << " / " << buffAsStdStrDt << "ms\t\t\t\t";
    //    });
    //auto wait_fps_action = DelayTime::create(1);
    //auto make_seq = Sequence::create(update_fps_action, wait_fps_action, nullptr);
    //auto seq_repeat_forever = RepeatForever::create(make_seq);
    //runAction(seq_repeat_forever);
    //str ip;
    //if (olc::net::getLocalMachineIp("8.8.8.8", ip) != 0)
    //{
    //    std::cout << std::endl << ip << "\n\n";
    //    while (true) { Sleep(15); }
    //}
    //std::cout << "Server Stats: PUBLIC at " << ip << ":" << "4624" << " :" << std::endl;
    return true;
}

void EmptyScene::update(F32 dt)
{

}
