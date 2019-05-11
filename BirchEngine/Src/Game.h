#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <vector>

class AssetManager;
class ColliderComponent;

class Game
{
public:
	Game();
	~Game();

	void init(const char* title, int width, int height, bool fullscreen);

	void handleEvents();
	void update();
	bool running() { return isRunning; }
	void render();
	void clean();
	

	float ballTimer = 0;
	float barTimer = 0;
	bool missed;
	bool tap = true;
	bool mR = true;
	bool mL = true;
	bool mU = true;
	bool mD = true;
	bool canShoot = true;
	bool scoredOnce = false;

	

	static SDL_Renderer *renderer;
	static SDL_Event event;
	
	static bool isRunning;
	static SDL_Rect camera;
	static AssetManager* assets;
	enum groupLabels : std::size_t
	{
		groupMap,
		groupPlayers,
		groupColliders,
		groupProjectiles
	};

private:
	
	int cnt = 0;
	SDL_Window *window;
	
};