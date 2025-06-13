#ifndef MULTIWINSCENE_HPP
#define MULTIWINSCENE_HPP
#include "Engine/IScene.hpp"
#include "UI/Component/TextBox.hpp"
#include <allegro5/allegro_audio.h>

class MultiWinScene final : public Engine::IScene {
private:
    float ticks;
    ALLEGRO_SAMPLE_ID bgmId;
    Engine::TextBox *EnterNameBox;

public:
    explicit MultiWinScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void BackOnClick(int stage);

    int which_player_win;
};

#endif   // MULTIWINSCENE_HPP
