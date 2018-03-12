#pragma once
#include "Player.h"

class Water : public Player
{
public:
	Water();
	virtual void Special();
private:
	int maxHp;
};