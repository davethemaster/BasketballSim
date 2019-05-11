#pragma once

#include "../Game.h"
#include "ECS.h"
#include "Components.h"


class KeyboardController : public Component
{
public:
	TransformComponent * transform;
	SpriteComponent * sprite;

	bool right;
	bool left;
	bool up;
	bool down;
	bool shoot;
	bool dribble;
	bool shootAnim;

	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
		sprite = &entity->getComponent<SpriteComponent>();
	}
	

	void update() override
	{
		if (Game::event.type == SDL_KEYDOWN && shoot == false)
		{
			switch (Game::event.key.keysym.sym)
			{
			case SDLK_UP:
				up = true;
				down = false;
				right = false;
				left = false;
				transform->velocity.y = -1;
				sprite->Play("Up");
				break;
			case SDLK_LEFT:
				left = true;
				up = false;
				right = false;
				down = false;
				transform->velocity.x = -1;
				sprite->Play("Walk");
				sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
				
				break;

			case SDLK_RIGHT:
				right = true;
				up = false;
				left = false;
				down = false;
				dribble = true;
				transform->velocity.x = 1;
				sprite->Play("Walk");
				
				break;
				
			case SDLK_DOWN:
				
				down = true;
				up = false;
				left = false;
				right = false;
				transform->velocity.y = 1;
				sprite->Play("Down");
				break;
				

			case SDLK_SPACE:
				
				shoot = true;
				down = false;
				up = false;
				right = false;
				left = false;
			
				break;
				
			
			}
		}

		if (Game::event.type == SDL_KEYUP)
		{
			switch (Game::event.key.keysym.sym)
			{
			case SDLK_UP:
				up = false;
				if(down == false)
				transform->velocity.y = 0;
				if(down == false && right == false && left == false)
				sprite->Play("Idle");
				break;
			case SDLK_LEFT:
				left = false;
				if(right == false)
				transform->velocity.x = 0;
				if (down == false && up == false && right == false)
				sprite->Play("Idle");
				sprite->spriteFlip = SDL_FLIP_NONE;
				
				break;

			case SDLK_RIGHT:
				right = false;
				if(left == false)
				transform->velocity.x = 0;
				if (down == false && up == false && left == false)
				sprite->Play("Idle");
				break;

			case SDLK_DOWN:
				down = false;
				if(up == false)
				transform->velocity.y = 0;
				if (up == false && right == false && left == false)
				sprite->Play("Idle");
				break;
			case SDLK_SPACE:

				
			
				if (up == false && right == false && left == false && down == false && shoot == true)
				{
					shoot = false;
				
					sprite->PlayOnce("Shoot");
					
				
				}
				
				
				
				break;
			case SDLK_ESCAPE:
				Game::isRunning = false;
			default:
				break;
			}
		}
	}
};
