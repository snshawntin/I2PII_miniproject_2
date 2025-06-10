#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "CustomModeSelectScene.hpp"
#include "StageSelectScene.hpp"
#include "PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void CustomModeSelectScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    //title
    AddNewObject(new Engine::Label("Select Gamemode", "pirulen.ttf", 60, halfW, halfH / 5 + 50, 10, 255, 255, 255, 0.5, 0.5));

    //multi mode
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200 + 250, halfH / 2 + 150, 400, 100);
    btn->SetOnClickCallback(std::bind(&CustomModeSelectScene::MultiModeOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("multiplayer", "pirulen.ttf", 36, halfW + 250, halfH / 2 + 200, 0, 0, 0, 255, 0.5, 0.5));
    
    //infinite mode
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 700 + 250, halfH / 2 + 150, 400, 100);
    btn->SetOnClickCallback(std::bind(&CustomModeSelectScene::InfiniteModeOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("singleplayer", "pirulen.ttf", 38, halfW - 500 + 250, halfH / 2 + 200, 0, 0, 0, 255, 0.5, 0.5));
    
    //back
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&CustomModeSelectScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    //display the coutom map id
    AddNewObject(new Engine::Label("Map: custom map 0" + std::to_string(custom_map_index), "pirulen.ttf", 36, halfW, halfH + 350, 255, 0, 0, 255, 0.5, 0.5));

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void CustomModeSelectScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void CustomModeSelectScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("custom-map-select");
}

void CustomModeSelectScene::MultiModeOnClick() {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    if (scene){
        scene->MapId = custom_map_index;
        scene->isInfiniteMode = 0;
        scene->isMultiPlayer = 1;
        scene->IsCustom = 1;
        scene->map_rereaded = 0;
    }

    Engine::GameEngine::GetInstance().ChangeScene("play");
}

void CustomModeSelectScene::InfiniteModeOnClick() {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    if (scene){
        scene->MapId = custom_map_index;
        scene->isInfiniteMode = 1;
        scene->isMultiPlayer = 0;
        scene->IsCustom = 1;
        scene->map_rereaded = 0;
    }

    Engine::GameEngine::GetInstance().ChangeScene("play");
}