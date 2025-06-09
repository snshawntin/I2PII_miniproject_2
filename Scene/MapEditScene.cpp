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

Engine::Point MapEditScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}

void MapEditScene::Initialize() {
    DebugMode = 0;

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

void MapEditScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    IScene::Terminate();
}

void MapEditScene::Update(float deltaTime) {
    //& display the selete mode.
    switch (selected_tile){
    case TILE_FLOOR: UISelected->Text = "unwalkable"; break;
    case TILE_DIRT: UISelected->Text = "walkable"; break;
    case NONE: UISelected->Text = "none"; break;
    }
}

void MapEditScene::UpdateMapUI() {
    //& prepare to redraw the map.
    delete TileMapGroup;
    TileMapGroup = new Group();
    
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            if (mapState[i][j] == TILE_FLOOR){
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }
            else if (mapState[i][j] == TILE_DIRT) {
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }
        }
    }
}

void MapEditScene::Draw() const {
    IScene::Draw();
    if (DebugMode) {
        // Draw reverse BFS distance on all reachable blocks.
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                if (mapDistance[i][j] != -1) {
                    // Not elegant nor efficient, but it's quite enough for debugging.
                    Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
                    label.Anchor = Engine::Point(0.5, 0.5);
                    label.Draw();
                }
            }
        }
    }
}

void MapEditScene::OnMouseDown(int button, int mx, int my) {
    if ((button & 1) && !imgTarget->Visible) {
        selected_tile = NONE;
    }

    IScene::OnMouseDown(button, mx, my);
}

void MapEditScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
        imgTarget->Visible = false;
        return;
    }
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}

void MapEditScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);

    if (!imgTarget->Visible){
        return;
    }

    const int x = mx / BlockSize;
    const int y = my / BlockSize;

    if (button & 1) {
        if(selected_tile != NONE){
            mapState[y][x] = selected_tile;
            UpdateMapUI();
        }
        OnMouseMove(mx, my);
    }
}

void MapEditScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);

    //& hotkey of debug mode.
    if (keyCode == ALLEGRO_KEY_TAB) {
        DebugMode = !DebugMode;
    } 
}

void MapEditScene::ReadMap() {
    std::string filename = std::string("../Resource/custom_map/cm0") + std::to_string(CustomMapId) + ".txt";
    // Read map file.
    char c;
    std::vector<bool> mapData;
    std::ifstream fin(filename);

    while (fin >> c) {
        switch (c) {
            case '0': mapData.push_back(false); break;
            case '1': mapData.push_back(true); break;
            case '\n':
            case '\r':
                if (static_cast<int>(mapData.size()) / MapWidth != 0){
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
    if (static_cast<int>(mapData.size()) != MapWidth * MapHeight){
        std::cout << static_cast<int>(mapData.size()) << " != " << MapWidth * MapHeight << std::endl;
        throw std::ios_base::failure("Map data is corrupted.");
    }

    // Store map in 2d array.
    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
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
void MapEditScene::ConstructUI() {
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
 
    //& add save and quit button like other scene do.
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    UIGroup->AddNewObject(new Engine::Label("Editing:", "pirulen.ttf", 40, 1294, 10));
    UIGroup->AddNewObject(new Engine::Label(std::string("Custom Map") + std::to_string(CustomMapId), "pirulen.ttf", 25, 1294, 55));

    //save and quit button
    Engine::ImageButton *back_btn;
    back_btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW * 8 / 5 + 140, halfH * 8 / 5 + 40, 150, 100);
    back_btn->SetOnClickCallback(std::bind(&MapEditScene::BackOnClick, this));
    AddNewControlObject(back_btn);
    AddNewObject(new Engine::Label("save", "pirulen.ttf", 24, halfW * 8 / 5 + 215, halfH * 8 / 5 + 65, 0, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("&", "pirulen.ttf", 24, halfW * 8 / 5 + 215, halfH * 8 / 5 + 90, 0, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("quit", "pirulen.ttf", 24, halfW * 8 / 5 + 215, halfH * 8 / 5 + 115, 0, 0, 0, 255, 0.5, 0.5));

    //& display the tile that is gonna put.
    UIGroup->AddNewObject(new Engine::Label("selected:", "pirulen.ttf", 20, 1294, 100, 0, 0, 255, 255));
    UIGroup->AddNewObject(UISelected = new Engine::Label("none", "pirulen.ttf", 20, 1294, 120, 0, 0, 255, 255));


    int shift = 135 + 25;
    dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
    dangerIndicator->Tint.a = 0;
    UIGroup->AddNewObject(dangerIndicator);
}

//& 2 ui btn is clicked.
void MapEditScene::UIBtnClicked(int id) {
    if (id == 0){
        selected_tile = TILE_DIRT;
    }
    else if (id == 1){
        selected_tile = TILE_FLOOR;
    }    
}

//& while save&quit btn is clicked.
void MapEditScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("custom-map-select");
}

const Engine::Point bfs_dxdy[4] = {
    Engine::Point(1, 0), Engine::Point(-1, 0), Engine::Point(0, 1), Engine::Point(0, -1)
};

std::vector<std::vector<int>> MapEditScene::CalculateBFSDistance() {
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

    // (END)TODO PROJECT-1 (1/1): Implement a BFS starting from the most right-bottom block in the map.
    // (END)              For each step you should assign the corresponding distance to the most right-bottom block.
    // (END)              mapState[y][x] is TILE_DIRT if it is empty.
    while (!que.empty()) {
        Engine::Point observing_point = que.front();
        que.pop();

        //& this point is searched before.
        if(map[observing_point.y][observing_point.x] != -1){
            continue;
        }

        Engine::Point possible_next_step;
        for(unsigned short i = 0; i < 4; i++){
            possible_next_step = observing_point + bfs_dxdy[i];
            //& invalid
            if(
                possible_next_step.x >= MapWidth || possible_next_step.x < 0 || //& out-of-bound
                possible_next_step.y >= MapHeight || possible_next_step.y < 0 || 
                (map[possible_next_step.y][possible_next_step.x] <= map[observing_point.y][observing_point.x] && 
                map[possible_next_step.y][possible_next_step.x] != -1) || //& not a shorter path
                mapState[possible_next_step.y][possible_next_step.x] != TILE_DIRT //& can't walk 
            ){ continue; }

            map[possible_next_step.y][possible_next_step.x] = map[observing_point.y][observing_point.x] + 1;

            //& reached the most left-top block, stop
            if (possible_next_step == Engine::Point(0, 0)){
                continue;
            }
            
            que.push(possible_next_step);
        }
    }

    return map;
}