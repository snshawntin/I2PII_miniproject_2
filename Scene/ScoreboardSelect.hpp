#ifndef SCOREBOARDSELECTSCENE_HPP
#define SCOREBOARDSELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class ScoreboardSelectScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit ScoreboardSelectScene() = default;
    void Initialize() override;
    void Terminate() override;

    void NormalModeOnClick();
    void InfiniteModeOnClick();
    void MultiP1OnClick();
    void MultiP2OnClick();
    void BackOnClick();

};

#endif   // SCOREBOARDSELECTSCENE_HPP
