#ifndef COSSACKQUEST_GAMEOBJECT_HPP
#define COSSACKQUEST_GAMEOBJECT_HPP

#include <raylib.h>

class GameObject
{
public:
	GameObject(Vector3 pos, Texture tex, Model mod);
	GameObject(Texture tex, Model mod);
	Vector3 position;
	Texture texture;
	Model model;
};

#endif //COSSACKQUEST_GAMEOBJECT_HPP
