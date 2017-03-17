#pragma once
#include <SDL2/SDL.h>
#include <list>

#include "../headers/GameStateManager.h"
#include "../headers/Background.h"
#include "../headers/TileMap.h"
#include "../headers/AudioPlayer.h"
#include "../headers/BgObject.h"
#include "../headers/Letters.h"
#include "../headers/Pig_Player.h"
#include "../headers/Cake.h"
#include "../headers/Numbers.h"

#define NUM_OPTIONS 2
class GameStateManager;
class GameState
{
	protected:
		GameStateManager *gsm;
		SDL_Renderer *renderTarget;

	public:
		virtual void init() =0;
		virtual void update() =0;
		virtual void draw() =0;
		virtual void keyPressed(int k) =0;
		virtual void keyReleased(int k) =0;
};

class GameplayState : public GameState
{
	private:
		bool game_end;
		bool input_allowed;
		double game_timer;
		double start_time;
		int score;
		int multiplier;
		int p_X, p_Y;
		int p_W, p_H;
		Pig_Player *player;
		std::list<Cake*> cakes;

		SDL_Rect cloud_rect;
		SDL_Texture *clouds;
		
		SDL_Rect ground_rect;
		SDL_Texture *ground;

		Numbers *score_display;
		Numbers *time_display;

		AudioPlayer **level_sounds;
		
		int cake_spawn_time;
		double cake_timer;
		void spawnCake();		
		int getMultiplier();
		void updateScore(int val);
	public:
		enum Level_Sounds{CAKE_SFX, SFX_NUM};
		GameplayState(GameStateManager *gsm_, SDL_Renderer *renderTarget_);
		~GameplayState();
		void init();
		void update();
		void draw();
		void keyPressed(int k);
		void keyReleased(int k);
};

