#include <stdio.h>
#include <string>
#include "../headers/GameState.h"
#include "../headers/SDL_Helpers.h"
#include "../headers/GamePanel.h"
#include "../headers/TileMap.h"

GameplayState::GameplayState(GameStateManager *gsm_, SDL_Renderer *renderTarget_)
{
	gsm = gsm_;
	renderTarget = renderTarget_;
	p_X = 100;
	p_Y = 100;
	p_H = 50;
	p_W = 60;
	player = NULL;
	clouds = NULL;
	ground = NULL;

	score_display = NULL;
	time_display = NULL;

	level_sounds = NULL;
	bgm = NULL;

	cake_spawn_time = -1;
	score = 0;
	multiplier = 0;	
	input_allowed = true;
	game_end = false;
	game_timer = 120000;
}
GameplayState::~GameplayState()
{
	if (player)
		delete player;
	player = NULL;

	std::list<Cake*>::iterator cakes_iter;
    for (cakes_iter=cakes.begin(); cakes_iter != cakes.end(); )
    {
        Cake *cake = *cakes_iter;
        delete cake;
        *cakes_iter = NULL;
        cakes_iter = cakes.erase(cakes_iter);
    }
	if (clouds)
		SDL_DestroyTexture(clouds);
	clouds = NULL;
	if (ground)
		SDL_DestroyTexture(ground);
	ground = NULL;

	if (score_display)
		delete score_display;
	score_display = NULL;
	if (time_display)
		delete time_display;
	time_display = NULL;


	for (int i=0; i<SFX_NUM; i++)
    {
        if (level_sounds[i])
            delete level_sounds[i];
        level_sounds[i] = NULL;
    }
    delete level_sounds;
    level_sounds = NULL;

	if (bgm)
		delete bgm;
	bgm = NULL;
}
void GameplayState::init()
{
	player = new Pig_Player(NULL, renderTarget);
    player->setPosition(GamePanel::WINDOW_WIDTH/2, GamePanel::WINDOW_HEIGHT - 50);//75, 0
    player->setVector(0, 0);

	clouds = LoadTexture("./Resources/Backgrounds/Clouds.bmp", renderTarget);
	if (!clouds)
		printf("Failed to load clouds\n");
	SDL_QueryTexture(clouds, NULL, NULL, &(cloud_rect.w), &(cloud_rect.h));
	cloud_rect.x = 0;
	cloud_rect.y = 0;

	ground = LoadTexture("./Resources/Backgrounds/ground.bmp", renderTarget);
	if (!ground)
        printf("Failed to load ground\n");
	SDL_QueryTexture(ground, NULL, NULL, &(ground_rect.w), &(ground_rect.h));
	ground_rect.x = 0;
	ground_rect.y = GamePanel::WINDOW_HEIGHT - ground_rect.h;

	score_display = new Numbers(renderTarget, 6, 2, 5, 5);
	time_display = new Numbers(renderTarget, 3, 2, 0, 5);
	time_display->setX(GamePanel::WINDOW_WIDTH - time_display->getWidth() - 5);
/*
	cake = new Cake(NULL, renderTarget, 100);
	cake->setPosition(150, 150);
	cake->setVector(0, 1);
*/

	level_sounds = NULL;
    level_sounds = new AudioPlayer*[SFX_NUM];
    for (int i=0; i<SFX_NUM; i++)
    {
        level_sounds[i] = NULL;
    }
    level_sounds[CAKE_SFX] = new AudioPlayer("./Resources/SFX/HamJam_Pickup.wav", false);

	bgm = new AudioPlayer("./Resources/Music/Digital_Lemonade.mp3", true);

	cake_timer = getMs();

	spawnCake();

	bgm->play(AudioPlayer::LOOP);

	start_time = getMs();	

}
void GameplayState::spawnCake()
{
	if (game_end)
		return;
	if (getMs() - cake_timer >= cake_spawn_time)
    {
        Cake *c = new Cake(NULL, renderTarget, 10);
        int margin = (int)(1.5*c->getWidth());
        c->setPosition(margin + rand()%(GamePanel::WINDOW_WIDTH - 2*margin), -c->getHeight());
        cakes.push_back(c);

        cake_spawn_time = 800 + rand()%500 + 1;
        cake_timer = getMs();
    }
}
int GameplayState::getMultiplier()
{
	return multiplier/3 + 1;
}
void GameplayState::updateScore(int val)
{
	if (game_end)
		return;
	score += val;
}
void GameplayState::update()
{
	player->update();
	if (player->isOnGround())
	{
		multiplier = 0;
	}

	std::list<Cake*>::iterator iter;
    for (iter=cakes.begin(); iter != cakes.end(); /*++iter*/)
    {
        Cake *cake = (*iter);
        cake->update();
        if (player->intersects(cake) && !cake->isCollected())
        {
            cake->collect();
			level_sounds[CAKE_SFX]->play(0);
			player->recharge();
			if (!game_end)
				multiplier++;
			updateScore(cake->getValue()*getMultiplier());
			//printf("Score: %d, Multipler: %d\n", score, getMultiplier());
        }
        if (cake->shouldRemove() || cake->getY() > GamePanel::WINDOW_HEIGHT*2)
        {
            delete cake;
            *iter = NULL;
            iter = cakes.erase(iter);

        }
        else
        {
            ++iter;
        }
    }




	spawnCake();	
	//printf("Time Left: %.2lf\n", (game_timer - (getMs() - start_time))/1000);
	double remaining_time = game_timer - (getMs() - start_time);
	if (remaining_time <= 0)
	{
		game_end = true;
		input_allowed = false;
		player->setLeft(false);
		player->setRight(false);
		player->setUp(false);
		player->setDown(false);
		//printf("Game End\n");
	}
	if (game_end && remaining_time < -5000)
	{
		bgm->stop();
		gsm->setScore(score);
		gsm->setState(GameStateManager::WIN_STATE);
		return;
	}
}
void GameplayState::draw()
{
	SDL_RenderCopyEx(renderTarget, ground, NULL, &ground_rect, 0, NULL, SDL_FLIP_NONE);	

	std::list<Cake*>::iterator iter;
    for (iter=cakes.begin(); iter != cakes.end(); ++iter)
    {
        (*iter)->draw();
    }

	player->draw();

	SDL_RenderCopyEx(renderTarget, clouds, NULL, &cloud_rect, 0, NULL, SDL_FLIP_NONE);
	score_display->draw(score);
	int time = (int)((game_timer - (getMs() - start_time))/1000);
	if (time < 0)
		time = 0;
	time_display->draw(time);
}
void GameplayState::keyPressed(int k)
{
	switch (k)
    {
        case SDLK_LEFT:
        {
			if (input_allowed)
				player->setLeft(true);	
            break;
        }
        case SDLK_RIGHT:
        {
			if (input_allowed)
				player->setRight(true);
            break;
        }
        case SDLK_UP:
        {
			if (input_allowed)
				player->setUp(true);
            break;
        }
        case SDLK_DOWN:
		{
			if (input_allowed)
				player->setDown(true);
			break;
		}
		case SDLK_SPACE:
        {
			if (input_allowed)
            	player->setJumping(true);
            break;
        }
		case SDLK_z:
		{
			if (input_allowed)
				player->dashAction();
			break;
		}
	}
}
void GameplayState::keyReleased(int k)
{
	switch (k)
    {
        case SDLK_LEFT:
        {
            player->setLeft(false);
            p_X -= 5;
            break;
        }
        case SDLK_RIGHT:
        {
            player->setRight(false);
            p_X += 5;
            break;
        }
        case SDLK_UP:
        {
            player->setUp(false);
            p_Y -= 5;
            break;
        }
        case SDLK_DOWN:
        {
            player->setDown(false);
            p_Y += 5;
            break;
        }
		case SDLK_SPACE:
        {
            //player->setJumping(false);
            break;
        }
    }
}


