#ifndef TOOL_HPP
#define TOOL_HPP
#include <allegro5/base.h>
#include <list>
#include <string>

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;

class Tool : public Engine::Sprite {
protected:
    std::list<Tool *>::iterator lockedTurretIterator;
    PlayScene *getPlayScene();
    // Reference: Design Patterns - Factory Method.

public:
    bool Preview = false;
    Enemy *Target = nullptr;
    Tool(std::string imgTool, float x, float y);
    void Update(float deltaTime) override;
    void Draw() const override;
    int GetPrice() const;
};
#endif   // TOOL_HPP
