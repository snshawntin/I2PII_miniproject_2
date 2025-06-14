#include <functional>
#include <string>
#include <fstream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/LOG.hpp"
#include "LoseScene.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/TextBox.hpp"

void LoseScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Image("lose/benjamin-happy.png", halfW, halfH - (record_score * 75), 0, 0, 0.5, 0.5));
    AddNewObject(new Engine::Label("You Lose :(", "pirulen.ttf", 48, halfW, halfH / 4 + 10 - (record_score * 20), 255, 255, 255, 255, 0.5, 0.5));
    
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&LoseScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    
    bgmInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, PlayScene::DangerTime);

    if(record_score){
        AddNewObject(new Engine::Label("Enter your name:", "pirulen.ttf", 36, halfW - 570, halfH * 7 / 4 - 155, 255, 255, 100, 255));
        AddNewControlObject(EnterNameBox = new Engine::TextBox(halfW - 50, halfH * 7 / 4 - 155, 500, 50, 36));
    }
}

void LoseScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();

    if(record_score){
        std::ifstream tmp_file("Resource/new_score.tmp", std::ios::in);
        if(tmp_file.is_open()) {
            std::string score_str;
            std::getline(tmp_file, score_str);

            //(END) TODO PROJECT-2 (3/5): Sort the scoreboard entries in a certain way. (insertion sort)
            std::ifstream scoreboard_file("Resource/scoreboard_infinite.txt", std::ios::in);
            std::ofstream new_scoreboard_file("Resource/scoreboard_infinite.tmp");
            if(new_scoreboard_file.is_open() && scoreboard_file.is_open()){
                if(EnterNameBox->text.size() == 0){
                    EnterNameBox->text = "<no name>";
                }

                std::string line;
                unsigned now_score = 2147483647, next_score;

                bool inserted = 0;
                while (std::getline(scoreboard_file, line)){
                    int old_score = std::stoi(line.substr(line.find_last_of(' ') + 1));

                    if (!inserted && std::stoi(score_str) > old_score) {
                        new_scoreboard_file << EnterNameBox->text << " " << score_str << "\n";
                        inserted = 1;
                    }
                    
                    new_scoreboard_file << line << "\n";
                }
                if (!inserted) {
                    new_scoreboard_file << EnterNameBox->text << " " << score_str << "\n";
                }
            }
            else{
                Engine::LOG(Engine::ERROR) << "Can't create (new) or open (old) scoreboard data file";
            }

            tmp_file.close();
            scoreboard_file.close();
            new_scoreboard_file.close();

            //delete the tmp file from PlayScene.cpp
            if(std::remove("Resource/new_score.tmp") != 0){
                Engine::LOG(Engine::ERROR) << "Can't delete temporary file";
            }

            //remove old scoreboard file, rename new scoreboard file
            if(std::remove("Resource/scoreboard_infinite.txt") != 0){
                Engine::LOG(Engine::ERROR) << "Can't delete old scoreboard file";
            }
            else{
                if(std::rename("Resource/scoreboard_infinite.tmp", "Resource/scoreboard_infinite.txt") != 0){
                    Engine::LOG(Engine::ERROR) << "Can't replace scoreboard file (DATA LOSS)";
                }
            }
        }
        else{
            Engine::LOG(Engine::ERROR) << "Can't open temporary file";
        }
    }
}

void LoseScene::BackOnClick(int stage) {
    // Change to select scene.
    Engine::GameEngine::GetInstance().ChangeScene("mode-select");
}
