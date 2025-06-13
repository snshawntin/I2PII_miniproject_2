#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

enum ScoreBoardType { NORMAL, INFINITE, MULTIP1, MULTIP2 };

class ScoreboardScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    unsigned page = 1;
    unsigned total_page;

public:
    explicit ScoreboardScene() = default;
    void Initialize() override;
    void Terminate() override;

    ScoreBoardType type;

    void OnNextPgClick(int stage);
    void OnPrevPgClick(int stage);
    void BackOnClick(int stage);
};

#endif   // SCOREBOARDSCENE_HPP
