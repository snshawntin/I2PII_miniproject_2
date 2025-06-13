#ifndef LOSESCENE_HPP
#define LOSESCENE_HPP
#include "Engine/IScene.hpp"
#include "UI/Component/TextBox.hpp"
#include <allegro5/allegro_audio.h>
#include <memory>

class LoseScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    Engine::TextBox *EnterNameBox;

public:
    explicit LoseScene() = default;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(int stage);

    bool record_score = 0; //set to 1 if is infinite mode (never win)
};

#endif   // LOSESCENE_HPP
