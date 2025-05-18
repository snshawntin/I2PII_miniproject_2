#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <fstream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"
#include "PlayScene.hpp"
#include "ScoreboardScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

// TODO PROJECT-2 (2/5): You need to save the score when the player wins.
// TODO PROJECT-2 (3/5): Sort the scoreboard entries in a certain way.
// TODO PROJECT-bonus (1): Add date time information to each record and display them.
// TODO PROJECT-bonus (2): Add a text box in WinScene to record the user’s name.

void ScoreboardScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    //& title
    AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 60, halfW, halfH / 5 + 50, 10, 255, 255, 255, 0.5, 0.5));

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

    //& scoreboard contents
    //(END) TODO PROJECT-2 (5/5): The scoreboard must be stored in a file to be permanent.
    std::ifstream ifs("../Resource/scoreboard.txt", std::ios::in); //std::ios::in means the mode is input
    if (!ifs.is_open()) {
        Engine::LOG(Engine::ERROR) << "Can't open scoreboard data file";
    }

    //& now it's only 1 page (5 users for each)
    std::string line;
    unsigned line_nowat = 0, index = 0;
    while(std::getline(ifs,line)){
        unsigned space_pos = line.find(' ');

        if(line_nowat < 5 * page && line_nowat >= 5 * (page - 1)){
            AddNewObject(
                new Engine::Label(
                    line.substr(0, space_pos), "pirulen.ttf", 50, 
                    halfW - 255, halfH / 5 + 130 + (index * 55), 
                    10, 255, 10, 255, 0, 0
                )
            );
            AddNewObject(
                new Engine::Label(
                    line.substr(space_pos + 1, line.length()), "pirulen.ttf", 50,
                    halfW + 155, halfH / 5 + 130 + (index * 55),
                    10, 255, 10, 255, 0, 0
                )
            );
            index++;
        }

        line_nowat++;
    }

    line_nowat--;
    total_page = (line_nowat / 5) + 1;
    AddNewObject(
        new Engine::Label(
            "page " + std::to_string(page) + "/" + std::to_string(total_page), "pirulen.ttf", 20,
            halfW, (halfH * 7) / 5,
            10, 255, 255, 255, 0.5, 0
        )
    );
}

void ScoreboardScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

//& set the callback of buttons
//(END) TODO PROJECT-2 (4/5): The scoreboard must be multi-page.
void ScoreboardScene::OnPrevPgClick(int stage){
    if(page == 1){
        return;
    }
    page--;
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}

void ScoreboardScene::OnNextPgClick(int stage){
    if(page == total_page){
        return;
    }
    page++;
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}

void ScoreboardScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}