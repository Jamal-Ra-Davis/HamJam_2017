#pragma once
#include <SDL2/SDL.h>
#include "../headers/MapObject.h"
#include "../headers/AudioPlayer.h"

class Cake : public MapObject
{
	private:
		int value;
		bool collected;
		bool should_remove;
		bool visible;

		//Animations
        //int numAnimations;
        SDL_Texture *animationTexture;
        SDL_Rect **sprite_rects;
        int *frameNumbers;
		//AudioPlayer *coin_sound;		

	public:
		enum Cake_States {DEFAULT, EAT_START, EAT_END, NUM_ANIMATIONS};
		Cake(TileMap *tm, SDL_Renderer *renderTarget_, int value_);
		~Cake();

		bool isCollected() {return collected;}
		bool shouldRemove() {return should_remove;}	
		int getValue() {return value;}

		void collect();
		void update();
		void draw();
};
