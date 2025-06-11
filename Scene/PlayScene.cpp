#include <algorithm>
#include <allegro5/allegro.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <random>
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
#include "Enemy/P2Base.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "MultiWin.hpp"
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

//! KNOWN BUG: THE PLAYER CAN PUT TURRET ON (SOME) PATH WHEN IN CUSTOM MAP.
//! WILL FIX IT IN NEXT COMMIT. SRY

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

void PlayScene::Initialize(){
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    money = 150;
    SpeedMult = 1;
    to_win_scene_lockdown = -1;
    infiniteTicks = 0.0f;
    p2_earn_money_cooldown = 30;
    machinegunturret_lv = growturret_lv = laserturret_lv = 0;
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

    //set base.
    if(isMultiPlayer){
        EnemyGroup->AddNewObject(p2_base = new P2Base(BlockSize / 2, BlockSize / 2));
        p2_base->UpdatePath(mapDistance);
    }
    else{
        p2_base = nullptr;
    }

    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    preview = nullptr;
    preview_tool = nullptr;
    UIGroup->AddNewObject(imgTarget);
    SpawnCoordinate = Engine::Point(
        SpawnGridPoint.x * BlockSize + BlockSize / 2,
        SpawnGridPoint.y * BlockSize + BlockSize / 2
    );

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

                //& will not enable death indicator in multi mode.
                if(!isMultiPlayer){
                    float alpha = pos / DangerTime;
                    alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
                    dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
                    newDeathCountDown = it;
                    break;
                }
            }
        }
    }
    deathCountDown = newDeathCountDown;
    if (SpeedMult == 0)
        AudioHelper::StopSample(deathBGMInstance);
    if (deathCountDown == -1 && lives > 0)
    {
        AudioHelper::StopSample(deathBGMInstance);
        if(!isMultiPlayer){
            dangerIndicator->Tint.a = 0;
        }
    }
    if (SpeedMult == 0)
        deathCountDown = -1;

    for (int i = 0; i < SpeedMult; i++)
    {
        IScene::Update(deltaTime);
        // Check if we should create new enemy.
        ticks += deltaTime;
        infiniteTicks += deltaTime;

        if (!isMultiPlayer && enemyWaveData.empty())
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

                if (to_win_scene_lockdown == -1){ to_win_scene_lockdown = 60; }
                else{ to_win_scene_lockdown--; }

                // Win.
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
        else if(isMultiPlayer){
            if (p2_base == nullptr || p2_base->gethp() <= 0){
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

                if (to_win_scene_lockdown == -1){ to_win_scene_lockdown = 120; }
                else{ to_win_scene_lockdown--; }

                // Win.
                if (to_win_scene_lockdown == 0){
                    MultiWinScene *scene = dynamic_cast<MultiWinScene *>(Engine::GameEngine::GetInstance().GetScene("multi-win"));
                    scene->which_player_win = 1;
                    Engine::GameEngine::GetInstance().ChangeScene("multi-win");
                }
            }
            continue;
        }

        Enemy *enemy = nullptr;
        if (!isInfiniteMode && !isMultiPlayer) { // normal mode
            auto current = enemyWaveData.front();
            if (ticks < current.second)
                continue;
            ticks -= current.second;
            enemyWaveData.pop_front();
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
        else if (isInfiniteMode) {
            // 無限模式：隨機生成敵人，且難度隨時間增加
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

    if(isMultiPlayer){
        //&reduce the cooldown/update the indicators.
        if(soldier_enemy_cooldown != 0){
            soldier_key->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/keyboard_a_cd.png"));
            soldier_unable->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/no_time.png"));
            soldier_enemy_cooldown--; 
        }
        else if(p2money < soldier_enemy_price){
            soldier_key->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/keyboard_a_cd.png"));
            soldier_unable->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/no_money.png"));
        }
        else{
            soldier_key->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/keyboard_a.png"));
            soldier_unable->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/empty.png"));
        }

        if(plane_enemy_cooldown != 0){
            plane_key->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/keyboard_s_cd.png"));
            plane_unable->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/no_time.png"));
            plane_enemy_cooldown--; 
        }
        else if(p2money < plane_enemy_price){
            plane_key->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/keyboard_s_cd.png"));
            plane_unable->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/no_money.png"));
        }
        else{
            plane_key->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/keyboard_s.png"));
            plane_unable->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/empty.png"));
        }

        if(tank_enemy_cooldown != 0){
            tank_key->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/keyboard_d_cd.png"));
            tank_unable->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/no_time.png"));
            tank_enemy_cooldown--; 
        }
        else if(p2money < tank_enemy_price){
            tank_key->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/keyboard_d_cd.png"));
            tank_unable->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/no_money.png"));
        }
        else{
            tank_key->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/keyboard_d.png"));
            tank_unable->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/empty.png"));
        }

        if(shield_enemy_cooldown != 0){
            shield_key->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/keyboard_f_cd.png"));
            shield_unable->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/no_time.png"));
            shield_enemy_cooldown--; 
        }
        else if(p2money < shield_enemy_price){
            shield_key->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/keyboard_f_cd.png"));
            shield_unable->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/no_money.png"));
        }
        else{
            shield_key->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/keyboard_f.png"));
            shield_unable->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/empty.png"));
        }

        //p2 will automatically earn money.(random value)
        if(p2_earn_money_cooldown > 0){ p2_earn_money_cooldown--; }
        else{
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist(50, 200);
            P2EarnMoney(dist(rng));

            p2_earn_money_cooldown += 60;
        }

        //update p2 lives.
        UpdateLifeIcons(1);
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

void PlayScene::OnKeyDown(int keyCode)
{
    IScene::OnKeyDown(keyCode);

    //cheatcode...
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

    //hotkeys...
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

    //multiplayer...
    if(isMultiPlayer){        
        //a: solder, s:plane, d:tank, f:shield
        Enemy *enemy = nullptr;
        if(keyCode == ALLEGRO_KEY_A && soldier_enemy_cooldown == 0 && p2money >= soldier_enemy_price){
            EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            soldier_enemy_cooldown += 10;
            p2money -= 50;
        }
        else if(keyCode == ALLEGRO_KEY_S && plane_enemy_cooldown == 0 && p2money >= plane_enemy_price){
            EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            plane_enemy_cooldown += 80;
            p2money -= 230;
        }        
        else if(keyCode == ALLEGRO_KEY_D && tank_enemy_cooldown == 0 && p2money >= tank_enemy_price){
            EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            tank_enemy_cooldown += 180;
            p2money -= 500;
        }
        else if(keyCode == ALLEGRO_KEY_F && shield_enemy_cooldown == 0 && p2money >= shield_enemy_price){
            EnemyGroup->AddNewObject(enemy = new ShieldEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            shield_enemy_cooldown += 300;
            p2money -= 1200;
        }
        
        if(enemy != nullptr){
            enemy->UpdatePath(mapDistance);
        }
    }
}

void PlayScene::Hit()
{
    lives--;
    UpdateLifeIcons(0);
    if(isMultiPlayer){
        P2EarnMoney(999);
    }

    if (lives <= 0)
    {
        MultiWinScene *scene = dynamic_cast<MultiWinScene *>(Engine::GameEngine::GetInstance().GetScene("multi_win"));
        scene->which_player_win = 2;
        Engine::GameEngine::GetInstance().ChangeScene("multi-win");
    }
}

int PlayScene::GetMoney() const
{
    return money;
}

void PlayScene::EarnMoney(int money)
{
    this->money += money;

    //& max money:99999
    std::string max_money_indication = "";
    if(this->money > 99999){
        this->money = 99999;
        max_money_indication = "(max)";
    }

    UIMoney->Text = "money: " + std::string("$") + std::to_string(this->money) + max_money_indication;
}

void PlayScene::P2EarnMoney(int money)
{
    p2money += money;

    std::string max_money_indication = "";
    if(p2money > 99999){
        p2money = 99999;
        max_money_indication = "(max)";
    }

    UIMoney_P2->Text = "money: " + std::string("$") + std::to_string(p2money) + max_money_indication;
}

void PlayScene::ReadMap()
{
    std::string filename;
    if(!IsCustom){
        filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    }
    else{
        filename = std::string("Resource/custom_map/cm0") + std::to_string(MapId) + ".txt";
    }

    // Read map file.
    char c;
    std::vector<bool> mapData;
    std::ifstream fin(filename);
    while (fin >> c)
    {
        //std::cout << "read a char." << std::endl;
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
    if(!isMultiPlayer){
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
    else{
        enemyWaveData.clear();
    }
}

void PlayScene::UpdateLifeIcons(bool isP2){
    int hearts;
    bool half;

    if(isP2){
        int hp2 = p2_base->gethp();
        hearts = hp2 / 20;
        half = (hp2 / 10) % 2;
    }
    else{
        hearts = lives / 2;
        half = lives % 2;
    }

    for (int i = (int)isP2 * 5; i < (int)isP2 * 5 + 5; ++i)
    {
        if(i >= lifeIcons.size()){
            return;
        }

        if (i < (int)isP2 * 5 + hearts){
            lifeIcons[i]->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/full.png"));
        }
        else if (i == (int)isP2 * 5 + hearts && half){
            lifeIcons[i]->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/half.png"));
        }
        else{
            lifeIcons[i]->SetBitmap(Engine::Resources::GetInstance().GetBitmap("play/empty.png")); // optional fallback
        }
    }
}

void PlayScene::ConstructUI()
{
    //& Background
    UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));

    //& stage name
    if(!IsCustom){
        UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
    }
    else{
        UIGroup->AddNewObject(new Engine::Label(std::string("custom stage ") + std::to_string(MapId), "pirulen.ttf", 24, 1294, 0));
    }

    //& "P1"
    if(isMultiPlayer){
        UIGroup->AddNewObject(new Engine::Label(std::string("player 1"), "pirulen.ttf", 32, 1294, 48));
    }

    int multi_ui_btn_shift = (isMultiPlayer) ? 50 : 0;

    //& p1 money
    UIGroup->AddNewObject(UIMoney = new Engine::Label("money: " + std::string("$") + std::to_string(money), "pirulen.ttf", 25, 1294, 36 + multi_ui_btn_shift));

    //& p1 life
    // Add 5 heart icons (each is 24*26), spaced by 30 pixels.
    for (int i = 0; i < 5; ++i)
    {
        auto icon = new Engine::Image("play/full.png", 1294 + i * 30, 68 + multi_ui_btn_shift, 32, 32);
        lifeIcons.push_back(icon);
        UIGroup->AddNewObject(icon);
    }
    UpdateLifeIcons(0); // initialize

    //& turret and tool buttons for p1
    TurretButton *btn;

    // Button 1 (machine gun turret)
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1294, 116 + multi_ui_btn_shift, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-1.png", 1294, 116 - 8 + multi_ui_btn_shift, 0, 0, 0, 0), 1294, 116 + multi_ui_btn_shift, MachineGunTurret::Price);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);
    //& lv.
    UIGroup->AddNewObject(machinegun_level_text = new Engine::Label("lv."+ std::to_string(machinegunturret_lv), "pirulen.ttf", 16, 1294 + 10, 185 + multi_ui_btn_shift));
    
    UIGroup->AddNewObject(new Engine::Label("lvup:", "pirulen.ttf", 16, 1294 + 30, 185 + 25 + multi_ui_btn_shift, 0, 0, 0, 255, 0.5, 0.5));
    UIGroup->AddNewObject(machinegun_upgrade_text = new Engine::Label("$" + std::to_string(upgrade_money[machinegunturret_lv]), "pirulen.ttf", 16, 1294 + 30, 185 + 40 + multi_ui_btn_shift, 0, 0, 0, 255, 0.5, 0.5));
    
    btn->SetOnRightClickCallback(std::bind(&PlayScene::UIBtnRightClicked, this, 0));

    // Button 2 (grow turret)
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1370, 116 + multi_ui_btn_shift, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-6.png", 1370, 116 - 8 + multi_ui_btn_shift, 0, 0, 0, 0), 1370, 116 + multi_ui_btn_shift, GrowTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);
    UIGroup->AddNewObject(grow_level_text = new Engine::Label("lv."+ std::to_string(growturret_lv), "pirulen.ttf", 16, 1370 + 10, 185 + multi_ui_btn_shift));
    
    UIGroup->AddNewObject(new Engine::Label("lvup:", "pirulen.ttf", 16, 1370 + 30, 185 + 25 + multi_ui_btn_shift, 0, 0, 0, 255, 0.5, 0.5));
    UIGroup->AddNewObject(grow_upgrade_text = new Engine::Label("$"+ std::to_string(upgrade_money[growturret_lv]), "pirulen.ttf", 16, 1370 + 30, 185 + 40 + multi_ui_btn_shift, 0, 0, 0, 255, 0.5, 0.5));
    
    btn->SetOnRightClickCallback(std::bind(&PlayScene::UIBtnRightClicked, this, 1));

    // Button 3 (laser turret)
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", 1446, 116 + multi_ui_btn_shift, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-2.png", 1446, 116 - 8 + multi_ui_btn_shift, 0, 0, 0, 0), 1446, 116 + multi_ui_btn_shift, LaserTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);
    UIGroup->AddNewObject(laser_level_text = new Engine::Label("lv."+ std::to_string(laserturret_lv), "pirulen.ttf", 16, 1446 + 10, 185 + multi_ui_btn_shift));
    
    UIGroup->AddNewObject(new Engine::Label("lvup:", "pirulen.ttf", 16, 1446 + 30, 185 + 25 + multi_ui_btn_shift, 0, 0, 0, 255, 0.5, 0.5));
    UIGroup->AddNewObject(laser_upgrade_text = new Engine::Label("$"+ std::to_string(upgrade_money[laserturret_lv]), "pirulen.ttf", 16, 1446 + 30, 185 + 40 + multi_ui_btn_shift, 0, 0, 0, 255, 0.5, 0.5));
    
    btn->SetOnRightClickCallback(std::bind(&PlayScene::UIBtnRightClicked, this, 2));

    // Button 4 (shovel)
    ToolButton *tool_btn;
    tool_btn = new ToolButton("play/floor.png", "play/dirt.png",
                              Engine::Sprite("play/shovel.png", 1299, 250 + 5 + multi_ui_btn_shift, 54, 54, 0, 0),
                              1294, 250 + multi_ui_btn_shift);
    tool_btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 100));
    UIGroup->AddNewControlObject(tool_btn);

    //all p2 uis...
    if(isMultiPlayer){
        //& "P2"
        UIGroup->AddNewObject(new Engine::Label(std::string("player 2"), "pirulen.ttf", 32, 1294, 380));
        
        //& p2 money
        UIGroup->AddNewObject(UIMoney_P2 = new Engine::Label("money: " + std::string("$") + std::to_string(p2money), "pirulen.ttf", 25, 1294, 418));
        for (int i = 0; i < 5; i++){
            auto icon = new Engine::Image("play/full.png", 1294 + i * 30, 450, 32, 32);

            lifeIcons.push_back(icon);
            UIGroup->AddNewObject(icon);
        }

        //TODO: add press ... key & countdown ui.
        //&instructions of enemies p2 can put.
        UIGroup->AddNewObject(new Engine::Image("play/enemy-1.png", 1294, 520, 64, 64));
        UIGroup->AddNewObject(new Engine::Label(std::string("$") + std::to_string(soldier_enemy_price), "pirulen.ttf", 15, 1340, 525 + 35));
        UIGroup->AddNewObject(soldier_key = new Engine::Image("play/keyboard_a.png", 1360, 515, 45, 45));
        UIGroup->AddNewObject(soldier_unable = new Engine::Image("play/empty.png", 1365, 520, 35, 35));

        UIGroup->AddNewObject(new Engine::Image("play/enemy-2.png", 1294, 590, 64, 64));
        UIGroup->AddNewObject(new Engine::Label(std::string("$") + std::to_string(plane_enemy_price), "pirulen.ttf", 15, 1340, 595 + 40));
        UIGroup->AddNewObject(plane_key = new Engine::Image("play/keyboard_s.png", 1360, 590, 45, 45));
        UIGroup->AddNewObject(plane_unable = new Engine::Image("play/empty.png", 1365, 595, 35, 35));

        UIGroup->AddNewObject(new Engine::Image("play/enemy-3.png", 1294, 660, 64, 64));
        UIGroup->AddNewObject(new Engine::Image("play/enemy-3-head.png", 1294, 660, 64, 64));
        UIGroup->AddNewObject(new Engine::Label(std::string("$") + std::to_string(tank_enemy_price), "pirulen.ttf", 15, 1340, 665 + 35));
        UIGroup->AddNewObject(tank_key = new Engine::Image("play/keyboard_d.png", 1360, 655, 45, 45));
        UIGroup->AddNewObject(tank_unable = new Engine::Image("play/empty.png", 1365, 660, 35, 35));

        UIGroup->AddNewObject(new Engine::Image("play/enemy-4.png", 1294, 730, 64, 64));
        UIGroup->AddNewObject(new Engine::Label(std::string("$") + std::to_string(shield_enemy_price), "pirulen.ttf", 15, 1340, 735 + 55));
        UIGroup->AddNewObject(shield_key = new Engine::Image("play/keyboard_f.png", 1360, 745, 45, 45));
        UIGroup->AddNewObject(shield_unable = new Engine::Image("play/empty.png", 1365, 750, 35, 35));

        UpdateLifeIcons(1);
    }
    else{
        int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
        int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
        int shift = 135 + 25;
        dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
        dangerIndicator->Tint.a = 0;
        UIGroup->AddNewObject(dangerIndicator);
    }
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

void PlayScene::UIBtnRightClicked(int id){
    if (id == 0 && money >= upgrade_money[machinegunturret_lv] && machinegunturret_lv < 5){
        EarnMoney(-1 * upgrade_money[machinegunturret_lv]);
        machinegun_level_text->SetText("lv." + std::to_string(++machinegunturret_lv));
        machinegun_upgrade_text->SetText("$"+ std::to_string(upgrade_money[machinegunturret_lv]));

        if(machinegunturret_lv == 5){
            machinegun_level_text->SetText("MAX");
        }

        //update all the turrets' lv.
        for (auto obj : TowerGroup->GetObjects()) {
            Turret* tur = dynamic_cast<Turret*>(obj);
            if(tur->type == 0){
                tur->on_playing_level = machinegunturret_lv;
            }
        }
    }
    else if (id == 1 && money >= upgrade_money[growturret_lv] && growturret_lv < 5){
        EarnMoney(-1 * upgrade_money[growturret_lv]);
        grow_level_text->SetText("lv." + std::to_string(++growturret_lv));
        grow_upgrade_text->SetText("$"+ std::to_string(upgrade_money[growturret_lv]));

        if(growturret_lv == 5){
            grow_level_text->SetText("MAX");
        }

        for (auto obj : TowerGroup->GetObjects()) {
            Turret* tur = dynamic_cast<Turret*>(obj);
            if(tur->type == 1){
                tur->on_playing_level = growturret_lv;
            }
        }
    }
    else if (id == 2 && money >= upgrade_money[laserturret_lv] && laserturret_lv < 5){
        EarnMoney(-1 * upgrade_money[laserturret_lv]);
        laser_level_text->SetText("lv." + std::to_string(++laserturret_lv));
        laser_upgrade_text->SetText("$"+ std::to_string(upgrade_money[laserturret_lv]));

        if(laserturret_lv == 5){
            laser_level_text->SetText("MAX");
        }

        for (auto obj : TowerGroup->GetObjects()) {
            Turret* tur = dynamic_cast<Turret*>(obj);
            if(tur->type == 2){
                tur->on_playing_level = laserturret_lv;
            }
        }
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
