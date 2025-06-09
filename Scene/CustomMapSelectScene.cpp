#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "ModeSelectScene.hpp"
#include "CustomMapSelectScene.hpp"
#include "MapEditScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void CustomMapSelectScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    //title
    AddNewObject(new Engine::Label("Select/Create Custom Map", "pirulen.ttf", 60, halfW, halfH / 5 + 50, 10, 255, 255, 255, 0.5, 0.5));
    const unsigned short dist_vertical = 20;
    const unsigned short dist_horizontal = 30;
    std::string custom_map_name;
    for (unsigned i = 0; i < 5; i++){
        custom_map_name = "custom map " + std::to_string(i + 1);
        AddNewObject(new Engine::Label(custom_map_name, "pirulen.ttf", 30, halfW - 180, halfH - 155 + i * (50 + dist_vertical), 255, 100, 255, 255, 0.5, 0.5));

        btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 30, halfH - 180 + i * (50 + dist_vertical), 150, 50);
        btn->SetOnClickCallback(std::bind(&CustomMapSelectScene::EditOnClick, this, i + 1));
        AddNewControlObject(btn);
        AddNewObject(new Engine::Label("Edit", "pirulen.ttf", 24, halfW + 75 + 30, halfH + 25 - 180 + i * (50 + dist_vertical), 0, 0, 0, 255, 0.5, 0.5));

        //TODO: determine the enemy sequence in play! section.
        //and connect to playscene.
        btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 180 + dist_horizontal, halfH - 180 + i * (50 + dist_vertical), 150, 50);
        btn->SetOnClickCallback(std::bind(&CustomMapSelectScene::PlayCustomOnClick, this));
        AddNewControlObject(btn);
        AddNewObject(new Engine::Label("Play!", "pirulen.ttf", 24, halfW + 75 + 180 + dist_horizontal, halfH + 25 - 180 + i * (50 + dist_vertical), 0, 0, 0, 255, 0.5, 0.5));
    }

    //back button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2, 400, 100);
    btn->SetOnClickCallback(std::bind(&CustomMapSelectScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2 + 50, 0, 0, 0, 255, 0.5, 0.5));

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void CustomMapSelectScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void CustomMapSelectScene::EditOnClick(unsigned short map_index) {
    MapEditScene *scene = dynamic_cast<MapEditScene *>(Engine::GameEngine::GetInstance().GetScene("map-edit"));
    scene->CustomMapId = map_index;
    Engine::GameEngine::GetInstance().ChangeScene("map-edit");
}

void CustomMapSelectScene::PlayCustomOnClick(){
    //...
}

void CustomMapSelectScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("mode-select");
}