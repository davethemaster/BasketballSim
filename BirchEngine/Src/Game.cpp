#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "ECS/Components.h"
#include "Vector2D.h"
#include "Collision.h"
#include "AssetManager.h"
#include <math.h>
#include <sstream>
#include <iostream>
#include <windows.h>
#include <time.h> 
//12:20 #8
Map* map;
Manager manager;


int distanceX;
int distanceY;
int distance;

float pChance;
float Gravity = 0;
Vector2D s0;
Vector2D s1;
int score;

float mmc;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;

SDL_Rect Game::camera = { 0,0,800,640 };

AssetManager* Game::assets = new AssetManager(&manager);

bool Game::isRunning = false;


auto& player(manager.addEntity());
auto& label(manager.addEntity());
auto& background(manager.addEntity());
auto& ball(manager.addEntity());
auto& ball2(manager.addEntity());
auto& hoop(manager.addEntity());
auto& hoop2(manager.addEntity());
auto& bar(manager.addEntity());
auto& ic(manager.addEntity());
auto& emark(manager.addEntity());
auto& ball3(manager.addEntity());
auto& backG(manager.addEntity());

int solve_ballistic_arc(Vector2D proj_pos, float proj_speed, Vector2D target, float gravity)
{
	//Debug.Assert(proj_pos != target && proj_speed > 0 && gravity > 0, "fts.solve_ballistic_arc called with invalid data");

	s0.Zero();
	s1.Zero();


 	Vector2D diff = target - proj_pos;

	float speed2 = proj_speed * proj_speed;
	float speed4 = proj_speed * proj_speed * proj_speed * proj_speed;
	float y = diff.y;
	float x = diff.x;
	float gx = gravity * x;
	float root = speed4 - gravity * (gravity*x*x + 2 * y*speed2);

	if (root < 0)
		return 0;

	root = sqrt(root);

	float lowAng = atan2(speed2 - root, gx);
	float highAng = atan2(speed2 + root, gx);
	int numSolutions = lowAng != highAng ? 2 : 1;
	 
	Vector2D up;
	up.x = 0;
	up.y = -1;
	Vector2D right;
	right.x = 1;
	right.y = 0;
	s0 = right * cos(lowAng)*proj_speed + up * sin(lowAng)*proj_speed;
	if (numSolutions > 1)
		s1 = right * cos(highAng)*proj_speed + up * sin(highAng)*proj_speed;

	return numSolutions;
}



Game::Game()
{}

Game::~Game()
{}

void Game::init(const char* title, int width, int height, bool fullscreen)
{
	int flags = 0;
	
	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		}

		isRunning = true;
	}

	if (TTF_Init() == -1)
	{
		//std::cout << "Error : SDL_TTF" << std::endl;
	}

	

	//assets->AddTexture("terrain", "assets/basketBallCourt.png");
	assets->AddTexture("player", "assets/player_animations.png");
	assets->AddTexture("projectile", "assets/proj.png");
	assets->AddTexture("ground", "assets/basketBallCourt.png");
	assets->AddTexture("ball", "assets/basketball.png");
	assets->AddTexture("ball2", "assets/bS.png");
	assets->AddTexture("hoop", "assets/hoops.png");
	assets->AddTexture("bar", "assets/strengthbar.png");
	assets->AddTexture("ic", "assets/ic.png");
	assets->AddTexture("emark", "assets/eMark.png");
	assets->AddFont("arial", "assets/arial.ttf", 16);
	assets->AddTexture("ball3", "assets/basketball.png");
	assets->AddTexture("backG", "assets/basketballBack.png");


	map = new Map("terrain", 3, 32);

	map->LoadMap("assets/map.map", 25, 20);
	
	player.addComponent <TransformComponent>(2);
	player.addComponent<SpriteComponent>("player", true);
	player.addComponent<KeyboardController>();
	player.addComponent<ColliderComponent>("player");
	player.addGroup(groupPlayers);


	SDL_Color black = { 0, 0, 0, 0 };
	label.addComponent<UILabel>(10, 10, "David", "arial", black);
	
	background.addComponent<TransformComponent>(147, 400, 470, 1050, 1);
	background.addComponent<SpriteComponent>("ground", false);
	
	ball.addComponent<TransformComponent>(600, 600, 445, 170, 0.08);
	ball.addComponent<SpriteComponent>("ball", true);

	//add gravity and physics
	//shoot 
	//percent chance landing the shot
	ball2.addComponent<TransformComponent>(600, 600, 445, 170, 0.08);
	ball2.addComponent<SpriteComponent>("ball2", false);

	ball3.addComponent<TransformComponent>(900, 500, 445, 170, 0.08);
	ball3.addComponent<SpriteComponent>("ball3", false);

	hoop.addComponent<TransformComponent>(940, 500, 32, 32, 7);
	hoop.addComponent<SpriteComponent>("hoop", false);

	backG.addComponent<TransformComponent>(-20, 100, 1000, 1600, 1.2);
	backG.addComponent<SpriteComponent>("backG", false);

	hoop2.addComponent<TransformComponent>(178, 493, 32, 32, 7);
	hoop2.addComponent<SpriteComponent>("hoop", false);
	hoop2.getComponent<SpriteComponent>().spriteFlip = SDL_FLIP_HORIZONTAL;

	bar.addComponent<TransformComponent>(500, 900, 7, 32, 4.5);
	bar.addComponent<SpriteComponent>("bar", false);

	ic.addComponent<TransformComponent>(500, 900, 32, 32, 0.6);
	ic.addComponent<SpriteComponent>("ic", false);

	emark.addComponent<TransformComponent>(500, 500, 256, 256, 0.1);
	emark.addComponent<SpriteComponent>("emark", false);
	/*assets->CreateProjectile(Vector2D(600, 600),Vector2D(2,0),200, 2, "projectile");
	assets->CreateProjectile(Vector2D(600, 620), Vector2D(2, 0), 200, 2, "projectile");
	assets->CreateProjectile(Vector2D(400, 600), Vector2D(2, 1), 200, 2, "projectile");
	assets->CreateProjectile(Vector2D(600, 600), Vector2D(2, -1), 200, 2, "projectile");
	*/

}


