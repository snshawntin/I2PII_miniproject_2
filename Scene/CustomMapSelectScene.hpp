#ifndef CUSTOMMAPSELECTSCENE_HPP
#define CUSTOMMAPSELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class CustomMapSelectScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit CustomMapSelectScene() = default;
    void Initialize();
    void Terminate();

    void EditOnClick(unsigned short map_index);
    void PlayCustomOnClick(unsigned short map_index);
    void BackOnClick();
};

#endif   // CUSTOMMAPSELECTSCENE_HPP
