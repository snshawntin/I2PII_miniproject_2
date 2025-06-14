#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <map>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Enemy/P2Base.hpp"

class Turret;
class Tool;
namespace Engine
{
    class Group;
    class Image;
    class Label;
    class Sprite;
} // namespace Engine

class PlayScene final : public Engine::IScene
{
private:
    enum TileType
    {
        TILE_DIRT,
        TILE_FLOOR,
        TILE_OCCUPIED
    };
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;

protected:
    int lives;
    int money;
    int p2money;
    int SpeedMult;
    const int upgrade_money[5] = {50, 250, 1500, 5000, 9999};
    std::vector<Engine::Image *> lifeIcons;

    Engine::Image *soldier_key, *plane_key, *tank_key, *shield_key;
    Engine::Image *soldier_unable, *plane_unable, *tank_unable, *shield_unable;

    Engine::Label *machinegun_level_text, *laser_level_text, *grow_level_text;
    Engine::Label *machinegun_upgrade_text, *laser_upgrade_text, *grow_upgrade_text;

    int p2_earn_money_cooldown;

public:
    static bool DebugMode;
    static int CheatCodeSeqNowAt;
    static const std::vector<Engine::Point> directions;
    static const int MapWidth, MapHeight;
    static const int BlockSize;
    static const float DangerTime;
    static const Engine::Point SpawnGridPoint;
    static const Engine::Point EndGridPoint;
    static const std::vector<int> code;
    Engine::Point SpawnCoordinate;
    int MapId;
    bool map_rereaded;
    float ticks;
    float deathCountDown;
    int to_win_scene_lockdown;
    bool isInfiniteMode;
    bool isMultiPlayer;
    bool IsCustom;
    float infiniteTicks = 0.0f;

    static const int soldier_enemy_price = 50;
    static const int plane_enemy_price = 230;
    static const int tank_enemy_price = 500;
    static const int shield_enemy_price = 1200;

    int soldier_enemy_cooldown;
    int plane_enemy_cooldown;
    int tank_enemy_cooldown;
    int shield_enemy_cooldown;

    int machinegunturret_lv, laserturret_lv, growturret_lv;
    int bossSpawnCount = 0;
    float bossTicks = 0.0f;
    const float bossSpawnInterval = 50;
    float bossWarningTimer = 0;
    bool bossWarningShown = false;
    Engine::Label *bossWarningLabel = nullptr;
    bool isShaking = false;
    float shakeDuration = 5.0f;
    float shakeMagnitude = 10.0f;
    Engine::Point shakeOffset = Engine::Point(0, 0);

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
    Engine::Label *UIMoney;
    Engine::Label *UIMoney_P2;
    Engine::Label *UILives;
    Engine::Image *imgTarget;
    Engine::Sprite *dangerIndicator;
    Turret *preview;
    Tool *preview_tool;
    std::vector<std::vector<TileType>> mapState;
    std::vector<std::vector<int>> mapDistance;
    std::list<std::pair<int, float>> enemyWaveData;
    std::list<int> keyStrokes;
    P2Base *p2_base;
    static Engine::Point GetClientSize();
    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void Hit();

    int GetMoney() const;
    void EarnMoney(int money);
    void P2EarnMoney(int money);
    void ReadMap();
    void ReadEnemyWave();
    void ConstructUI();
    void UIBtnClicked(int id);
    void UIBtnRightClicked(int id);
    bool CheckSpaceValid(int x, int y);
    std::vector<std::vector<int>> CalculateBFSDistance();
    void UpdateLifeIcons(bool isP2);
    void StartShake(float duration, float magnitude);
    void AutoBuild();
    void ToggleAutoBuild();
    std::map<std::pair<int, int>, Turret *> turret_map;
    Turret *CreateTurret(int type, int x, int y);

    // void ModifyReadMapTiles();
};
#endif // PLAYSCENE_HPP
