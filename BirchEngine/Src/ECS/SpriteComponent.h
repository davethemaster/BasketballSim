#pragma once

#include "Components.h"
#include "SDL.h"
#include "../TextureManager.h"
#include "Animation.h"
#include <map>
#include "../AssetManager.h"

class SpriteComponent : public Component
{
private:
	TransformComponent * transform;
	SDL_Texture *texture;
	SDL_Rect srcRect, destRect;

	bool animated = false;
	int frames = 0;
	int speed = 100;
	bool once = false;
	int currFrame = 0;
public:

	int Num;
	int animIndex = 0;

	std::map<const char*, Animation> animations;

	SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

	SpriteComponent() = default;
	SpriteComponent(std::string id)
	{
		setTex(id);
	}



	SpriteComponent(std::string id, bool isAnimated)
	{
		animated = isAnimated;

		
		Animation idle = Animation(0, 3, 100);
		Animation walk = Animation(1, 8, 100);
		Animation up = Animation(2, 8, 100);
		Animation down = Animation(3, 8, 100);
		Animation shoot = Animation(4, 6, 150);
		Animation dribble = Animation(0, 5, 100);
	


		animations.emplace("Dribble", dribble);
		animations.emplace("Idle", idle);
		animations.emplace("Walk", walk);
		animations.emplace("Up", up);
		animations.emplace("Down", down);
		animations.emplace("Shoot", shoot);
		

		Play("Idle");
	
		setTex(id);
	}


	~SpriteComponent()
	{
		
	}
	void setTex(std::string id)
	{
		texture = Game::assets->GetTexture(id);
	}
	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();

		srcRect.x = srcRect.y = 0;
		srcRect.w = transform->width;
		srcRect.h = transform->height;
		
	}

	void update() override
	{

		if (animated && once == false) 
		{
			srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
		}
		else if (animated && once == true)
		{
			srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
			int frameNum = static_cast<int>((SDL_GetTicks() / speed) % frames);
			if (currFrame != frameNum)
			{
				currFrame = frameNum;
				Num++;
				if (Num > frames)
				{
					Play("Idle");
				}
			}
		}

		srcRect.y = animIndex * transform->height;

		destRect.x = static_cast<int>(transform->position.x - Game::camera.x);
        destRect.y = static_cast<int>(transform->position.y - Game::camera.y);
		destRect.w = transform->width * transform->scale;
	    destRect.h = transform->height * transform->scale;
 	}

	void draw() override
	{
		TextureManager::Draw(texture, srcRect, destRect, spriteFlip);
	}

	void Play(const char* animName)
	{
		frames = animations[animName].frames;
		animIndex = animations[animName].index;
		speed = animations[animName].speed;
		once = false;
	}
	void PlayOnce(const char* animName)
	{
		Num = 0;
		frames = animations[animName].frames;
		currFrame = -1;
		animIndex = animations[animName].index;
		speed = animations[animName].speed;
		once = true;
	}
	

};