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
    PlayScene *getPlayScene();

public:
    bool Enabled = true;
    bool Preview = false;
    Tool(std::string imgTool, float x, float y);
    void Update(float deltaTime) override;
    void Draw() const override;
};

#endif   // TOOL_HPP