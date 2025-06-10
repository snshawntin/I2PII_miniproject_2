#include <algorithm>
#include <allegro5/allegro.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <fstream>

#include <iostream>

#include "Enemy/Enemy.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Enemy/ShieldEnemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/GrowTurret.hpp"
#include "Turret/TurretButton.hpp"
#include "Tool/ToolButton.hpp"
#include "Tool/Tool.hpp"
#include "Tool/Shovel.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/Label.hpp"

bool PlayScene::DebugMode = false;
int PlayScene::CheatCodeSeqNowAt = 0;
const std::vector<Engine::Point> PlayScene::directions = {Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1)};
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);

//(END) TODO HACKATHON-4 (2/3): Find the cheat code sequence in this file.
const std::vector<int> PlayScene::code = {
    ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEY_RSHIFT, ALLEGRO_KEY_ENTER};

Engine::Point PlayScene::GetClientSize()
{
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize()
{

    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    money = 150;
    SpeedMult = 1;
    to_win_scene_lockdown = -1;
    infiniteTicks = 0.0f;
    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(TowerGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(PlaneGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ReadMap();
    ReadEnemyWave();
    mapDistance = CalculateBFSDistance();
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    preview = nullptr;
    preview_tool = nullptr;
    UIGroup->AddNewObject(imgTarget);
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("play.ogg");
}
void PlayScene::Terminate()
{
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime)
{
    // if (isInfiniteMode)
    //     printf("INF");
    // else
    //     printf("NORM");
    // If we use deltaTime directly, then we might have Bullet-through-paper problem.
    // Reference: Bullet-Through-Paper
    if(!map_rereaded){
        ReadMap();
        map_rereaded = 1;
    }

    if (SpeedMult == 0)
        deathCountDown = -1;
    else if (deathCountDown != -1)
        SpeedMult = 1;
    // Calculate danger zone.
    std::vector<float> reachEndTimes;
    for (auto &it : EnemyGroup->GetObjects())
    {
        reachEndTimes.push_back(dynamic_cast<Enemy *>(it)->reachEndTime);
    }
    // Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
    std::sort(reachEndTimes.begin(), reachEndTimes.end());
    float newDeathCountDown = -1;
    int danger = lives;
    for (auto &it : reachEndTimes)
    {
        if (it <= DangerTime)
        {
            danger--;
            if (danger <= 0)
            {
                // Death Countdown
                float pos = DangerTime - it;
                if (it > deathCountDown)
                {
                    // Restart Death Count Down BGM.
                    AudioHelper::StopSample(deathBGMInstance);
                    if (SpeedMult != 0)
                        deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
                }
                float alpha = pos / DangerTime;
                alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
                dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
                newDeathCountDown = it;
                break;
            }
        }
    }
    deathCountDown = newDeathCountDown;
    if (SpeedMult == 0)
        AudioHelper::StopSample(deathBGMInstance);
    if (deathCountDown == -1 && lives > 0)
    {
        AudioHelper::StopSample(deathBGMInstance);
        dangerIndicator->Tint.a = 0;
    }
    if (SpeedMult == 0)
        deathCountDown = -1;
    for (int i = 0; i < SpeedMult; i++)
    {
        IScene::Update(deltaTime);
        // Check if we should create new enemy.
        ticks += deltaTime;
        infiniteTicks += deltaTime;

        //(END) TODO HACKATHON-5 (1/4): There's a bug in this file, which crashes the game when you win. Try to find it.
        if (enemyWaveData.empty())
        {
            if (EnemyGroup->GetObjects().empty())
            {
                // Free resources.
                //  delete TileMapGroup;
                //  delete GroundEffectGroup;
                //  delete DebugIndicatorGroup;
                //  delete TowerGroup;
                //  delete EnemyGroup;
                //  delete BulletGroup;
                //  delete EffectGroup;
                //  delete UIGroup;
                //  delete imgTarget;

                if (to_win_scene_lockdown == -1)
                {
                    to_win_scene_lockdown = 60;
                }
                else
                {
                    to_win_scene_lockdown--;
                }

                // Win.
                //(END) TODO PROJECT-2 (2/5)-1: You need to save the score when the player wins.
                //(END) TODO                    (save score in tmp file)
                // TODO PROJECT-bonus (1): Add date time information to each record and display them.
                if (to_win_scene_lockdown == 0)
                {
                    //& ofstream: write(**output**) data in code to files.
                    //& just like "w" mode in python
                    std::ofstream tmp_file("../Resource/new_score.tmp");
                    if (tmp_file.is_open())
                    {
                        tmp_file << money; // score

                        tmp_file.close();
                    }
                    else
                    {
                        Engine::LOG(Engine::ERROR) << "Can't create temporary file";
                    }

                    Engine::GameEngine::GetInstance().ChangeScene("win");
                }
            }
            continue;
        }

        Enemy *enemy = nullptr;
        if (!isInfiniteMode)
        {
            auto current = enemyWaveData.front();
            if (ticks < current.second)
                continue;
            ticks -= current.second;
            enemyWaveData.pop_front();
            const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
            switch (current.first)
            {
            case 1:
                EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 2:
                EnemyGroup->AddNewObject(enemy = new ShieldEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 3:
                EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 4:
                //planeenemy is deleted in mini2, now its back.
                EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            default:
                continue;
            }
            enemy->UpdatePath(mapDistance);
            enemy->Update(ticks);
        }
        else
        {
            // 無限模式：隨機生成敵人，且難度隨時間增加
            const Engine::Point SpawnCoordinate = Engine::Point(
                SpawnGridPoint.x * BlockSize + BlockSize / 2,
                SpawnGridPoint.y * BlockSize + BlockSize / 2);

            static float infiniteSpawnInterval = 3.3f; // 初始間隔 3.3 秒
            static float infiniteTimer = 0.0f;
            infiniteTimer += deltaTime;

            // 隨著遊戲時間增加，降低生成間隔，加快敵人生成速度（最小間隔 0.3 秒）
            float difficultyFactor = std::max(0.3f, 2.0f - infiniteTimer * 0.003f);
            infiniteSpawnInterval = difficultyFactor;

            if (infiniteTimer >= infiniteSpawnInterval)
            {
                infiniteTimer = 0.0f;

                // 隨機選敵人類型，依照時間變化機率
                
                //& 註：我把原本在shield_enemy位置的plane_enemy加回來了，所以壓縮到一點tank_enemy的生成機率
                //& 但是整體體驗應該不會改變太多
                int enemyType;
                float r = static_cast<float>(rand()) / RAND_MAX;

                if (infiniteTicks < 90)
                {
                    enemyType = (r < 0.8f) ? 1 : 4;
                }
                else if (infiniteTicks < 300)
                {
                    if (r < 0.5f)
                        enemyType = 1;
                    else if (r < 0.75f)
                        enemyType = 4;
                    else if (r < 0.85f)
                        enemyType = 3;
                    else
                        enemyType = 2;
                }
                else if (infiniteTicks < 1000)
                {
                    if (r < 0.3f)
                        enemyType = 1;
                    else if (r < 0.6f)
                        enemyType = 2;
                    else if (r > 0.85f)
                        enemyType = 4;
                    else
                        enemyType = 3;
                }
                else
                {
                    if (r < 0.5)
                        enemyType = 2;
                    else if (r > 0.85f)
                        enemyType = 4;
                    else 
                        enemyType = 3;
                }

                Enemy *enemy = nullptr;
                switch (enemyType)
                {
                case 1:
                    enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y);
                    break;
                case 2:
                    enemy = new ShieldEnemy(SpawnCoordinate.x, SpawnCoordinate.y);
                    break;
                case 3:
                    enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y);
                    break;
                case 4:
                    enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y);
                    break;
                }

                if (enemy)
                {
                    EnemyGroup->AddNewObject(enemy);
                    enemy->UpdatePath(mapDistance);
                }
            }
        }
    }
    if (preview)
    {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview->Update(deltaTime);
    }
    if (preview_tool)
    {
        preview_tool->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview_tool->Update(deltaTime);
    }
}
void PlayScene::Draw() const
{
    IScene::Draw();
    if (DebugMode)
    {
        // Draw reverse BFS distance on all reachable blocks.
        for (int i = 0; i < MapHeight; i++)
        {
            for (int j = 0; j < MapWidth; j++)
            {
                if (mapDistance[i][j] != -1)
                {
                    // Not elegant nor efficient, but it's quite enough for debugging.
                    Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
                    label.Anchor = Engine::Point(0.5, 0.5);
                    label.Draw();
                }
            }
        }
    }
}
void PlayScene::OnMouseDown(int button, int mx, int my)
{
    if ((button & 1) && !imgTarget->Visible && preview)
    {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    if ((button & 1) && !imgTarget->Visible && preview_tool)
    {
        // Cancel tool construct.
        UIGroup->RemoveObject(preview_tool->GetObjectIterator());
        preview_tool = nullptr;
    }
    IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my)
{
    IScene::OnMouseMove(mx, my);
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if ((!preview && !preview_tool) || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
    {
        imgTarget->Visible = false;
        return;
    }
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}
void PlayScene::OnMouseUp(int button, int mx, int my)
{
    IScene::OnMouseUp(button, mx, my);
    if (!imgTarget->Visible)
        return;
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (button & 1)
    {
        if (preview_tool || mapState[y][x] != TILE_OCCUPIED)
        {
            if (!preview && !preview_tool)
            {
                return;
            }
            // Check if valid.
            if (!CheckSpaceValid(x, y))
            {
                Engine::Sprite *sprite;
                GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
                sprite->Rotation = 0;
                return;
            }

            if (preview)
            {
                // Purchase.
                EarnMoney(-preview->GetPrice());
                // Remove Preview.
                preview->GetObjectIterator()->first = false;
                UIGroup->RemoveObject(preview->GetObjectIterator());
                // Construct real turret.
                preview->Position.x = x * BlockSize + BlockSize / 2;
                preview->Position.y = y * BlockSize + BlockSize / 2;
                preview->Enabled = true;
                preview->Preview = false;
                preview->Tint = al_map_rgba(255, 255, 255, 255);
                TowerGroup->AddNewObject(preview);
                turret_map[std::make_pair(preview->Position.x, preview->Position.y)] = preview;

                // To keep responding when paused.
                preview->Update(0);
                // Remove Preview.
                preview = nullptr;

                mapState[y][x] = TILE_OCCUPIED;
            }
            else if (preview_tool)
            {
                // Remove Preview.
                preview_tool->GetObjectIterator()->first = false;
                UIGroup->RemoveObject(preview_tool->GetObjectIterator());
                // real tool operated.
                std::pair<int, int> shovel_place = std::make_pair(x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2);
                if(turret_map.find(shovel_place) != turret_map.end()){
                    EarnMoney(turret_map[shovel_place]->GetPrice() * 0.5);
                    TowerGroup->RemoveObject(turret_map[shovel_place]->GetObjectIterator());
                    turret_map.erase(shovel_place);
                }
                // To keep responding when paused.
                preview_tool->Update(0);
                // Remove Preview.
                preview_tool = nullptr;

                //& restore map state to let the turret can be placed again.
                if(mapState[y][x] == TILE_OCCUPIED){
                    mapState[y][x] = TILE_DIRT;
                }
            }
            
            OnMouseMove(mx, my);
        }
    }
}

//(END) TODO HACKATHON-4 (1/3): Trace how the game handles keyboard input.
void PlayScene::OnKeyDown(int keyCode)
{
    IScene::OnKeyDown(keyCode);

    //(END) TODO HACKATHON-4 (3/3): When the cheat code is entered, a plane should
    //(END)                         be spawned and added to the scene.
    if (keyCode == code[CheatCodeSeqNowAt])
    {
        if (CheatCodeSeqNowAt == 11)
        {
            Plane *new_plane;
            PlaneGroup->AddNewObject(new_plane = new Plane());
            EarnMoney(10000);

            CheatCodeSeqNowAt = 0;
        }
        else
        {
            CheatCodeSeqNowAt++;
        }
    }
    else
    {
        CheatCodeSeqNowAt = 0;
    }

    if (keyCode == ALLEGRO_KEY_TAB)
    {
        DebugMode = !DebugMode;
    }
    else
    {
        keyStrokes.push_back(keyCode);
        if (keyStrokes.size() > code.size())
            keyStrokes.pop_front();
    }
    if (keyCode == ALLEGRO_KEY_Q)
    {
        // Hotkey for MachineGunTurret.
        UIBtnClicked(0);
    }
    else if (keyCode == ALLEGRO_KEY_W)
    {
        // Hotkey for LaserTurret.
        UIBtnClicked(1);
    }
    else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9)
    {
        // Hotkey for Speed up.
        SpeedMult = keyCode - ALLEGRO_KEY_0;
    }
}

//(END) TODO HACKATHON-5 (2/4): The "LIFE" label are not updated when you lose a life. Try to fix it.
void PlayScene::Hit()
{
    lives--;
    UpdateLifeIcons();
    if (lives <= 0)
    {
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }
}
int PlayScene::GetMoney() const
{
    return money;
}
void PlayScene::EarnMoney(int money)
{
    this->money += money;
    UIMoney->Text = std::string("$") + std::to_string(this->money);
}
void PlayScene::ReadMap()
{
    std::string filename;
    if(!IsCustom){
        filename = std::string("../Resource/map") + std::to_string(MapId) + ".txt";
    }
    else{
        filename = std::string("../Resource/custom_map/cm0") + std::to_string(MapId) + ".txt";
    }

    // Read map file.
    char c;
    std::vector<bool> mapData;
    std::ifstream fin(filename);
    while (fin >> c)
    {
        switch (c)
        {
        case '0':
            mapData.push_back(false);
            break;
        case '1':
            mapData.push_back(true);
            break;
        case '\n':
        case '\r':
            if (static_cast<int>(mapData.size()) / MapWidth != 0)
                throw std::ios_base::failure("Map data is corrupted.");
            break;
        default:
            throw std::ios_base::failure("Map data is corrupted.");
        }
    }
    fin.close();
    // Validate map data.
    if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
        throw std::ios_base::failure("Map data is corrupted.");
    // Store map in 2d array.
    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    for (int i = 0; i < MapHeight; i++)
    {
        for (int j = 0; j < MapWidth; j++)
        {
            const int num = mapData[i * MapWidth + j];
            mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
            if (num)
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            else
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
        }
    }
}
void PlayScene::ReadEnemyWave()
{
    std::string filename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
    // Read enemy file.
    float type, wait, repeat;
    enemyWaveData.clear();
    std::ifstream fin(filename);
    while (fin >> type && fin >> wait && fin >> repeat)
    {
        for (int i = 0; i < repeat; i++)
            enemyWaveData.emplace_back(type, wait);
    }
    fin.close();
}
void PlayScene::UpdateLifeIcons()
{
    int hearts = lives / 2;
    bool half = lives % 2;

    for (int i = 0; i < 5; ++i)
    {
        if (i < hearts)
        {
            lifeIcons[i]->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/full.png"));
        }
        else if (i == hearts && half)
        {
            lifeIcons[i]->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/half.png"));
        }
        else
        {
            lifeIcons[i]->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/empty.png")); // optional fallback
        }
    }
}

void PlayScene::ConstructUI()
{
    // Background
    UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));

    // Text
    if(!IsCustom){
        UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
    }
    else{
        UIGroup->AddNewObject(new Engine::Label(std::string("custom stage ") + std::to_string(MapId), "pirulen.ttf", 24, 1294, 0));
    }

    UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 48));
    
    // Add 5 heart icons (each is 24*26), spaced by 30 pixels.
    for (int i = 0; i < 5; ++i)
    {
        auto icon = new Engine::Image("play/full.png", 1294 + i * 30, 88, 32, 32);
        lifeIcons.push_back(icon);
        UIGroup->AddNewObject(icon);
    }
    UpdateLifeIcons(); // initialize
    TurretButton *btn;

    // Button 1 (machine gun turret)
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1294, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-1.png", 1294, 136 - 8, 0, 0, 0, 0), 1294, 136, MachineGunTurret::Price);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);

    // Button 2 (grow turret)
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1370, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-6.png", 1370, 136 - 8, 0, 0, 0, 0), 1370, 136, GrowTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);

    // Button 3 (laser turret)
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1446, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-2.png", 1446, 136 - 8, 0, 0, 0, 0), 1446, 136, LaserTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);

    // Button 4 (shovel)
    ToolButton *tool_btn;
    tool_btn = new ToolButton("play/floor.png", "play/dirt.png",
                              Engine::Sprite("play/shovel.png", 1299, 291, 54, 54, 0, 0),
                              1294, 286);
    tool_btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 100));
    UIGroup->AddNewControlObject(tool_btn);

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int shift = 135 + 25;
    dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
    dangerIndicator->Tint.a = 0;
    UIGroup->AddNewObject(dangerIndicator);
}

