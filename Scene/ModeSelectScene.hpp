#ifndef MODESELECTSCENE_HPP
#define MODESELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class ModeSelectScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit ModeSelectScene() = default;
    void Initialize() override;
    void Terminate() override;

    void NormalModeOnClick();
    void InfiniteModeOnClick();
    void CustomMapOnClick();
    void BackOnClick();

};

#endif   // MODESELECTSCENE_HPP
