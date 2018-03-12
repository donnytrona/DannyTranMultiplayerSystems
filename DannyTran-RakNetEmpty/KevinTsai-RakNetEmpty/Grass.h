#pragma once
#include "Player.h"

class Grass : public Player
{
public:
	Grass();
	virtual void Special();
	void SetNewType(PlayerType type);
private:
	PlayerType newType;
};