void PlayScene::UIBtnClicked(int id)
{
    if (preview)
    {
        UIGroup->RemoveObject(preview->GetObjectIterator());
    }
    if (preview_tool)
    {
        UIGroup->RemoveObject(preview_tool->GetObjectIterator());
    }

    if (id == 0 && money >= MachineGunTurret::Price)
    {
        preview = new MachineGunTurret(0, 0);
    }
    else if (id == 1 && money >= GrowTurret::Price)
    {
        preview = new GrowTurret(0, 0);
    }
    else if (id == 2 && money >= LaserTurret::Price)
    {
        preview = new LaserTurret(0, 0);
    }
    else if (id == 100)
    {
        preview_tool = new Shovel(0, 0);
    }

    if (preview)
    {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        preview->Tint = al_map_rgba(255, 255, 255, 200);
        preview->Enabled = false;
        preview->Preview = true;
        UIGroup->AddNewObject(preview);
        OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
    }
    else if (preview_tool)
    {
        preview_tool->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        preview_tool->Tint = al_map_rgba(255, 255, 255, 200);
        preview_tool->Enabled = false;
        preview_tool->Preview = true;
        UIGroup->AddNewObject(preview_tool);
        OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
    }
}

bool PlayScene::CheckSpaceValid(int x, int y)
{
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
        return false;
    auto map00 = mapState[y][x];
    mapState[y][x] = TILE_OCCUPIED;
    std::vector<std::vector<int>> map = CalculateBFSDistance();
    mapState[y][x] = map00;
    if (map[0][0] == -1)
        return false;
    for (auto &it : EnemyGroup->GetObjects())
    {
        Engine::Point pnt;
        pnt.x = floor(it->Position.x / BlockSize);
        pnt.y = floor(it->Position.y / BlockSize);
        if (pnt.x < 0)
            pnt.x = 0;
        if (pnt.x >= MapWidth)
            pnt.x = MapWidth - 1;
        if (pnt.y < 0)
            pnt.y = 0;
        if (pnt.y >= MapHeight)
            pnt.y = MapHeight - 1;
        if (map[pnt.y][pnt.x] == -1)
            return false;
    }
    // All enemy have path to exit.
    mapState[y][x] = TILE_OCCUPIED;
    mapDistance = map;
    for (auto &it : EnemyGroup->GetObjects())
        dynamic_cast<Enemy *>(it)->UpdatePath(mapDistance);
    return true;
}

