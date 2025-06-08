//TODO: mostly copied from playscene.cpp.
//! this is just a temporary version.

#ifndef MAPEDITSCENE_HPP
#define MAPEDITSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class MapEditScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit MapEditScene() = default;
    void Initialize();
    void Terminate();

    void EditOnClick(unsigned short map_index);
    void BackOnClick();
};

#endif   // MAPEDITSCENE_HPP
