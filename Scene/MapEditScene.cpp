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
#include "MapEditScene.hpp"
#include "Turret/TurretButton.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/Label.hpp"

const std::vector<Engine::Point> MapEditScene::directions = {Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1)};
const int MapEditScene::MapWidth = 20, MapEditScene::MapHeight = 13;
const int MapEditScene::BlockSize = 64;
const Engine::Point MapEditScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point MapEditScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);

Engine::Point MapEditScene::GetClientSize()
{
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}

void MapEditScene::Initialize()
{
    DebugMode = 0;
    operating = 0;

    mapState.clear();
    keyStrokes.clear();

    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    ReadMap();

    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    UIGroup->AddNewObject(imgTarget);
    selected_tile = NONE;

    // Start BGM.
    bgmId = AudioHelper::PlayBGM("happy.ogg");
}

void MapEditScene::Terminate()
{
    AudioHelper::StopBGM(bgmId);
    IScene::Terminate();
}

void MapEditScene::Update(float deltaTime)
{
    //& display the selete mode.
    switch (selected_tile)
    {
    case TILE_FLOOR:
        UISelected->Text = "unwalkable";
        break;
    case TILE_DIRT:
        UISelected->Text = "walkable";
        break;
    case NONE:
        UISelected->Text = "none";
        break;
    }
}

void MapEditScene::UpdateMapUI()
{
    // Redraw the map by adding new images to TileMapGroup.
    // No deletion or clearing of TileMapGroup.
    for (int i = 0; i < MapHeight; i++)
    {
        for (int j = 0; j < MapWidth; j++)
        {
            if (mapState[i][j] == TILE_FLOOR)
            {
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }
            else if (mapState[i][j] == TILE_DIRT)
            {
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }
        }
    }
}

void MapEditScene::Draw() const
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

static bool mousedown = 0;

void MapEditScene::OnMouseDown(int button, int mx, int my)
{
    mousedown = 1;
    // do bound check
    if ((button & 1) && !imgTarget->Visible)
    {
        selected_tile = NONE;
    }

    IScene::OnMouseDown(button, mx, my);
}

void MapEditScene::OnMouseMove(int mx, int my)
{
    IScene::OnMouseMove(mx, my);

    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
    {
        imgTarget->Visible = false;
        return;
    }

    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;

    if (selected_tile != NONE && mousedown == 1 && mapState[y][x] != selected_tile)
    {
        mapState[y][x] = selected_tile;
        UpdateMapUI();
    }
}

void MapEditScene::OnMouseUp(int button, int mx, int my)
{
    mousedown = 0;
    IScene::OnMouseUp(button, mx, my);

    if (!imgTarget->Visible)
    {
        return;
    }

    // do bound check
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
    {
        return;
    }

    if (button & 1)
    {
        if (selected_tile != NONE)
        {
            UpdateMapUI();
            mapState[y][x] = selected_tile;
            UpdateMapUI();
        }
        OnMouseMove(mx, my);
    }
}

void MapEditScene::OnKeyDown(int keyCode)
{
    IScene::OnKeyDown(keyCode);

    //& hotkey of debug mode.
    if (keyCode == ALLEGRO_KEY_TAB)
    {
        DebugMode = !DebugMode;
    }
}

void MapEditScene::ReadMap()
{
    std::string filename = std::string("C:/Users/User/Downloads/final_project/2025_I2P2_TowerDefense-main/2025_I2P2_TowerDefense-main/Resource/custom_map/cm0") + std::to_string(CustomMapId) + ".txt";
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
            {
                throw std::ios_base::failure("Map data is corrupted.");
            }
            break;
        default:
            throw std::ios_base::failure("Map data is corrupted.");
            break;
        }
    }
    fin.close();

    // Validate map data.
    if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
    {
        std::cout << static_cast<int>(mapData.size()) << " != " << MapWidth * MapHeight << std::endl;
        throw std::ios_base::failure("Map data is corrupted.");
    }

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

