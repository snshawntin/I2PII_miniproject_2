#ifndef MAPEDITSCENE_HPP
#define MAPEDITSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <map>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"

class Turret;
class Tool;
namespace Engine{
    class Group;
    class Image;
    class Label;
    class Sprite;
} // namespace Engine

class MapEditScene final : public Engine::IScene {
private:
    enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
        NONE
    };
    ALLEGRO_SAMPLE_ID bgmId;


public:
    bool DebugMode;

    static const std::vector<Engine::Point> directions;
    static const int MapWidth, MapHeight;
    static const int BlockSize;
    static const Engine::Point SpawnGridPoint;
    static const Engine::Point EndGridPoint;
    int CustomMapId;
    // Map tiles.
    Group *TileMapGroup;
    Group *GroundEffectGroup;
    Group *DebugIndicatorGroup;
    Group *BulletGroup;
    Group *TowerGroup;
    Group *EnemyGroup;
    Group *PlaneGroup;
    Group *EffectGroup;
    Group *UIGroup;
    TileType selected_tile;
    Engine::Label *UISelected;
    Engine::Label *ErrorMsg;
    Engine::Label *ErrorMsgExplicit;
    Engine::Image *imgTarget;
    Engine::Sprite *dangerIndicator;
    std::vector<std::vector<TileType>> mapState;
    std::vector<std::vector<int>> mapDistance;
    std::list<std::pair<int, float>> enemyWaveData;
    std::list<int> keyStrokes;
    static Engine::Point GetClientSize();

    explicit MapEditScene() = default;

    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void UpdateMapUI();
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void ReadMap();
    void ConstructUI();
    void UIBtnClicked(int id);
    void BackOnClick();
    std::vector<std::vector<int>> CalculateBFSDistance();

    std::map<std::pair<int, int>, Turret *> turret_map;
    // void ModifyReadMapTiles();
};
#endif   // MAPEDITSCENE_HPP