auto& tiles(manager.getGroup(Game::groupMap));
auto& players(manager.getGroup(Game::groupPlayers));
auto& colliders(manager.getGroup(Game::groupColliders));
auto& projectiles(manager.getGroup(Game::groupProjectiles));
void Game::handleEvents()
{
	

	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT :
		isRunning = false;
		break;
	default:
		break;
	}
}



void Game::update()
{

	SDL_Rect playerCol = player.getComponent<ColliderComponent>().collider;
	Vector2D playerPos = player.getComponent<TransformComponent>().position;
	ballTimer = ballTimer - 0.1;
	barTimer = barTimer - 0.1;
	std::stringstream ss;
	ss << "Score: " << score;
	label.getComponent<UILabel>().SetLabelText(ss.str(), "arial");
	
	manager.refresh();
	manager.update();
	//588 + velocity.y(-4.59)
	//velocity.y += gravity(0.11)
	// got 610.25
	// 610.25 - 564 = 46.75
	//http://cpp.sh/9it3d
	ball2.getComponent<TransformComponent>().velocity.y += Gravity;

	for (auto& c : colliders)
	{
		SDL_Rect cCol = c->getComponent<ColliderComponent>().collider;
		if (Collision::AABB(cCol, playerCol))
		{
			player.getComponent<TransformComponent>().position = playerPos;
		}
	}

	for (auto& p : projectiles)
	{
		if (Collision::AABB(player.getComponent<ColliderComponent>().collider, p->getComponent<ColliderComponent>().collider))
		{
			std::cout << "Hit Player" << std::endl;
			p->destroy();
		}
	}

		ball3.getComponent<TransformComponent>().position.x = hoop.getComponent<TransformComponent>().position.x + 126;
		ball3.getComponent<TransformComponent>().position.y = hoop.getComponent<TransformComponent>().position.y + 64;

	if (player.getComponent<KeyboardController>().right == true)
	{
		ball.getComponent<TransformComponent>().position.x = playerPos.x + 40;

		ball.getComponent<TransformComponent>().position.y = playerPos.y + 33;

	}
	else if (player.getComponent<KeyboardController>().left == true)
	{
		ball.getComponent<TransformComponent>().position.x = playerPos.x + 13;
		ball.getComponent<TransformComponent>().position.y = playerPos.y + 33;
	}
	else if (player.getComponent<KeyboardController>().down == true) {
		ball.getComponent<TransformComponent>().position.x = playerPos.x + 25;
		ball.getComponent<TransformComponent>().position.y = playerPos.y + 31;
	}
	else if (player.getComponent<KeyboardController>().up == true) {
		ball.getComponent<TransformComponent>().position.x = playerPos.x + 22;
		ball.getComponent<TransformComponent>().position.y = playerPos.y + 20;
	}
	else if (player.getComponent<KeyboardController>().shoot == true)
	{
		ball.getComponent<TransformComponent>().position.x = playerPos.x + 100000;

		ball2.getComponent<TransformComponent>().position.x = playerPos.x + 27;
		ball2.getComponent<TransformComponent>().position.y = playerPos.y + 13;
		//true

		canShoot = true;

	}
    
	else if (player.getComponent<KeyboardController>().shoot == false && canShoot == true)
	{
		//false
		canShoot = false;
		ballTimer = 5;
		barTimer = 3;
		ball2.getComponent<TransformComponent>().position.x = playerPos.x + 27;
		ball2.getComponent<TransformComponent>().position.y = playerPos.y + 13;
		Vector2D offset;          
		offset.x = hoop.getComponent<TransformComponent>().position.x + 126;
		offset.y = hoop.getComponent<TransformComponent>().position.y + 64;

		float speed = 2;
		if (distance > 80 && distance < 136)
		{
			speed = 3;
		}
		else if (distance > 136 && distance < 184)
		{
			speed = 4;
		}
		else  if (distance > 184 && distance < 280)
		{
			speed = 5;
		}
		else if (distance > 280 && distance < 379)
		{
			speed = 6;
		}
		else if (distance > 379 && distance < 496)
		{
			speed = 7;
		}
		else if (distance > 496 && distance < 640)
		{
			speed = 8;
		}
		else if(distance > 640)
		{
			speed = 10;
		}

 		solve_ballistic_arc(ball2.getComponent<TransformComponent>().position, speed, offset, 0.11);
		ball2.getComponent<TransformComponent>().velocity.x = s1.x;
		ball2.getComponent<TransformComponent>().velocity.y = s1.y;
		//ball2.getComponent<TransformComponent>().velocity.x = 0.0095 * distanceX + 0.5;//2.5;
		//ball2.getComponent<TransformComponent>().velocity.y = -0.012 * distanceY - 1.3;//-2.8;
		std::cout << ball2.getComponent<TransformComponent>().velocity.x << '\n' << ball2.getComponent<TransformComponent>().velocity.y << '\n';
   		Gravity = 0.11;
		float p = (rand() % 100) / 100.0f;
		//std::cout << p << "/" << pChance << "\n";
		if (p > pChance)
		{
			missed = true;
			//std::cout << "missed" << "\n";


			float x, y;
			x = rand() % 200 / 100.0f + 0.5;
			y = rand() % 200 / 100.0f + 0.5;
			if (x > 0.7 && x < 1.1)
				x = 0.5;
			if (y > 0.9 && y < 1.1)
				y = 0.5;
			ball2.getComponent<TransformComponent>().velocity.x *= x;
			ball2.getComponent<TransformComponent>().velocity.y *= y;

		}
		else {
			missed = false;
		}


		if (missed == true) {
			emark.getComponent<TransformComponent>().position.x = player.getComponent<TransformComponent>().position.x + 19;
			emark.getComponent<TransformComponent>().position.y = player.getComponent<TransformComponent>().position.y - 17;

			
			
		
		}


		ball.getComponent<TransformComponent>().position.x = playerPos.x + 100000;
		ball.getComponent<TransformComponent>().position.y = playerPos.y + 1000;

	}

	else if (ballTimer <= 0) {
		ball.getComponent<TransformComponent>().position.x = playerPos.x + 22;
		ball.getComponent<TransformComponent>().position.y = playerPos.y + 31;
		scoredOnce = false;
	}

	ball.getComponent<SpriteComponent>().Play("Dribble");


	if (player.getComponent<KeyboardController>().shoot == true)
	{
		if (tap == true) {
			ic.getComponent<TransformComponent>().position.x = playerPos.x - 42;
			ic.getComponent<TransformComponent>().position.y = playerPos.y + 136;
			tap = false;
		}
		pChance = abs(playerPos.x + 10.5 - ic.getComponent<TransformComponent>().position.x) / 56.5;
		pChance = abs(pChance - 1);
		if (pChance < 0.1)
		{
			pChance = 0.1;
		}
		else if (pChance > 0.99)
		{
			pChance = 0.99;
		}
		distanceX = hoop.getComponent<TransformComponent>().position.x + 126 - player.getComponent<TransformComponent>().position.x;
		distanceY = abs(hoop.getComponent<TransformComponent>().position.y + 64 - player.getComponent<TransformComponent>().position.y);
		distance = distanceX + distanceY;

		if (distanceX < 0)
		{

			distanceX = 0;
		}
		int distance = distanceX + distanceY;
		if (playerPos.y < hoop.getComponent<TransformComponent>().position.y + 64)
		{ 
			distanceY = 0;
		}

		bar.getComponent<TransformComponent>().position.x = playerPos.x - 48;
		bar.getComponent<TransformComponent>().position.y = playerPos.y + 130;
		Gravity = 0;



		if (mR == true)
		{
			ic.getComponent<TransformComponent>().velocity.x = -1 + pChance * -(distance / 60);
			if (ic.getComponent<TransformComponent>().position.x <= playerPos.x - 42)
			{
				mR = false;
				mL = true;
			}
		}
		else if (mL == true)
		{
			ic.getComponent<TransformComponent>().velocity.x = 1 + pChance * (distance / 60);
			if (ic.getComponent<TransformComponent>().position.x >= playerPos.x + 71)
			{
				mR = true;
				mL = false;
			}
		}

		// playerPos + 14.5 = middle   56.5
		// 10% chance furthest point
		//99% chance direct middle
		// |middle - currentPos| / 56.5 -> min 10 max 99

		//std::cout << pChance << std::endl;

	}


	if (player.getComponent<KeyboardController>().shoot == false)
	{
		ic.getComponent<TransformComponent>().velocity.x = 0;
		tap = true;
		mR = true;
		mL = true;

		if (barTimer <= 0)
		{
			bar.getComponent<TransformComponent>().position.x = playerPos.x - 1500;
			bar.getComponent<TransformComponent>().position.y = playerPos.y + 1500;
			ic.getComponent<TransformComponent>().position.x = playerPos.x - 1500;
			ic.getComponent<TransformComponent>().position.y = playerPos.y + 1506;

		}
	}
	//get the indicator to move right and left smoothly.
	//when shoot is equal to true i can't move or animate.



	camera.x = static_cast<int>(player.getComponent<TransformComponent>().position.x - 320);
	camera.y = static_cast<int>(player.getComponent<TransformComponent>().position.y - 250);
	


	if (camera.x < 0)
		camera.x = 0;
	if (camera.y < 0)
		camera.y = 0;
	if (camera.x > camera.w)
		camera.x = camera.w;
	if (camera.y > camera.h)
		camera.y = camera.h;

	if (ballTimer <= 0) {
		emark.getComponent<TransformComponent>().position.x = 10000;
		emark.getComponent<TransformComponent>().position.y = 342342;
	}

	else {
		if (missed == true)
		{
			emark.getComponent<TransformComponent>().position.x = player.getComponent<TransformComponent>().position.x + 19;
			emark.getComponent<TransformComponent>().position.y = player.getComponent<TransformComponent>().position.y - 17;

			float madeItx = abs(hoop.getComponent<TransformComponent>().position.x + 126 - ball2.getComponent<TransformComponent>().position.x);
			float madeIty = abs(hoop.getComponent<TransformComponent>().position.y + 64 - ball2.getComponent<TransformComponent>().position.y);
			float madeIt = madeItx + madeIty;


			if (madeIt <= 20 && scoredOnce == false)
			{
				scoredOnce = true;
				std::cout << "\n" << "made it!!";
				score += 2;
			}
		}
		else {

			float madeItx = abs(hoop.getComponent<TransformComponent>().position.x + 126 - ball2.getComponent<TransformComponent>().position.x);
			float madeIty = abs(hoop.getComponent<TransformComponent>().position.y + 64 - ball2.getComponent<TransformComponent>().position.y);
			float madeIt = madeItx + madeIty;
			//std::cout << "\n" << madeItx;
		
			if (madeIt <= 20 && scoredOnce == false)
			{
				scoredOnce = true;
				std::cout << "\n" << "made it!!";
				score += 2;
	
			}
			if (madeIt <= 20 && scoredOnce == false)
			{
				scoredOnce = true;
				std::cout << "\n" << "made it!!";
				score += 2;
			}
			
	}
		ball.getComponent<TransformComponent>().position.x = playerPos.x + 100000;
		ball.getComponent<TransformComponent>().position.y = playerPos.y + 1000;



	}


}


void Game::render()
{


	SDL_RenderClear(renderer);
	for (auto& t : tiles)
	{
		t->draw();
	}
	
	backG.draw();

	background.draw();
	
	
	


	if (player.getComponent<KeyboardController>().up == true) {
		ball.draw();
	}
	hoop.draw();
	hoop2.draw();
	


	for (auto& p : players)
	{
			p->draw();
	}

	emark.draw();

	ball2.draw();
	ball3.draw();
	if (player.getComponent<KeyboardController>().up == false) {
			ball.draw();
			
	}

	bar.draw();
	ic.draw();
	
	

	/*for (auto& p : projectiles)
	{
		p->draw();
	}
	*/
	label.draw();
	
	
		
	

	SDL_RenderPresent(renderer);
}

void Game::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}



