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
#include "UI/Component/TextBox.hpp"
#include "WinScene.hpp"

void WinScene::Initialize() {
    ticks = 0;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Image("win/benjamin-sad.png", halfW, halfH - 75, 0, 0, 0.5, 0.5));
    AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 - 10, 255, 255, 255, 255, 0.5, 0.5));
    
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    
    // TODO PROJECT-bonus (2): Add a text box in WinScene to record the user’s name.
    // test the finctionality of this.
    AddNewObject(new Engine::Label("Enter your name:", "pirulen.ttf", 36, halfW - 570, halfH * 7 / 4 - 155, 255, 255, 100, 255));
    AddNewControlObject(EnterNameBox = new Engine::TextBox(halfW - 50, halfH * 7 / 4 - 155, 500, 50, 36));

    bgmId = AudioHelper::PlayAudio("win.wav");
}

void WinScene::Terminate() {
    //(END) TODO PROJECT-2 (2/5)-2: You need to save the score when the player wins.
    //(END) TODO                    (save tmp file's score into txt file)
    std::ifstream tmp_file("../Resource/new_score.tmp", std::ios::in);
    if(tmp_file.is_open()) {
        std::string score_str;
        std::getline(tmp_file, score_str);

        //(END) TODO PROJECT-2 (3/5): Sort the scoreboard entries in a certain way. (insertion sort)
        std::ifstream scoreboard_file("../Resource/scoreboard.txt", std::ios::in);
        std::ofstream new_scoreboard_file("../Resource/scoreboard.tmp");
        if(new_scoreboard_file.is_open() && scoreboard_file.is_open()){
            if(EnterNameBox->text.size() == 0){
                EnterNameBox->text = "<no name>";
            }

            std::string line;
            unsigned now_score = 2147483647, next_score;

            bool no_data = 1;
            bool inserted = 0;
            while (std::getline(scoreboard_file, line)){
                if(!no_data){
                    new_scoreboard_file << "\n";
                }

                no_data = 0;
                next_score = std::stoi(line.substr(line.find_last_of(' ') + 1, line.length()));

                if(stoi(score_str) < now_score && stoi(score_str) > next_score){
                    new_scoreboard_file << EnterNameBox->text << " " << score_str << "\n";
                }
                new_scoreboard_file << line;

                now_score = next_score;
            }
            if(no_data){ // no data, insert straightly
                new_scoreboard_file << EnterNameBox->text << " " << score_str;
            }
            if(!inserted){
                new_scoreboard_file << "\n" << EnterNameBox->text << " " << score_str;
            }
        }
        else{
            Engine::LOG(Engine::ERROR) << "Can't create (new) or open (old) scoreboard data file";
        }

        tmp_file.close();
        scoreboard_file.close();
        new_scoreboard_file.close();

        //delete the tmp file from PlayScene.cpp
        if(std::remove("../Resource/new_score.tmp") != 0){
            Engine::LOG(Engine::ERROR) << "Can't delete temporary file";
        }

        //remove old scoreboard file, rename new scoreboard file
        if(std::remove("../Resource/scoreboard.txt") != 0){
            Engine::LOG(Engine::ERROR) << "Can't delete old scoreboard file";
        }
        else{
            if(std::rename("../Resource/scoreboard.tmp", "../Resource/scoreboard.txt") != 0){
                Engine::LOG(Engine::ERROR) << "Can't replace scoreboard file (DATA LOSS)";
            }
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
    Engine::GameEngine::GetInstance().ChangeScene("mode-select");
}
