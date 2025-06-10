#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "StageSelectScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void StageSelectScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    // Stage 1 按鈕
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 1", "pirulen.ttf", 48, halfW, halfH / 2, 0, 0, 0, 255, 0.5, 0.5));

    // Stage 2 按鈕
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 100, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 2", "pirulen.ttf", 48, halfW, halfH / 2 + 150, 0, 0, 0, 255, 0.5, 0.5));

    // Back 按鈕
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 400, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH / 2 + 450, 0, 0, 0, 255, 0.5, 0.5));

    // // 切換無限模式按鈕（Toggle）
    // btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 250, 400, 100);
    // btn->SetOnClickCallback(std::bind(&StageSelectScene::ToggleInfiniteMode, this));
    // AddNewControlObject(btn);
    // AddNewObject(new Engine::Label("Toggle Mode", "pirulen.ttf", 36, halfW, halfH / 2 + 300, 0, 0, 0, 255, 0.5, 0.5));

    // 畫面上顯示目前模式的 Label
    std::string modestr;
    if (isMultiplayer)
    {
        if(isInfiniteMode)
        {
            modestr = "Mode: MULTIPLAYER";
        }
        else
        {
            modestr = "Mode: INFINITE";
        }
    }
    else
    {
        modestr = "Mode: NORMAL";
    }

    AddNewObject(new Engine::Label(modestr, "pirulen.ttf", 36, halfW, halfH + 350, 255, 0, 0, 255, 0.5, 0.5));

    // 播放背景音樂
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void StageSelectScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void StageSelectScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("mode-select");
}

void StageSelectScene::PlayOnClick(int stage)
{
    if(!isMultiplayer)
    {
        PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
        if (scene)
        {
            scene->MapId = stage;
            scene->isInfiniteMode = isInfiniteMode; // 傳遞無限模式設定
            scene->IsCustom = 0;
            scene->map_rereaded = 1;
        }
        Engine::GameEngine::GetInstance().ChangeScene("play");
    }
    else
    {
        //TODO: link into multiplayer scene...
    }
}

void StageSelectScene::ScoreboardOnClick()
{
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
}

void StageSelectScene::BGMSlideOnValueChanged(float value)
{
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
}

void StageSelectScene::SFXSlideOnValueChanged(float value)
{
    AudioHelper::SFXVolume = value;
}

// // 顯示當前模式
// void StageSelectScene::ToggleInfiniteMode()
// {
//     isInfiniteMode = !isInfiniteMode;
//     if (modeLabel)
//     {
//         if (isInfiniteMode)
//         {
//             modeLabel->SetText("Mode: INFINITE");
//         }
//         else
//         {
//             modeLabel->SetText("Mode: NORMAL");
//         }
//     }
// }
