#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "ScoreboardScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

// TODO PROJECT-2 (2/5): You need to save the score when the player wins.
// TODO PROJECT-2 (3/5): Sort the scoreboard entries in a certain way.
// TODO PROJECT-2 (5/5): The scoreboard must be stored in a file to be permanent.
// TODO PROJECT-bonus (1): Add date time information to each record and display them.
// TODO PROJECT-bonus (2): Add a text box in WinScene to record the user’s name.

void ScoreboardScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    //& title
    AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 50, halfW, halfH / 5 + 50, 10, 255, 255, 255, 0.5, 0.5));

    //(END) TODO PROJECT-2 (1/5)-2: Add a way to exit ScoreboardScene.
    Engine::ImageButton *btn;

    //& exit button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 + 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2 + 100, 0, 0, 0, 255, 0.5, 0.5));

    //& prev page button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 700, halfH * 3 / 2 + 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::OnPrevPgClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Prev Page", "pirulen.ttf", 48, halfW - 500, halfH * 3 / 2 + 100, 0, 0, 0, 255, 0.5, 0.5));

    //& next page button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 300, halfH * 3 / 2 + 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::OnNextPgClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Next Page", "pirulen.ttf", 48, halfW + 500, halfH * 3 / 2 + 100, 0, 0, 0, 255, 0.5, 0.5));

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void ScoreboardScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

//& set the callback of buttons
// TODO PROJECT-2 (4/5): The scoreboard must be multi-page.
void ScoreboardScene::OnPrevPgClick(int stage){
    //...
}
void ScoreboardScene::OnNextPgClick(int stage){
    //...
}

void ScoreboardScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

