#ifndef COSSACKQUEST_ENEMY_HPP
#define COSSACKQUEST_ENEMY_HPP

#include "GameObject.hpp"
#include "PlayerControl.hpp"

class Enemy : public GameObject
{
public:
	Enemy(Vector3 pos, Texture tex, Model mod, BoundingBox* wa);
	~Enemy();
	Vector3 position{};
	Texture texture;
	Model model;
	void update();
private:
	BoundingBox* wallArray;
	BoundingBox* wallIndex;
	bool checkCollisionPlayer(BoundingBox& box);
	unsigned findPath();
	bool alive;
};


#endif //COSSACKQUEST_ENEMY_HPP
