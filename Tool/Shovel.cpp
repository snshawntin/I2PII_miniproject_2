#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Tool.hpp"
#include "Shovel.hpp"
#include "Scene/PlayScene.hpp"

//TODO PROJECT-3 (3/3): Add new tool: can be use in the map (not turret)
//TODO                  e.g., shovel to remove turret, landmine to immediately damage enemies...

Shovel::Shovel(float x, float y)
    : Tool("play/shovel.png", x, y) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}