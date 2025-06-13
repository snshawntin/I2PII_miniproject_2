// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/ModeSelectScene.hpp"
#include "Scene/CustomMapSelectScene.hpp"
#include "Scene/CustomModeSelectScene.hpp"
#include "Scene/MapEditScene.hpp"
#include "Scene/ScoreboardScene.hpp"
#include "Scene/ScoreboardSelect.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/MultiWin.hpp"
#include "Scene/StartScene.h"
#include "Scene/SettingsScene.hpp"

////PROJECT-bonus (3) (Max 10%, subjective) Optimize or add features not mentioned in the file.
//(END) TODO PROJECT-bonus (4): Use version control tools (e.g., Git) to manage your project.

int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

    //(END) TODO HACKATHON-2 (2/3): Register Scenes here
	game.AddNewScene("start", new StartScene());

    game.AddNewScene("mode-select", new ModeSelectScene());
    game.AddNewScene("stage-select", new StageSelectScene());

	game.AddNewScene("settings", new SettingsScene());

	game.AddNewScene("custom-map-select", new CustomMapSelectScene());
	game.AddNewScene("custom-mode-select", new CustomModeSelectScene());
	game.AddNewScene("map-edit", new MapEditScene());

	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("lose", new LoseScene());
	game.AddNewScene("win", new WinScene());
	game.AddNewScene("multi-win", new MultiWinScene());
	game.AddNewScene("scoreboard", new ScoreboardScene());
	game.AddNewScene("scoreboard-select", new ScoreboardSelectScene());

    //(END) TODO HACKATHON-1 (1/1): Change the start scene
	game.Start("start", 60, 1600, 832);
	return 0;
}
