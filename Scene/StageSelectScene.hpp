#ifndef STAGESELECTSCENE_HPP
#define STAGESELECTSCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"

class StageSelectScene final : public Engine::IScene
{
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

    Engine::Label *modeLabel = nullptr; // 顯示目前模式的 Label 指標

public:
    explicit StageSelectScene() = default;
    void Initialize() override;
    void Terminate() override;

    void PlayOnClick(int stage);
    void ScoreboardOnClick();
    void BackOnClick(int stage);

    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);

    void ToggleInfiniteMode(); // 切換無限模式
    bool isInfiniteMode = false;
};

#endif // STAGESELECTSCENE_HPP
