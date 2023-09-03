#include "GameObject.h"

GameObject::GameObject(SDL_Rect rect, int posX, int posY, int speed)
{
	this->rect = rect;
	this->posX = posX;
	this->posY = posY;
	this->speed = speed;
}

void GameObject::Draw(SDL_Renderer* ren)
{
	SDL_Rect dest{ this->rect.x, this->rect.y, 32, 32 };
	SDL_SetRenderDrawColor(ren, 0xff, 0x00, 0x00, 0xff);
	SDL_RenderFillRect(ren, &dest);
}

void GameObject::Update(SDL_Renderer* ren, int x, int y)
{
	this->rect.x = this->rect.x + (x);
	this->rect.y = this->rect.y + (y);	
}