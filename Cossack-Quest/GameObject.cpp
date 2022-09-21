#include "GameObject.hpp"

GameObject::GameObject(Vector3 pos, Texture tex, Model mod) : position(pos), texture(tex), model(mod)
{}

GameObject::GameObject(Texture tex, Model mod) : texture(tex), model(mod)
{}