// Created by Griffin Walraven
//   ___________        __
//  / ___/ ___\ \      / /
// | |  _\___ \\ \ /\ / / 
// | |_| |___) |\ V  V /  
//  \____|____/  \_/\_/   
//
// Simple 2d platformer to learn sdl and c
//
// Base Build command:
// clang -lSDL2main -lSDL2_mixer -lSDL2_image -lSDL2 main.c
// With Extra debuggin stuff:
// clang -lSDL2main -lSDL2_mixer -lSDL2_image -lSDL2 -Werror -Wall -Wextra main.c
//
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h> 
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


// Screen Dimensions
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

struct GameStruct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event input;
} Game;

struct PlayerStruct {
	int SPEED;
	int GRAVITY;
	int movementX;
	int movementY;
	int moveRight;
	int moveLeft;
	int moveUp;
	int moveDown;
	int momentumY;
	char lastDirection;

	SDL_Rect Rect;
	SDL_Texture* spriteSheet;
	SDL_Texture* spriteSheetLeft;
	SDL_Rect textureRect;
} Player;

void initSDL(){
	// Initiated everything needed for SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	Game.window = SDL_CreateWindow("SDL Jumper", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED);
	Game.renderer = SDL_CreateRenderer(Game.window, -1, SDL_RENDERER_ACCELERATED);
}

SDL_Texture* createSpriteSheet(char* path) {
	// Create a spriteSheet from a given image path
	SDL_Texture* spriteSheet = NULL;
	SDL_Surface* temp = IMG_Load(path);
	spriteSheet = SDL_CreateTextureFromSurface(Game.renderer, temp);
	SDL_FreeSurface(temp);

	return spriteSheet;
}

SDL_Rect createRect(int width, int height, int x, int y){
	SDL_Rect rect;
	if (width != NULL){
		rect.w = width;
	}
	if (height != NULL){
		rect.h = height;
	}
	rect.x = x;
	rect.y = y;

	return rect;
}

void createPlayerSprite() {
	Player.spriteSheet = createSpriteSheet("sprites/capguy-walk.png");
	Player.spriteSheetLeft = createSpriteSheet("sprites/capguy-walk-left.png");

	Player.Rect = createRect(140, 200, 0, 0);
	Player.Rect.y = SCREEN_HEIGHT - Player.Rect.h;

	Player.textureRect = createRect(0, 0, 0, 0);
	Player.textureRect.x = 0;
	Player.textureRect.y = 0;

	SDL_QueryTexture(Player.spriteSheet, NULL, NULL, &Player.textureRect.w, &Player.textureRect.h);

	Player.textureRect.w /= 8;

	// Set some default values for the Player
	Player.SPEED = 15;
	Player.GRAVITY = 3;
	Player.lastDirection = 'r';
}

void movePlayer() {
		// Move the player based on the input
		Player.momentumY -= Player.GRAVITY;
		Player.movementX = (Player.moveLeft * -Player.SPEED) + (Player.moveRight * Player.SPEED);
		Player.movementY = -Player.momentumY;
		Player.Rect.x += Player.movementX;
		Player.Rect.y += Player.movementY;

		// Keep the player on screen
		if (Player.Rect.x + Player.Rect.w > SCREEN_WIDTH){
			Player.Rect.x = SCREEN_WIDTH - Player.Rect.w;
		} else if (Player.Rect.x < 0){
			Player.Rect.x = 0;
		} else if (Player.Rect.y + Player.Rect.h > SCREEN_HEIGHT){
			Player.Rect.y = SCREEN_HEIGHT - Player.Rect.h;
			Player.momentumY = 0;
		} else if (Player.Rect.y < 0){
			Player.Rect.y = 0;
		}
}

void drawPlayer() {
		int totalFrames = 8;
		int delayPerFrame = 100;

		int frame = (SDL_GetTicks() / delayPerFrame) % totalFrames;

		// Only show movement if the player is moving left or right
		if (Player.movementX != 0){
			Player.textureRect.x = frame * Player.textureRect.w;
		}
		
		SDL_SetRenderDrawColor(Game.renderer, 0, 0, 0, 255);
		SDL_RenderClear(Game.renderer);

		// Show the player moving left if moving left,
		// or right if moving right,
		// else show the player facing its last direction
		if (Player.movementX > 0){
			// Moving right
			SDL_RenderCopy(Game.renderer, Player.spriteSheet, &Player.textureRect, &Player.Rect);
			Player.lastDirection = 'r';
		} else if (Player.movementX < 0){
			// Moving left 
			SDL_RenderCopy(Game.renderer, Player.spriteSheetLeft, &Player.textureRect, &Player.Rect);
			Player.lastDirection = 'l';
		} else{
			switch (Player.lastDirection){
				case ('r'):
					SDL_RenderCopy(Game.renderer, Player.spriteSheet, &Player.textureRect, &Player.Rect);
					break;
				case ('l'):
					SDL_RenderCopy(Game.renderer, Player.spriteSheetLeft, &Player.textureRect, &Player.Rect);
					break;
			}
		}
}

void playerJump() {
	// Move up if on the ground
	if (Player.Rect.y + Player.Rect.h > SCREEN_HEIGHT -10){
		Player.moveUp = 1;
		Player.momentumY += Player.SPEED * 3;
	}
}

int main( int argc, char* args[] )
{
	
	initSDL();

	createPlayerSprite();

	// Get info for animation timings
	int FPSCAP = 120;
	float desiredDelta = 1000 / FPSCAP;
	
	// Game loop
	int quit = 0;
	while (!quit) {
		
		float startLoop = SDL_GetTicks();

		while (SDL_PollEvent(&Game.input) > 0){
			switch (Game.input.type){
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_KEYDOWN):
					switch (Game.input.key.keysym.sym){
						case ('a'):
							// Move left
							Player.moveLeft = 1;
							break;
						case ('d'):
							// Move right
							Player.moveRight = 1;
							break;
						case ('s'):
							// Move down
							Player.moveDown = 1;
							break;
						case ('w'):
							// Move up
							playerJump();
							break;
						case (SDLK_ESCAPE):
							// Quit the game
							quit = 1;
							break;
					}
					break;
				case (SDL_KEYUP):
					switch (Game.input.key.keysym.sym){
						case ('a'):
							// Stop Move left
							Player.moveLeft = 0;
							break;
						case ('d'):
							// Stop Move right
							Player.moveRight = 0;
							break;
						case ('s'):
							// Stop Move down
							Player.moveDown = 0;
							break;
						case ('w'):
							// Stop Move up
							Player.moveUp = 0;
							break;
					}
					break;
			}
		}

		movePlayer();
		drawPlayer();

		SDL_RenderPresent(Game.renderer);

		float delta = SDL_GetTicks() - startLoop;
		if (delta < desiredDelta){
			SDL_Delay(desiredDelta - delta);
		}
	}

	// Destroy the renderer created above
	SDL_DestroyRenderer(Game.renderer);

	// Destroy the window created above
	SDL_DestroyWindow(Game.window);

	// Quit out of SDL
	SDL_Quit();

	return 0;
}

