#ifndef MULTIWIN_HPP
#define MULTIWIN_HPP
#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>
#include <memory>

class MultiWinScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit MultiWinScene() = default;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(int stage);

    int which_player_win;
};

#endif   // MULTIWIN_HPP
