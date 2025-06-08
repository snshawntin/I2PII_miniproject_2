#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "ModeSelectScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void ModeSelectScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    //title
    AddNewObject(new Engine::Label("Select Gamemode", "pirulen.ttf", 60, halfW, halfH / 5 + 50, 10, 255, 255, 255, 0.5, 0.5));

    //normal mode
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 150, 400, 100);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::NormalModeOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Normal mode", "pirulen.ttf", 36, halfW, halfH / 2 + 200, 0, 0, 0, 255, 0.5, 0.5));
    
    //infinite mode
    //*連接到無限模式
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 700, halfH / 2 + 150, 400, 100);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::InfiniteModeOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Infinite mode", "pirulen.ttf", 38, halfW - 500, halfH / 2 + 200, 0, 0, 0, 255, 0.5, 0.5));

    //custom map
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 300, halfH / 2 + 150, 400, 100);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::CustomMapOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Play", "pirulen.ttf", 36, halfW + 500, halfH / 2 + 180, 0, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("custom map", "pirulen.ttf", 36, halfW + 500, halfH / 2 + 220, 0, 0, 0, 255, 0.5, 0.5));

    //back
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ModeSelectScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void ModeSelectScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void ModeSelectScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void ModeSelectScene::NormalModeOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void ModeSelectScene::InfiniteModeOnClick() {
    //*connect to infinite mode.
    //Engine::GameEngine::GetInstance().ChangeScene("...");
}
void ModeSelectScene::CustomMapOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("custom-map-select");
}