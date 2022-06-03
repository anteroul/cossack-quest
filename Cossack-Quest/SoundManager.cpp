#include "SoundManager.h"

SoundManager::SoundManager()
{
	sfx = LoadSound("assets/sounds/swing.mp3");
}

SoundManager::~SoundManager()
{
	UnloadSound(sfx);
}

Sound sound(unsigned id)
{
	return SoundManager::sfx;
}