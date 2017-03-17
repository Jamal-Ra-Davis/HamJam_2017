#include <stdio.h>
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


	cake_timer = getMs();

	spawnCake();

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
		gsm->setState(GameStateManager::GAMEPLAY_STATE);
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

