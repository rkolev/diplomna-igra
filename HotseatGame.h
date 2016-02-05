#pragma once
#include "Game.h"
class HotseatGame : public Game
{
public:
	HotseatGame(s8* levelId) : Game(levelId) { }
	~HotseatGame() { }

	void Run() override;
};

