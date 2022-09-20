#ifndef COSSACKQUEST_GAMEOBJECT_HPP
#define COSSACKQUEST_GAMEOBJECT_HPP

#include "raylib.h"

typedef struct GameObject {
	Vector3 position;
	Texture texture;
	Model model;
} GameObject;

#endif //COSSACKQUEST_GAMEOBJECT_HPP
