#ifndef CUSTOMMODESELECTSCENE_HPP
#define CUSTOMMODESELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class CustomModeSelectScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit CustomModeSelectScene() = default;
    void Initialize() override;
    void Terminate() override;

    void MultiModeOnClick();
    void InfiniteModeOnClick();
    void BackOnClick();

    unsigned short custom_map_index;
};

#endif   // CUSTOMMODESELECTSCENE_HPP
