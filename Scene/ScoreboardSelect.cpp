#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "ScoreboardSelect.hpp"
#include "ScoreboardScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void ScoreboardSelectScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    //title
    AddNewObject(new Engine::Label("select scoreboard type", "pirulen.ttf", 60, halfW, halfH / 5 + 50, 10, 255, 255, 255, 0.5, 0.5));

    const unsigned halfdist = 50;

    //normal mode
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200 + 250, halfH / 2 + 150 - 100, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardSelectScene::NormalModeOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Normal mode", "pirulen.ttf", 36, halfW + 250, halfH / 2 + 200 - 100, 0, 0, 0, 255, 0.5, 0.5));
    
    //infinite mode
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 700 + 250, halfH / 2 + 150 - 100, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardSelectScene::InfiniteModeOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Infinite mode", "pirulen.ttf", 38, halfW - 500 + 250, halfH / 2 + 200 - 100, 0, 0, 0, 255, 0.5, 0.5));

    //multi-p1
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 700 + 250, halfH / 2 + 150 + halfdist, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardSelectScene::MultiP1OnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("multi-p1", "pirulen.ttf", 36, halfW - 500 + 250, halfH / 2 + 200 + halfdist, 0, 0, 0, 255, 0.5, 0.5));
    
    //multi-p2
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200 + 250, halfH / 2 + 150 + halfdist, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardSelectScene::MultiP2OnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("multi-p2", "pirulen.ttf", 36, halfW + 250, halfH / 2 + 200 + halfdist, 0, 0, 0, 255, 0.5, 0.5));

    //back
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardSelectScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void ScoreboardSelectScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void ScoreboardSelectScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void ScoreboardSelectScene::NormalModeOnClick() {
    ScoreboardScene *scene = dynamic_cast<ScoreboardScene *>(Engine::GameEngine::GetInstance().GetScene("scoreboard"));
    scene->type = NORMAL;
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}

void ScoreboardSelectScene::InfiniteModeOnClick() {
    ScoreboardScene *scene = dynamic_cast<ScoreboardScene *>(Engine::GameEngine::GetInstance().GetScene("scoreboard"));
    scene->type = INFINITE;
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}

void ScoreboardSelectScene::MultiP1OnClick() {
    ScoreboardScene *scene = dynamic_cast<ScoreboardScene *>(Engine::GameEngine::GetInstance().GetScene("scoreboard"));
    scene->type = MULTIP1;
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}

void ScoreboardSelectScene::MultiP2OnClick() {
    ScoreboardScene *scene = dynamic_cast<ScoreboardScene *>(Engine::GameEngine::GetInstance().GetScene("scoreboard"));
    scene->type = MULTIP2;
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}