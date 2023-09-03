#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <random>
#include "GameObject.h"
#include <SDL_mixer.h>
#include <sstream>

bool alive = true;
bool flag = true;
int score = 0;



// Sets up a new random Y position to render enemy
int ChangeYPosition() {
	std::random_device rd;     // Only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(32, 448); // Guaranteed unbiased
	auto random_integer = uni(rng);
	return random_integer;
}


// Moves enemy object from the left to right wall and vice versa
void enemyMovement(SDL_Rect& rect, int speed, Mix_Chunk* hitSound)
{
	if (flag)
	{
		rect.x -= speed;
		if (rect.x <= 0) {
			Mix_PlayChannel(-1, hitSound, 0);
			score++;
			flag = false;
			rect.x = 0;
		}
	}

	if (flag == false)
	{
		rect.x += speed;
		if (rect.x >= 608) {
			Mix_PlayChannel(-1, hitSound, 0);
			flag = true;
			rect.x = 608;
			rect.y = ChangeYPosition();
		}
	}
}

// Some memory management and termination
void CleanUp(SDL_Renderer* ren, SDL_Window* win)
{
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	Mix_Quit();
	TTF_Quit();
	SDL_Quit();
}

// Generate text texture to display on the screen 
// TODO: Should also provide the positions to draw the text, 
// create the rect and draw it on the screen
void CreateText(const char* text, SDL_Renderer* ren, int xPos, int yPos, int fontSize)
{
	TTF_Font* font = TTF_OpenFont("assets/sstaff.otf", fontSize);
	SDL_Surface* textSurf = TTF_RenderText_Solid(font, text, SDL_Color{ 255, 255, 255, 255 });
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, textSurf);
	SDL_FreeSurface(textSurf);
	SDL_Rect textRect;
	textRect.x = xPos;
	textRect.y = yPos;
	SDL_QueryTexture(tex, NULL, NULL, &textRect.w, &textRect.h);
	SDL_RenderCopy(ren, tex, NULL, &textRect);
}

// Handles setting up and drawing the score value
void UpdateScore(SDL_Renderer* ren)
{
	std::stringstream strm;
	strm << score;
	SDL_Surface* scoreValue;
	TTF_Font* font = TTF_OpenFont("assets/sstaff.otf", 20);
	scoreValue = TTF_RenderText_Solid(font, strm.str().c_str(), SDL_Color{ 255, 255, 255, 255 });
	SDL_Texture* scoreValueTex = SDL_CreateTextureFromSurface(ren, scoreValue);
	SDL_Rect scoreValueRect;
	scoreValueRect.x = 416;
	scoreValueRect.y = 16;
	scoreValueRect.w = scoreValue->w;
	scoreValueRect.h = scoreValue->h;
	SDL_FreeSurface(scoreValue);
	SDL_RenderCopy(ren, scoreValueTex, NULL, &scoreValueRect);

}

// AABB - Axis Aligned Bounding Box Collision
bool Collides(SDL_Rect& A, SDL_Rect& B)
{	
	return ( A.x + A.w >= B.x &&
			 B.x + B.w >= A.x &&
			 A.y + A.h >= B.y &&
			 B.y + B.h >= A.y );
}

// Entry point
int main(int argc, char* argv[])
{
	unsigned int a = SDL_GetTicks();
	unsigned int b = SDL_GetTicks();
	double delta = 0;

	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	SDL_Event event;
	
	SDL_Window* window = SDL_CreateWindow("DodgeCube", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (window == NULL) 
		std::cout << "Error initializing window [ " << SDL_GetError() << " ]" << std::endl;

	SDL_Renderer* ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL) 
		std::cout << "Error initializing renderer [ " << SDL_GetError() << " ]" << std::endl;
	
	SDL_Rect playerRect = { 0, 128, 32, 32 };
	GameObject* playerObj = new GameObject(playerRect, 0, 0, 3);
	SDL_Rect enemy = { 608, 400, 32, 32 };

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048*2) < 0)
		std::cout << "Error initializing audio [ " << Mix_GetError() << " ]" << std::endl;

	// General rule for audio: 
	// if something's longer than 10s you make it Mix_Music*
	// otherwise Mix_Chunk*
	//Mix_Chunk* moveSound = Mix_LoadWAV("move.wav");
	Mix_Chunk* wallHitSound = Mix_LoadWAV("assets/hit.wav");
	Mix_Chunk* gameOverSound = Mix_LoadWAV("assets/gameover.wav");
	Mix_Chunk* winSound = Mix_LoadWAV("assets/win.wav");

	while (alive)
	{
		//std::cout << "Score: " << score << std::endl; 
		a = SDL_GetTicks();
		delta = a - b;

		if (delta > 1000 / 60.0)
		{
			//std::cout << "FPS: " << (int)(1000 / delta) << std::endl;
			b = a;
			while (SDL_PollEvent(&event)) {

				if (event.type == SDL_QUIT)
					alive = false;


				if (event.type == SDL_KEYDOWN) {
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							alive = false;
							break;

						case SDLK_UP:
							up = true;
							break;

						case SDLK_DOWN:
							down = true;
							break;

						default:
							break;
					}
				}

				if (event.type == SDL_KEYUP) {
					switch (event.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						alive = false;
						break;

					case SDLK_UP:
						up = false;
						break;

					case SDLK_DOWN:
						down = false;
						break;

					default:
						break;
					}
				}
			}

			// Set background color
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
			SDL_RenderClear(ren);

			// Render player
			if (up) {
				//Mix_PlayChannel(-1, moveSound, 0);
				playerObj->Update(ren, playerObj->posX, -5);
			}
			if (down) {
				//Mix_PlayChannel(-1, moveSound, 0);
				playerObj->Update(ren, playerObj->posX, 5);
			}
			playerObj->Draw(ren);
			
			// Render enemy
			SDL_Rect destEnemy{enemy.x, enemy.y, 32, 32};
			SDL_SetRenderDrawColor(ren, 0xff, 0xff, 0xcc, 0xff);
			SDL_RenderFillRect(ren, &destEnemy);
			enemyMovement(enemy, 25, wallHitSound);

			// Render score
			CreateText("Score : ", ren, 320, 16, 20);
			UpdateScore(ren);

			if (Collides(playerObj->rect, enemy))
			{
				score = 0;
				Mix_PlayChannel(-1, gameOverSound, 0);
				SDL_Delay(2);
			}

			if (score == 100)
			{
				Mix_PlayChannel(-1, winSound, 0);
				std::cout << "Congrats! You beat the game!" << std::endl;
				SDL_Delay(4);
				return 0;
			}
			

			SDL_RenderPresent(ren);

		}
	}


	CleanUp(ren, window);
	
	return 0;
}