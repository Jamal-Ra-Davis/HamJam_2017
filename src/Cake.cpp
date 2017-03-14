#include <stdio.h>
#include "../headers/Cake.h"
#include "../headers/SDL_Helpers.h"


Cake::Cake(TileMap *tm, SDL_Renderer *renderTarget_, int value_) : MapObject(tm, renderTarget_)
{
	value = value_;
	collected = false;
	should_remove = false;
	visible = true;
	setVector(0, 2);

	width = 30;
	height = 30;
	cwidth = 18;
	cheight = 18;

	facingRight = true;

	//load sprites
	//numAnimations = NUM_ANIMATIONS;
    //numAnimations = 8;
    frameNumbers = NULL;
    frameNumbers = new int[NUM_ANIMATIONS];
    int tempNums[NUM_ANIMATIONS] = {1, 4, 5};
    for (int i=0; i<NUM_ANIMATIONS; i++)
        frameNumbers[i] = tempNums[i];

    sprite_rects = NULL;
    sprite_rects = new SDL_Rect*[NUM_ANIMATIONS];
    for (int i=0; i<NUM_ANIMATIONS; i++)
    {
        sprite_rects[i] = NULL;
        sprite_rects[i] = new SDL_Rect[frameNumbers[i]];
        //printf("DEBUG: frameNumbers[%d] = %d\n", i, frameNumbers[i]);
    }

	animationTexture = LoadTexture("./Resources/Sprites/Cake_spritesheet.bmp", renderTarget);
    //if (animationTexture)
    //    printf("DEBUG: Loaded player textures\n");
    for (int i=0; i<NUM_ANIMATIONS; i++)
    {
        for (int j=0; j<frameNumbers[i]; j++)
        {
        	sprite_rects[i][j].x = j*width;
            sprite_rects[i][j].y = i*height;
            sprite_rects[i][j].w = width;
            sprite_rects[i][j].h = height;
        }
    }

	currentAction = DEFAULT;
    animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
    animation.setDelay(-1);
    width = sprite_rects[currentAction][animation.getFrame()].w;

	//coin_sound =  new AudioPlayer("./Resources/SFX/BMOGame_CoinCollect.wav", false);	
}

Cake::~Cake()
{
	delete frameNumbers;
    frameNumbers = NULL;
    for (int i=0; i<NUM_ANIMATIONS; i++)
    {
        delete [] sprite_rects[i];
        sprite_rects[i] = NULL;
    }
    delete [] sprite_rects;
    sprite_rects = NULL;

    SDL_DestroyTexture(animationTexture);
    animationTexture = NULL;

	//if (coin_sound)
	//	delete coin_sound;
	//coin_sound = NULL;
}

void Cake::collect()
{
	collected = true;
	currentAction = EAT_START;
    animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction], true);
    animation.setDelay(100);
    width = sprite_rects[currentAction][animation.getFrame()].w;
	//coin_sound->play(0);
}
void Cake::update()
{
	if (should_remove)
		return;

	if (!collected )
	{
		x += dx;
		y += dy;
	}


	if (currentAction == EAT_START)
	{
		if (animation.hasPlayedOnce())
		{
			
    		currentAction = EAT_END;
    		animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction], true);
    		animation.setDelay(100);
    		width = sprite_rects[currentAction][animation.getFrame()].w;
		}
	}
	else if (currentAction == EAT_END)
    {	
		if(animation.hasPlayedOnce())
			should_remove = true;
    }
	//printf("currentAction = %d\n", currentAction);
	//printf("animation.getFrame() = %d\n", animation.getFrame());
	//printf("");
	//if (!should_remove)
	animation.update();
}
void Cake::draw()
{

	if (should_remove)
		return;
	if (!visible)
        return;

    SDL_Rect rect = getCollisionRect();
    //drawRect(rect);
    //mapObjectDraw();
    SDL_Rect posRect = {(int)(x - width/2), (int)(y - height/2), width, height};
    SDL_Rect cropRect = animation.getImageRect();
    if (facingRight)
    {
        SDL_RenderCopyEx(renderTarget, animation.getFrameTexture(), &cropRect, &posRect, 0, NULL, SDL_FLIP_NONE);
    }
    else
    {
        SDL_RenderCopyEx(renderTarget, animation.getFrameTexture(), &cropRect, &posRect, 0, NULL, SDL_FLIP_HORIZONTAL);
    }
}
