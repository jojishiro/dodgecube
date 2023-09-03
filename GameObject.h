#pragma once
#include <SDL.h>

class GameObject
{
public:
	SDL_Rect rect;
	int posX, posY, speed;
	GameObject(SDL_Rect rect, int posX, int posY, int speed);
	void Draw(SDL_Renderer* ren);
	void Update(SDL_Renderer* ren, int x, int y);
};