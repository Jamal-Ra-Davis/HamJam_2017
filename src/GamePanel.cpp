#include "../headers/GamePanel.h"
#include "../headers/SDL_Helpers.h"
#include "../headers/AudioPlayer.h"
#include <stdio.h>

bool *GamePanel::isRunningControl;
int GamePanel::WIDTH;
int GamePanel::HEIGHT;

GamePanel::GamePanel()
{
	window_name = "Window Name";
	isRunning = false;
	FPS = 60;
	//frameTime = 0;
	prevTime = 0;
	currTime = 0;
	deltaTime = 0;
	window = NULL;
	renderTarget = NULL;
	gsm = NULL;
	display_texture = NULL;
	GamePanel::isRunningControl = getRunningPointer();
}
GamePanel::~GamePanel()
{
	if (window)
		SDL_DestroyWindow(window);
	if (renderTarget)
		SDL_DestroyRenderer(renderTarget);
	if (gsm)
		delete gsm;
	if (display_texture)
		SDL_DestroyTexture(display_texture);

	display_texture = NULL;
	gsm = NULL;
	window = NULL;
	renderTarget = NULL;

	SDL_Quit();
}
int GamePanel::init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Video init error: %s\n", SDL_GetError());
        return 1;
    }
	
	//window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED,
    //                            SDL_WINDOWPOS_CENTERED, (int)(WIDTH*3), (int)(HEIGHT*3), SDL_WINDOW_SHOWN /*| SDL_WINDOW_FULLSCREEN_DESKTOP*/);
	window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (window == NULL)
    {
        printf("Window creation error: %s\n", SDL_GetError());
        return 1;
    }

	SDL_GetWindowSize(window, &WIDTH, &HEIGHT);


	renderTarget = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderTarget == NULL)
	{
		printf("Failed to init render target. Error: %s\n", SDL_GetError());
		return 1;
	}
	if (AudioPlayer::initAudioSystem())
    {
        return 1;
    }
	gsm = new GameStateManager(renderTarget);
	if (gsm == NULL)
	{
		printf("Failed to create game state manager\n");
		return 1;
	}
	
	//SDL_SetRenderDrawColor(renderTarget, 0xFF, 0, 0, 0xFF);
	//SDL_SetRenderDrawColor(renderTarget, 0, 0, 0, 0xFF);
	int r=77;//27;
	int g=52;//147;
	int b=202;//169;
	SDL_SetRenderDrawColor(renderTarget, r, g, b, 0xFF);
	display_texture = SDL_CreateTexture(renderTarget, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
	return 0;	
}
int GamePanel::run()
{
	int init_check = init();
	if (init_check)
		return init_check;

	isRunning = true;
	while(isRunning)
	{
		
		prevTime = currTime;
        currTime = SDL_GetTicks();
        deltaTime = (currTime - prevTime)/1000.0f;
		
		handleInputs();
		update();
		draw();
		drawToScreen();
		while (SDL_GetTicks() - currTime < 16);
			
	}
	close();
	return 0;
}
void GamePanel::handleInputs()
{
	while (SDL_PollEvent(&event) != 0)
	{
		//PrintEvent(&event);
		if (event.type == SDL_QUIT)
		{
			isRunning = false;
		}
		else if (event.type == SDL_KEYDOWN)
		{
			int keyCode = event.key.keysym.sym;
			gsm->keyPressed(keyCode);
		}
		else if (event.type == SDL_KEYUP)
		{
			int keyCode = event.key.keysym.sym;
			gsm->keyReleased(keyCode);
		}
		else if (event.type == SDL_TEXTINPUT)
		{
			char *text = event.text.text;
		}
	}
	keyState = SDL_GetKeyboardState(NULL);
}
void GamePanel::update()
{
	gsm->update();	
}
void GamePanel::draw()
{
	SDL_SetRenderTarget(renderTarget, display_texture);

	SDL_RenderClear(renderTarget);//--
	
	//Draw all game objects/graphics
	gsm->draw();//--

	SDL_SetRenderTarget(renderTarget, NULL);

	SDL_Rect scale = {0, 0, WIDTH, HEIGHT};
	//printf("Height = %d, Width = %d\n", WIDTH, HEIGHT);
	
	SDL_RenderCopy(renderTarget, display_texture, NULL, NULL);//&scale);
}
void GamePanel::drawToScreen()
{
	SDL_RenderPresent(renderTarget);
}
void GamePanel::close()
{
	AudioPlayer::closeAudioSystem();
	SDL_Quit();
}
