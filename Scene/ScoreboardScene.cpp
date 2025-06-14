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

void ScoreboardScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    //& title & filepath
    //! if the scoreboard is not updating, modify here.
    std::string scoreboard_filepath = "";
    switch(type){
    case NORMAL:
        AddNewObject(new Engine::Label("Scoreboard-normal mode", "pirulen.ttf", 60, halfW, halfH / 5 + 50, 10, 255, 255, 255, 0.5, 0.5));
        scoreboard_filepath = "Resource/scoreboard.txt";
        break;
    case INFINITE:
        AddNewObject(new Engine::Label("Scoreboard-infinite mode", "pirulen.ttf", 60, halfW, halfH / 5 + 50, 10, 255, 255, 255, 0.5, 0.5));
        scoreboard_filepath = "Resource/scoreboard_infinite.txt";
        break;
    case MULTIP1:
        AddNewObject(new Engine::Label("Scoreboard-multi mode(P1)", "pirulen.ttf", 60, halfW, halfH / 5 + 50, 10, 255, 255, 255, 0.5, 0.5));
        scoreboard_filepath = "Resource/scoreboard_multi_def.txt";
        break;
    case MULTIP2:
        AddNewObject(new Engine::Label("Scoreboard-multi mode(P2)", "pirulen.ttf", 60, halfW, halfH / 5 + 50, 10, 255, 255, 255, 0.5, 0.5));
        scoreboard_filepath = "Resource/scoreboard_multi_atk.txt";
        break;
    }

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
    //& ifstream: input (**read**) file data to code.
    //& just like "r" mode in python
    //(END) TODO PROJECT-2 (5/5): The scoreboard must be stored in a file to be permanent.
    std::ifstream ifs(scoreboard_filepath, std::ios::in); //std::ios::in means the mode is input
    if (!ifs.is_open()) {
        Engine::LOG(Engine::ERROR) << "Can't open scoreboard data file";
    }

    std::string line;
    unsigned line_nowat = 0, index = 0;
    while(std::getline(ifs,line)){
        //this let the user's name can have whitespace
        unsigned space_pos = line.find_last_of(' ');

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
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard-select");
}