//& uis.
void MapEditScene::ConstructUI()
{
    // Background
    UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));
    TurretButton *btn;

    // Button 1 (dirt (walkable))
    btn = new TurretButton("play/shovel-base.png", "play/dirt.png",
                           Engine::Sprite("play/floorbutton-base.png", 1294, 165, 0, 0, 0, 0),
                           Engine::Sprite("play/dirt.png", 1306, 165 + 13, 38, 38, 0, 0), 1294, 165, 0);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&MapEditScene::UIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);

    // Button 2 (floor (unwalkable))
    btn = new TurretButton("play/shovel-base.png", "play/dirt.png",
                           Engine::Sprite("play/floorbutton-base.png", 1400, 165, 0, 0, 0, 0),
                           Engine::Sprite("play/floor.png", 1412, 165 + 13, 38, 38, 0, 0), 1400, 165, 0);
    btn->SetOnClickCallback(std::bind(&MapEditScene::UIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    UIGroup->AddNewObject(new Engine::Label("Editing:", "pirulen.ttf", 40, 1294, 10));
    UIGroup->AddNewObject(new Engine::Label(std::string("Custom Map") + std::to_string(CustomMapId), "pirulen.ttf", 25, 1294, 55));

    //& save and quit button
    Engine::ImageButton *back_btn;
    back_btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW * 8 / 5 + 20, halfH * 8 / 5 + 40 - 110, 150, 100);
    back_btn->SetOnClickCallback(std::bind(&MapEditScene::BackOnClick, this, 1));
    AddNewControlObject(back_btn);
    AddNewObject(new Engine::Label("save", "pirulen.ttf", 24, halfW * 8 / 5 + 95, halfH * 8 / 5 + 65 - 110, 0, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("&", "pirulen.ttf", 24, halfW * 8 / 5 + 95, halfH * 8 / 5 + 90 - 110, 0, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("quit", "pirulen.ttf", 24, halfW * 8 / 5 + 95, halfH * 8 / 5 + 115 - 110, 0, 0, 0, 255, 0.5, 0.5));

    back_btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW * 8 / 5 + 20, halfH * 8 / 5 + 40, 150, 100);
    back_btn->SetOnClickCallback(std::bind(&MapEditScene::BackOnClick, this, 0));
    AddNewControlObject(back_btn);
    AddNewObject(new Engine::Label("quit", "pirulen.ttf", 24, halfW * 8 / 5 + 95, halfH * 8 / 5 + 65, 0, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("w/o", "pirulen.ttf", 24, halfW * 8 / 5 + 95, halfH * 8 / 5 + 90, 0, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("saving", "pirulen.ttf", 24, halfW * 8 / 5 + 95, halfH * 8 / 5 + 115, 0, 0, 0, 255, 0.5, 0.5));

    //& display the tile that is gonna put.
    UIGroup->AddNewObject(new Engine::Label("selected:", "pirulen.ttf", 20, 1294, 100, 0, 0, 255, 255));
    UIGroup->AddNewObject(UISelected = new Engine::Label("none", "pirulen.ttf", 20, 1294, 120, 0, 0, 255, 255));

    //& show the error msg (empty)
    AddNewObject(ErrorMsg = new Engine::Label("", "pirulen.ttf", 30, halfW * 8 / 5 + 20, halfH * 8 / 5 - 25 - 110, 255, 0, 0, 255));
    AddNewObject(ErrorMsgExplicit = new Engine::Label("", "pirulen.ttf", 15, halfW * 8 / 5 + 20, halfH * 8 / 5 + 10 - 110, 255, 0, 0, 255));

    int shift = 135 + 25;
    dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
    dangerIndicator->Tint.a = 0;
    UIGroup->AddNewObject(dangerIndicator);
}

//& 2 ui btn is clicked.
void MapEditScene::UIBtnClicked(int id)
{
    if (id == 0)
    {
        selected_tile = TILE_DIRT;
    }
    else if (id == 1)
    {
        selected_tile = TILE_FLOOR;
    }
}

//& while quit btn is clicked.
void MapEditScene::BackOnClick(bool save)
{
    if (save)
    {
        std::vector<std::vector<int>> dist = CalculateBFSDistance();

        if (dist[0][0] == -1)
        {
            int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
            int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
            int halfW = w / 2;
            int halfH = h / 2;

            ErrorMsg->Text = "error:";
            ErrorMsgExplicit->Text = "no valid path";
            return;
        }

        unsigned have_place_for_turrets = 0;
        for (int i = 0; i < MapHeight; i++)
        {
            for (int j = 0; j < MapWidth; j++)
            {
                if (mapState[i][j] == TILE_FLOOR)
                {
                    have_place_for_turrets = 1;
                    break;
                }
            }
            if (have_place_for_turrets)
            {
                break;
            }
        }

        if (!have_place_for_turrets)
        {
            ErrorMsg->Text = "error:";
            ErrorMsgExplicit->Text = "no place for turrets";
            return;
        }

        std::string filename = std::string("C:/Users/User/Downloads/final_project/2025_I2P2_TowerDefense-main/2025_I2P2_TowerDefense-main/Resource/custom_map/cm0") + std::to_string(CustomMapId) + ".txt";
        std::ofstream fout(filename);

        for (int i = 0; i < MapHeight; i++)
        {
            for (int j = 0; j < MapWidth; j++)
            {
                if (mapState[i][j] == TILE_FLOOR)
                {
                    fout << "1";
                }
                else if (mapState[i][j] == TILE_DIRT)
                {
                    fout << "0";
                }
            }
            fout << "\n";
        }

        fout.close();
    }

    Engine::GameEngine::GetInstance().ChangeScene("custom-map-select");
}

const Engine::Point bfs_dxdy[4] = {
    Engine::Point(1, 0), Engine::Point(-1, 0), Engine::Point(0, 1), Engine::Point(0, -1)};

std::vector<std::vector<int>> MapEditScene::CalculateBFSDistance()
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

    while (!que.empty())
    {
        Engine::Point observing_point = que.front();
        que.pop();
        for (int i = 0; i < 4; i++)
        {
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