WinState::WinState(GameStateManager *gsm_, SDL_Renderer *renderTarget_)
{
    gsm = gsm_;
    renderTarget = renderTarget_;
	words = NULL;
	digits = NULL;

	animationTexture = NULL;
	sprite_rects = NULL;
	frameNumbers = NULL;

	score_breaks[0] = 500;
	score_breaks[1] = 1250;
	score_breaks[2] = 2000;
	score_breaks[3] = 3000;

	numAnimations = PIC_NUM;
}
WinState::~WinState()
{
	if (words)
		delete words;
	words = NULL;

	if (digits)
		delete digits;
	digits = NULL;

	
	if (animationTexture)
        SDL_DestroyTexture(animationTexture);
    animationTexture = NULL;

    if (sprite_rects)
    {
        for (int i=0; i<numAnimations; i++)
        {
            if (sprite_rects[i])
                delete [] sprite_rects[i];
            sprite_rects[i] = NULL;
        }
        delete [] sprite_rects;
        sprite_rects = NULL;
    }

    if (frameNumbers)
        delete[] frameNumbers;
    frameNumbers = NULL;
}
void WinState::init()
{
	name_idx = 0;
	letter_idx = 0;
	up = false;
	down = false;
	enter = false;
	back = false;

	for (int i=0; i<4; i++)
	{
		score_name[i] = '\0';
	}

	score_val = gsm->getScore();

	sprintf(high_score_filename, "./Resources/Files/High_Scores.txt");

    FILE *fp = fopen(high_score_filename, "r");
    if (fp == NULL)
        printf("Error, failed to open file: %s\n",  high_score_filename);

	set_high_score = false;
	insert_idx = -1;
	for (int i=0; i<MAX_SCORES; i++)
	{
		fscanf(fp, "%s %d ", score_names[i], &(high_scores[i]));

		//printf("%d\n", high_scores[i]);
		if (score_val > high_scores[i] && !set_high_score)
		{
			set_high_score = true;
			insert_idx = i;
		}
	}
	fclose(fp);

	words = new Letters(renderTarget, "", 1, 0, 0);
	digits = new Numbers(renderTarget, 6, 2, 5, 5);


	score_tally = 0;
	score_scale = score_val/(60.0 * TALLY_TIME);
	if (score_scale > MAX_SCORE_SCALE)
		score_scale = MAX_SCORE_SCALE;
	tallying = true;

	pic_x = 55;
	pic_y = 55;
	pic_width = 250;
	pic_height = 250;
	
	
	frameNumbers = NULL;
    frameNumbers = new int[numAnimations];
    int tempNums[PIC_NUM] = {1, 1, 1, 1, 1};
    for (int i=0; i<numAnimations; i++)
        frameNumbers[i] = tempNums[i];

    sprite_rects = new SDL_Rect*[numAnimations];
    for (int i=0; i<numAnimations; i++)
    {
        sprite_rects[i] = NULL;
        sprite_rects[i] = new SDL_Rect[frameNumbers[i]];
    }
    animationTexture = LoadTexture("./Resources/Sprites/Winscreen_pig_sprites.bmp", renderTarget);
    if (animationTexture)
        printf("DEBUG: Loaded title background\n");
    for (int i=0; i<numAnimations; i++)
    {
        for (int j=0; j<frameNumbers[i]; j++)
        {
            sprite_rects[i][j].x = j*pic_width;
            sprite_rects[i][j].y = i*pic_height;
            sprite_rects[i][j].w = pic_width;
            sprite_rects[i][j].h = pic_height;
        }
    }

	currentAction = STARVING;
    animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
    animation.setDelay(-1);
    pic_width = sprite_rects[currentAction][animation.getFrame()].w;

	
}
void WinState::update()
{
	if (tallying)
	{
		if (score_tally < score_val)
		{
			score_tally += score_scale;
			if (score_tally > score_val)
			{
				score_tally = score_val;
			}
		
			int sc_val = (int)score_tally;
			if (sc_val < score_breaks[0])
			{
				if (currentAction != STARVING)
				{
					currentAction = STARVING;
    				animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
    				animation.setDelay(-1);
    				pic_width = sprite_rects[currentAction][animation.getFrame()].w;
				}
			}
			else if (sc_val < score_breaks[1])
        	{
            	if (currentAction != NORMAL)
            	{
                	currentAction = NORMAL;
                	animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
                	animation.setDelay(-1);
                	pic_width = sprite_rects[currentAction][animation.getFrame()].w;
            	}
        	}
			else if (sc_val < score_breaks[2])
        	{
            	if (currentAction != HAPPY)
            	{
                	currentAction = HAPPY;
                	animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
                	animation.setDelay(-1);
                	pic_width = sprite_rects[currentAction][animation.getFrame()].w;
            	}
        	}
			else if (sc_val < score_breaks[3])
        	{
            	if (currentAction != PUDGY)
            	{
                	currentAction = PUDGY;
                	animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
                	animation.setDelay(-1);
                	pic_width = sprite_rects[currentAction][animation.getFrame()].w;
            	}
        	}
			else
			{
				if (currentAction != FAT)
            	{
                	currentAction = FAT;
                	animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
                	animation.setDelay(-1);
                	pic_width = sprite_rects[currentAction][animation.getFrame()].w;
            	}
			}
		}

		if ((int)score_tally == score_val && enter)
			tallying = false;

		up = false;
    	down = false;
    	enter = false;
    	back = false;
		return;
	}

	if (set_high_score)
	{
		//Enterting name characters
        if (name_idx < 3)
        {
			if (up)
				letter_idx++;
			else if (down)
				letter_idx--;

			if (letter_idx > 25)
				letter_idx = 0;
			if (letter_idx < 0)
				letter_idx = 25;

			if (enter)
			{
				score_name[name_idx] = (char)(letter_idx + 65);
				letter_idx = 0;
			}
        }
        //Display confirmation      
        else if(name_idx == 3)
        {

        }
        //
        else
        {
            set_high_score = false;

			for (int i=MAX_SCORES-1; i > insert_idx; i--)
			{
				high_scores[i] = high_scores[i-1];
				sprintf(score_names[i], "%s", score_names[i-1]);
			}
			high_scores[insert_idx] = score_val;
			sprintf(score_names[insert_idx], "%s", score_name);

			FILE *fp = fopen(high_score_filename, "w");
			for (int i=0; i<MAX_SCORES; i++)
			{
				fprintf(fp, "%s %d\n", score_names[i], high_scores[i]);

			}
			fclose(fp);	
        }


		if (enter)
            name_idx++;
        else if (back)
            name_idx--;
        if (name_idx > 4)
            name_idx = 4;
        if (name_idx < 0)
            name_idx = 0;	
	}
	else
	{
		if (enter)
		{
			gsm->setScore(0);
        	gsm->setState(GameStateManager::GAMEPLAY_STATE);
			return;
		}	
	}

	blink_timer++;
	if (blink_timer >= BLINK_PERIOD)
		blink_timer = 0;


	up = false;
    down = false;
    enter = false;
    back = false;	
}
void WinState::draw()
{
	SDL_Rect posRect = {(int)(pic_x), (int)(pic_y), pic_width, pic_height};
    SDL_Rect cropRect = animation.getImageRect();
    SDL_RenderCopyEx(renderTarget, animation.getFrameTexture(), &cropRect, &posRect, 0, NULL, SDL_FLIP_NONE);

	if (tallying)
	{
		
		//SDL_Rect posRect = {(int)(pic_x - pic_width/2), (int)(pic_y - pic_height/2), pic_width, pic_height};
		//SDL_Rect posRect = {(int)(pic_x), (int)(pic_y), pic_width, pic_height};
    	//SDL_Rect cropRect = animation.getImageRect();
    	//SDL_RenderCopyEx(renderTarget, animation.getFrameTexture(), &cropRect, &posRect, 0, NULL, SDL_FLIP_NONE);

		
		words->setVisible(true);
		words->setWord("SCORE");
		words->setX(335/2 + 305 - words->getWidth()/2);
        words->setY(100);
        words->draw();

		digits->setX(335/2 + 305 - digits->getWidth()/2);
        digits->setY(words->getY() + words->getHeight() + 10);
        digits->draw((int)score_tally);
		return;
	}

	if (set_high_score)
    {
		bool word_visible = true;
		if (name_idx < 3)
		{
			word_visible = (blink_timer < BLINK_PERIOD/2);
		}

		//Enterting name characters
		words->setVisible(true);
		words->setWord("NEW HIGH SCORE");
		words->setX(335/2 + 305 - words->getWidth()/2);
        words->setY(100);
		words->draw();

		digits->setX(335/2 + 305 - digits->getWidth()/2);
        digits->setY(words->getY() + words->getHeight() + 10);
        digits->draw((int)score_tally);

		//words->setX(100);
        //words->setY(words->getY() + words->getHeight() + 10);
		std::string str = "";
		for (int i=0; i<name_idx; i++)
			str.push_back(score_name[i]);

		words->setVisible(true);
		words->setWord(str);
		words->setX(335/2 + 273);
		words->setY(digits->getY() + digits->getHeight() + 10);
		words->draw();


		if (name_idx < 3)
		{
		str = "";
		str.push_back((char)(letter_idx + 65));		
		//words->setWord(""+(char)(letter_idx + 65));
		//words->setWord(str);
		//printf("%c\n", (char)(letter_idx + 65));
		words->setX(words->getX() + words->getWidth() + words->getSpacing());
		words->setWord(str);
		words->setVisible(word_visible);		
		words->draw();
		}

		if (name_idx == 3)
		{
			words->setWord("Confirm");
			words->setY(words->getY() + words->getHeight() + words->getSpacing());
			words->draw();
		}

	
		/*
		if (name_idx < 3)
		{
			std::string str = "";

			for (i=0; i<name_idx; i++)
				str += score_name[i];

			words->setWord(str)
			words->draw();
			words->setWord(score_name[name_idx]);
			words->setX(words->getX() + words->getWidth() + words->getSpacing());
			words->setVisible((blinkTimer < BLINK_PERIOD/2));
			words->draw();
		}	
		//Display confirmation 		
		else if(name_idx == 3)
		{
			std::string str = ""
			for (i=0; i<=name_idx; i++)
                str += score_name[i];	
			
		}
		*/
    }
    else
    {
		words->setX(335/2 + 200);
        words->setY(100);
		words->setVisible(true);

		for (int i=0; i<MAX_SCORES; i++)
		{
        	std::string name(score_names[i]);
        	words->setWord(name);
        	words->draw();	

			digits->setX(words->getX() + 100);
			digits->setY(words->getY());
			digits->draw(high_scores[i]);			

			words->setY(words->getY() + 2*words->getHeight() + words->getSpacing());	
		}	
    }
}
void WinState::keyPressed(int k)
{
    switch (k)
    {
        case SDLK_LEFT:
        {
            break;
        }
        case SDLK_RIGHT:
        {
            break;
        }
        case SDLK_UP:
        {
			up = true;
            break;
        }
        case SDLK_DOWN:
        {
			down = true;
            break;
        }
        case SDLK_SPACE:
        {
            break;
        }
		case SDLK_RETURN:
        {
			enter = true;
            break;
        }
		case SDLK_RSHIFT:
		{
			back = true;
			break;
		}
    }
}
void WinState::keyReleased(int k)
{
    switch (k)
    {
        case SDLK_LEFT:
        {
            break;
        }
        case SDLK_RIGHT:
        {
            break;
        }
        case SDLK_UP:
        {
            break;
        }
        case SDLK_DOWN:
        {
            break;
        }
        case SDLK_SPACE:
        {
            //player->setJumping(false);
            break;
        }
		case SDLK_RETURN:
        {
            break;
        }
		case SDLK_RSHIFT:
        {
            break;
        }
    }
}

