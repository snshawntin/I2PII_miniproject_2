#include <functional>
#include <string>
#include <fstream>
#include <cstdio>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/LOG.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "WinScene.hpp"

void WinScene::Initialize() {
    ticks = 0;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Image("win/benjamin-sad.png", halfW, halfH, 0, 0, 0.5, 0.5));
    AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 - 10, 255, 255, 255, 255, 0.5, 0.5));
    
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    
    // TODO PROJECT-bonus (2): Add a text box in WinScene to record the user’s name.

    bgmId = AudioHelper::PlayAudio("win.wav");
}

void WinScene::Terminate() {
    //(END) TODO PROJECT-2 (2/5)-2: You need to save the score when the player wins.
    //(END) TODO                    (save tmp file's score into txt file)
    std::ifstream tmp_file("../Resource/new_score.tmp", std::ios::in);
    if(tmp_file.is_open()) {
        std::string score;
        std::getline(tmp_file, score);

        // TODO PROJECT-2 (3/5): Sort the scoreboard entries in a certain way.
        //std::ios::in means the mode is append ("a" mode in python)
        std::ofstream scoreboard_file("../Resource/scoreboard.txt", std::ios::app);
        if(scoreboard_file.is_open()){
            scoreboard_file << "\n";
            scoreboard_file << "new user" << " " << score;

            scoreboard_file.close();
        }
        else{
            Engine::LOG(Engine::ERROR) << "Can't open scoreboard data file";
        }

        tmp_file.close();

        //delete file
        if(std::remove("../Resource/new_score.tmp") != 0){
            Engine::LOG(Engine::ERROR) << "Can't delete temporary file";
        }
    }
    else{
        Engine::LOG(Engine::ERROR) << "Can't open temporary file";
    }

    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}

void WinScene::Update(float deltaTime) {
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}

void WinScene::BackOnClick(int stage) {
    // Change to select scene.
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
