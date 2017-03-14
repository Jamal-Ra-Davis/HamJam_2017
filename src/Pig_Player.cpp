#include <time.h>

#include "../headers/Pig_Player.h"
#include "../headers/SDL_Helpers.h"
#include "../headers/GamePanel.h"


Pig_Player::Pig_Player(TileMap *tm, SDL_Renderer *renderTarget_) : MapObject(tm, renderTarget_)
{
	dead = false;
	onGround = false;
	falling = true;	
	canDoubleJump = true;
	canDash = true;
	dashing = false;
	dashStart = false;
	dash_end = false;	
	landing = false;
	
	visible = true;	


	width = 60;
    height = 50;
    cwidth = 40;
    cheight = 30;//27;
	cOffset_x = 0;
	cOffset_y = 10;	

    maxSpeed = 6;
	moveSpeed = 0.35;//maxSpeed;
    stopSpeed = 0.35;//maxSpeed;

    fallSpeed = 0.35;
    maxFallSpeed = 15.0;
    jumpStart = -10;

    facingRight = true;

	//jumpInit = false;

	//load sprites
    numAnimations = ANM_NUM;
    frameNumbers = NULL;
    frameNumbers = new int[numAnimations];
    int tempNums[ANM_NUM] = {1, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 1};
    for (int i=0; i<numAnimations; i++)
        frameNumbers[i] = tempNums[i];

    sprite_rects = NULL;
    sprite_rects = new SDL_Rect*[numAnimations];
    for (int i=0; i<numAnimations; i++)
    {
        sprite_rects[i] = NULL;
        sprite_rects[i] = new SDL_Rect[frameNumbers[i]];
        //printf("DEBUG: frameNumbers[%d] = %d\n", i, frameNumbers[i]);
    }
	
	animationTexture = LoadTexture("./Resources/Sprites/Player/Pig_spritesheet.bmp", renderTarget);
    if (animationTexture)
        printf("DEBUG: Loaded player textures\n");
    for (int i=0; i<numAnimations; i++)
    {
        for (int j=0; j<frameNumbers[i]; j++)
        {
         	sprite_rects[i][j].x = j*width;
        	sprite_rects[i][j].y = i*height;
        	sprite_rects[i][j].w = width;
        	sprite_rects[i][j].h = height;
        }
    }

	initAnimation();

/*
	//Audio loading
    player_sounds = NULL;
    player_sounds = new AudioPlayer*[SFX_NUM];
    for (int i=0; i<SFX_NUM; i++)
    {
        player_sounds[i] = NULL;
    }
	player_sounds[DANCE_SFX] = new AudioPlayer("./Resources/SFX/BMOGame_DanceAnim.wav", false);
	player_sounds[WALK_SFX] = new AudioPlayer("./Resources/SFX/BMOGame_Footstep.wav", false);
    player_sounds[JUMP_SFX] = new AudioPlayer("./Resources/SFX/BMOGame_Jump.wav", false);
	player_sounds[KICK_SFX] = new AudioPlayer("./Resources/SFX/BMOGame_Kick.wav", false);
	player_sounds[HIT_SFX] = new AudioPlayer("./Resources/SFX/BMOGame_Hit.wav", false);
    player_sounds[DIE_SFX] = new AudioPlayer("./Resources/SFX/BMOGame_Die.wav", false);
	player_sounds[BOMBA_START_SFX] = new AudioPlayer("./Resources/SFX/BMOGame_TakeOutBomba.wav", false);
	player_sounds[BOMBA_FUSE_SFX] = new AudioPlayer("./Resources/SFX/BMOGame_BombaFuse.wav", false);
	player_sounds[BOMBA_THROW_SFX] = new AudioPlayer("./Resources/SFX/scratch.wav", false);
	player_sounds[BOMBA_HIT_SFX] = new AudioPlayer("./Resources/SFX/BMOGame_BombaExplodes.wav", false);
*/
	
}
Pig_Player::~Pig_Player()
{
    delete frameNumbers;
    frameNumbers = NULL;
    for (int i=0; i<numAnimations; i++)
    {
        delete [] sprite_rects[i];
        sprite_rects[i] = NULL;
    }
    delete [] sprite_rects;
    sprite_rects = NULL;

    SDL_DestroyTexture(animationTexture);
    animationTexture = NULL;

/*
    for (int i=0; i<SFX_NUM; i++)
    {
        delete player_sounds[i];
        player_sounds[i] = NULL;
    }
    delete player_sounds;
    player_sounds = NULL;
*/
}
void Pig_Player::getNextPosition()
{
	//Fall
   	dy += fallSpeed;
/*
	if (onGround)
	{
    	if (left)
    	{
			if (dx > 0)
        		dx -= moveSpeed*2;
			else
				dx -= moveSpeed;
        	if (dx < -maxSpeed)
        	{
            	dx = -maxSpeed;
        	}	
    	}
    	else if (right)
    	{
			if (dx < 0)
				dx += moveSpeed*2;
			else
        		dx += moveSpeed;
        	if (dx > maxSpeed)
        	{
            	dx = maxSpeed;
        	}
   		}
    	else
    	{
        	if (dx > 0)
        	{
            	dx -= stopSpeed;
            	if (dx < 0)
                	dx = 0;
        	}
        	else if (dx < 0)
        	{
            	dx += stopSpeed;
            	if (dx > 0)
                	dx = 0;
        	}
    	}
	}
	else
	{
		if (left)
		{
			dx -= moveSpeed/DRIFT_VAL;
			if (dx < -maxSpeed)
			{
				dx = -maxSpeed;
			}
		}
		else if (right)
		{
			dx += moveSpeed/DRIFT_VAL;
            if (dx > maxSpeed)
            {
                dx = maxSpeed;
            }
		}
	}
*/
	if (dashing)
	{
		dy = 0;
		if (dashStart)
		{
			if (left)
			{
				dx = -dashSpeed;
			}
			else if (right)
			{
				dx = dashSpeed;
			}
			else if (facingRight)
			{
				dx = dashSpeed;
			}
			else
			{
				dx = -dashSpeed;
			}
		}
	}
	else if (jumping)
	{
		if (onGround)
		{
			onGround = false;
			dy = jumpStart;
		}
		else if (canDoubleJump)
		{
			canDoubleJump = false;
			dy = jumpStart*0.85;
			if (right)
			{
				/*
				if (dx >= 0)
					dx = dx;
				else 
					dx = -dx;
				*/
				dx = maxSpeed*0.75;
			}
			else if (left)
			{	
				/*
				if (dx <= 0)
					dx = dx;
				else
					dx = -dx;
				*/
				dx = -maxSpeed*0.75;
			}
			else
				dx = 0;	
		}
	}
	else
	{
		if (/*!isBusy()*/onGround)
    	{
        	if (left)
        	{
            	if (dx > 0)
                	dx -= moveSpeed*1.5;
            	else
                	dx -= moveSpeed;
            	if (dx < -maxSpeed)
            	{
               	 	dx = -maxSpeed;
            	}	
        	}
        	else if (right)
        	{
            	if (dx < 0)
                	dx += moveSpeed*1.5;
            	else
                	dx += moveSpeed;
            	if (dx > maxSpeed)
            	{
                	dx = maxSpeed;
            	}
        	}
        	else
        	{
            	if (dx > 0)
            	{
                	dx -= stopSpeed;
                	if (dx < 0)
                    	dx = 0;
            	}
            	else if (dx < 0)
            	{
                	dx += stopSpeed;
                	if (dx > 0)
                    	dx = 0;
            	}
        	}
    	}
		else
		{
			 if (left)
        	{
            	dx -= moveSpeed/DRIFT_VAL;
            	if (dx < -maxSpeed)
            	{
                	dx = -maxSpeed;
            	}
        }
        	else if (right)
        	{
            	dx += moveSpeed/DRIFT_VAL;
            	if (dx > maxSpeed)
            	{
                	dx = maxSpeed;
            	}
        	}
		}
	}

	if (dy > maxFallSpeed)
        dy = maxFallSpeed;

	if (!dashing)
	{
		if (dx < -maxSpeed)
			dx = -maxSpeed;
		if (dx > maxSpeed)
			dx = maxSpeed;
	}

/*
	//falling
    if (!dashing)
    {
        dy += fallSpeed;
        if (dy > maxFallSpeed)
            dy = maxFallSpeed;
    }
*/
}
void Pig_Player::initAnimation()
{
	currentAction = STANDING;
    animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
    animation.setDelay(-1);
    width = sprite_rects[currentAction][animation.getFrame()].w;
}
void Pig_Player::dashAction()
{
	if (canDash && !dashing)
	{
		dashStart = true;
		dashStartX = x;
		dashStartY = y;
		dashing = true;
		jumping = false;
		canDash = false;
		//set dash start animation
	}
}
void Pig_Player::dashStop()
{
	dashing = false;
	//dx = 0;
}
void Pig_Player::setJumping(bool b)
{
	if (!canDoubleJump)
		return;
	jumping = b;
	if (jumping)
		dashStop();
}
SDL_Rect Pig_Player::getCollisionRect()
{
	//printf("In Pig collision rect funtion\n");
	SDL_Rect cRect;
    cRect.x = (int)(x - cwidth/2) + cOffset_x;
    cRect.y = (int)(y - cheight/2) + cOffset_y;
    cRect.w = cwidth;
    cRect.h = cheight;
    return cRect;
}
void Pig_Player::reset(double x, double y)
{
    dead = false;
    onGround = false;
	visible = true;
	canDoubleJump = true;
	canDash = true;
    dashing = false;
	dashStart = false;
    dash_end = false;
	landing = false;	

	left = false;
    right = false;
    up = false;
    down = false;
    jumping = false;
    falling = false;
	
	setVector(0, 0);
	setPosition(x, y);

	initAnimation();
}
void Pig_Player::update()
{
	//update position
    getNextPosition();
	xdest = x + dx;
	ydest = y + dy;


	onGround = false;
	if (xdest - cwidth/2 + cOffset_x < 0)
	{
		xdest = cwidth/2 - cOffset_x;
		dx = 0;
		if (dashing)
		{
			dashStop();
			dash_end = true;
		}
	}
	else if (xdest + cwidth/2 + cOffset_x > GamePanel::WINDOW_WIDTH)
	{
		xdest = GamePanel::WINDOW_WIDTH - cwidth/2 - cOffset_x;
		dx = 0;
		if (dashing)
		{
			dashStop();
			dash_end = true;
		}
	}
	if (ydest + cheight/2 + cOffset_y > GamePanel::WINDOW_HEIGHT)
	{
		if (dy > -jumpStart/4)
		{
			landing = true;
		}

		ydest = GamePanel::WINDOW_HEIGHT - cheight/2 - cOffset_y;
		dy = 0;
		onGround = true;
		canDoubleJump = true;
		canDash = true;
		if (dashing)
			dashStop();
	}

/*
	if (dashing)
	{
		//printf("Dashing\n");
		dashStart = false;
		double delta = x - dashStartX;
		if (delta < 0)
			delta *= -1;
		if (delta >= DASH_LEN)
		{
			dashStop();
		}
	}
*/	
	//if (onGround)
	//	printf("On Ground : %lf\n", getMs());

	x = xdest;
	y = ydest;

	if (dashing)
	{
		if (dashStart)
		{
			dashStart = false;
			currentAction = DASH_START;
        	animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction], true);
        	animation.setDelay(50);
        	width = sprite_rects[currentAction][animation.getFrame()].w;
		}
		else
		{
			if (currentAction != DASHING)
			{
				currentAction = DASHING;
           		animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
            	animation.setDelay(50);
            	width = sprite_rects[currentAction][animation.getFrame()].w;
			}
		}
		double delta = x - dashStartX;
        if (delta < 0)
            delta *= -1;
        if (delta >= DASH_LEN)
        {
            dashStop();
			//dash_end = true;
        }
	}
	else if (dash_end)
	{
		if (currentAction != DASH_STOP)
		{
			currentAction = DASH_STOP;
            animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction], true);
            animation.setDelay(100);
            width = sprite_rects[currentAction][animation.getFrame()].w;
		}	
		if (animation.hasPlayedOnce())
		{
			dash_end = false;
		}
	}
	else if (jumping)
	{
		currentAction = JUMP_START;
       	animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction], true);
       	animation.setDelay(100);
       	width = sprite_rects[currentAction][animation.getFrame()].w;
	}
	else if (!onGround && dy > 0)
	{
		if (currentAction != FALLING)
		{
			currentAction = FALLING;
            animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
            animation.setDelay(75);
            width = sprite_rects[currentAction][animation.getFrame()].w;
		}	
	}
	else if (currentAction == JUMP_START)
	{
		if (animation.hasPlayedOnce())
		{
			currentAction = JUMP_PEAK;
        	animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
        	animation.setDelay(75);
        	width = sprite_rects[currentAction][animation.getFrame()].w;
		}
	}
	else if (currentAction == JUMP_PEAK)
	{
		
	}
	else if (landing)
	{
		if (currentAction != LANDING)
		{
			currentAction = LANDING;
            animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction], true);
            animation.setDelay(35);
            width = sprite_rects[currentAction][animation.getFrame()].w;
		}
		else if (animation.hasPlayedOnce())
		{
			landing = false;
		}
	}
	else if (onGround && (left || right))
	{
		if ((facingRight && dx < 0) || (!facingRight && dx > 0))
		{
			if (currentAction != SLIDE)
            {
                currentAction = SLIDE;
                animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
                animation.setDelay(-1);
                width = sprite_rects[currentAction][animation.getFrame()].w;
            }
		}
		else
		{ 
			if (currentAction != WALKING)
			{
				currentAction = WALKING;
            	animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
            	animation.setDelay(100);
            	width = sprite_rects[currentAction][animation.getFrame()].w;
			}
		}
	}
	else if (onGround && dx != 0)
	{
		if ((facingRight && dx > 0) || (!facingRight && dx < 0))
		{
			if (currentAction != BRAKING_F)
			{
				currentAction = BRAKING_F;
            	animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
            	animation.setDelay(-1);
            	width = sprite_rects[currentAction][animation.getFrame()].w;
			}
		}
		else if ((facingRight && dx < 0) || (!facingRight && dx > 0))
		{
			if (currentAction != BRAKING_R)
            {
                currentAction = BRAKING_R;
                animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
                animation.setDelay(-1);
                width = sprite_rects[currentAction][animation.getFrame()].w;
            }
		}
		else
		{
			if (currentAction != STANDING)
        	{
            	initAnimation();
        	}
		}
	}
	else
	{
		if (currentAction != STANDING)
		{
			initAnimation();
		}		
	}


	jumping = false;
	animation.update();
	if (/*onGround*/!dashing)
	{
        if (right)
            facingRight = true;
        else if (left)
            facingRight = false;
	}
	else
	{
		if (dx >= 0)
			facingRight = true;
		else
			facingRight = false;	
	}
}
void Pig_Player::draw()
{
	if (!visible)
		return;

	SDL_Rect rect = this->getCollisionRect();
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