TitleState::TitleState(GameStateManager *gsm_, SDL_Renderer *renderTarget_)
{
	gsm = gsm_;
    renderTarget = renderTarget_;

	bg = NULL;
	animationTexture = NULL;
	sprite_rects = NULL;
	frameNumbers = NULL;

	numAnimations = ANM_NUM;

	up = false;
    down = false;
    enter = false;
    back = false;
	timer = 0;
}
TitleState::~TitleState()
{
	if (bg)
		delete bg;
	bg = NULL;

	if (animationTexture)
		SDL_DestroyTexture(animationTexture);
    animationTexture = NULL;

	if (sprite_rects)
	{
		for (int i=0; i<numAnimations; i++)
    	{
			if (sprite_rects[i])
        		delete [] sprite_rects[i];
        	sprite_rects[i] = NULL;
    	}
		delete [] sprite_rects;
    	sprite_rects = NULL;
	}

	if (frameNumbers)
		delete[] frameNumbers;
	frameNumbers = NULL;
}
void TitleState::init()
{
	up = false;
    down = false;
    enter = false;
    back = false;
	timer = 0;

	int temp_cur_pos[NUM_CHOICES][2] = 	{
											{437, 234},
											{425, 282},
											{442, 330}
										};
	for (int i=0; i<NUM_CHOICES; i++)
	{
		cursor_positions[i][0] = temp_cur_pos[i][0];
		cursor_positions[i][1] = temp_cur_pos[i][1];
	}
	menu_choice = START;
	setCursorPos(menu_choice);

	cursor_width = 35;
	cursor_height = 35;

	bg = new Background("./Resources/Backgrounds/HamJam_Title.bmp", 1, renderTarget);
	bg->setVector(0, 0);
	bg->setPosition(0, 0);

    frameNumbers = NULL;
    frameNumbers = new int[numAnimations];
    int tempNums[ANM_NUM] = {1, 4};
	for (int i=0; i<numAnimations; i++)
        frameNumbers[i] = tempNums[i];
	
    sprite_rects = new SDL_Rect*[numAnimations];
    for (int i=0; i<numAnimations; i++)
    {
        sprite_rects[i] = NULL;
        sprite_rects[i] = new SDL_Rect[frameNumbers[i]];
    }
	animationTexture = LoadTexture("./Resources/Sprites/Pig_Cursor_sprites.bmp", renderTarget);
    if (animationTexture)
        printf("DEBUG: Loaded title background\n");
    for (int i=0; i<numAnimations; i++)
    {
        for (int j=0; j<frameNumbers[i]; j++)
        {
            sprite_rects[i][j].x = j*cursor_width;
            sprite_rects[i][j].y = i*cursor_height;
            sprite_rects[i][j].w = cursor_width;
            sprite_rects[i][j].h = cursor_height;
        }
    }

	initAnimation();
}
void TitleState::setCursorPos(int mc)
{
	if (mc >= NUM_CHOICES)
		return;
	cursor_x = cursor_positions[mc][0];
	cursor_y = cursor_positions[mc][1];
}
void TitleState::initAnimation()
{
	currentAction = IDLE;
    animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction]);
    animation.setDelay(-1);
    cursor_width = sprite_rects[currentAction][animation.getFrame()].w;
}
void TitleState::update()
{
	if (currentAction == NOD)
	{
		up = false;
    	down = false;
    	enter = false;
    	back = false;
	}

	if (up)
	{
		//printf("Increment menu choice\n");
		menu_choice--;
	}
	else if (down)
	{
		menu_choice++;
	}
	if (menu_choice >= NUM_CHOICES)
	{
		menu_choice = 0;
	}
	if (menu_choice < 0)
	{
		menu_choice = NUM_CHOICES - 1;
	}

	setCursorPos(menu_choice);


	if (enter && currentAction != NOD)
	{
		currentAction = NOD;
        animation.setFrames(animationTexture, sprite_rects[currentAction], frameNumbers[currentAction], true);
        animation.setDelay(100);
        cursor_width = sprite_rects[currentAction][animation.getFrame()].w;
	}
	if (currentAction == NOD && animation.hasPlayedOnce())
	{
		//initAnimation();
		if (timer == 0)
		{
			timer = getMs();
		}
		else if (getMs() - timer >= DELAY)
		{
			switch(menu_choice)
			{
				case START:
				{
					gsm->setState(GameStateManager::GAMEPLAY_STATE);
					return;
				}
				case CREDITS:
				{
					initAnimation();
					timer = 0;
					break;
				}
				case EXIT:
				{
					*(GamePanel::isRunningControl) = false;
					return;
				}
			}
		}
	}
	animation.update();	
	
	up = false;
    down = false;
    enter = false;
    back = false;	
}
void TitleState::draw()
{
	bg->draw();

	SDL_Rect posRect = {(int)(cursor_x - cursor_width/2), (int)(cursor_y - cursor_height/2), cursor_width, cursor_height};
    SDL_Rect cropRect = animation.getImageRect();
    SDL_RenderCopyEx(renderTarget, animation.getFrameTexture(), &cropRect, &posRect, 0, NULL, SDL_FLIP_NONE);
}
void TitleState::keyPressed(int k)
{
	switch (k)
    {
        case SDLK_LEFT:
        {
            break;
        }
        case SDLK_RIGHT:
        {
            break;
        }
        case SDLK_UP:
        {
            up = true;
            break;
        }
        case SDLK_DOWN:
        {
            down = true;
            break;
        }
        case SDLK_SPACE:
        {
            break;
        }
        case SDLK_RETURN:
        {
            enter = true;
            break;
        }
        case SDLK_RSHIFT:
        {
            back = true;
            break;
        }
    }
}
void TitleState::keyReleased(int k)
{

}
