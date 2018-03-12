#pragma once
#include <string>

enum PlayerType
{
	fire = 0,
	water,
	grass
};

class Player
{
public:
	Player();
	void StartTurn();
	int Attack(Player* other);
	void Block();
	virtual void Special() = 0;
	float TypeMultiplier(Player* other);
	bool IsAlive() const;
	void SetName(std::string& name);
	std::string GetName() const;
	int GetHP() const;
	int GetAtk() const;
	int GetDef() const;
	std::string GetType() const;
	void Die();
protected:
	std::string name;
	int hp;
	int attack;
	int defense;
	PlayerType type;
	bool isBlocking;
};