const Engine::Point bfs_dxdy[4] = {
    Engine::Point(1, 0), Engine::Point(-1, 0), Engine::Point(0, 1), Engine::Point(0, -1)};

std::vector<std::vector<int>> PlayScene::CalculateBFSDistance()
{
    // Reverse BFS to find path.
    //& map: the path distance 2D array constructed by BFS. (if not path, val. will be -1)
    std::vector<std::vector<int>> map(MapHeight, std::vector<int>(MapWidth, -1));
    std::queue<Engine::Point> que;
    // Push end point.
    // BFS from end point.
    if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
        return map;
    que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
    map[MapHeight - 1][MapWidth - 1] = 0;

    //(END) TODO PROJECT-1 (1/1): Implement a BFS starting from the most right-bottom block in the map.
    //               For each step you should assign the corresponding distance to the most right-bottom block.
    //               mapState[y][x] is TILE_DIRT if it is empty.
    while (!que.empty()) {
        Engine::Point observing_point = que.front();
        que.pop();
        for (int i = 0; i < 4; i++) {
            int next_x = observing_point.x + bfs_dxdy[i].x;
            int next_y = observing_point.y + bfs_dxdy[i].y;
            if (next_x < 0 || next_x >= MapWidth || next_y < 0 || next_y >= MapHeight)
                continue;
            if (map[next_y][next_x] != -1)
                continue;
            if (mapState[next_y][next_x] != TILE_DIRT)
                continue;
            map[next_y][next_x] = map[observing_point.y][observing_point.x] + 1;
            que.push(Engine::Point(next_x, next_y));
        }
    }
    

    return map;
}
