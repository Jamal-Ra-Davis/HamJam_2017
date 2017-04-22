OBJS = 	./src/HamJam_2017.o ./src/SDL_Helpers.o ./src/GamePanel.o ./src/GameStateManager.o ./src/GameState.o ./src/Background.o\
		./src/TileMap.o ./src/Tile.o ./src/MapObject.o ./src/Animation.o\
		./src/AudioPlayer.o ./src/BgObject.o\
		./src/Numbers.o\
		./src/InputBuffer.o ./src/Letters.o ./src/Pig_Player.o ./src/Cake.o

CC = g++
DEBUG = -g
#CFLAGS = -Wall -c $(DEBUG)
CFLAGS = -Wall $(DEBUG)
#LFLAGS = -Wall $(DEBUG)
LFLAGS = -lSDL2 -lSDL2_mixer
DS = ./src/
DH = ./headers/

#g++ SpriteAnimation.cpp -w -lSDL2 -o SpriteAnimation


HamJam_2017: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o HamJam_2017

#enemy.o: $(DS)enemy.cpp $(DH)enemy.h $(DH)skill.h $(DH)FileFunctions.h
#	$(CC) $(CFLAGS) $(DS)enemy.cpp -o $(DS)enemy.o

#player.o: $(DS)player.cpp $(DH)player.h $(DH)enemy.h $(DH)skill.h $(DH)item.h\
			 $(DH)Safe_Input.h
#	$(CC) $(CFLAGS) $(DS)player.cpp -o $(DS)player.o

SDL_Helpers.o: $(DS)SDL_Helpers.cpp $(DH)SDL_Helpers.h
	$(CC) -c $(CFLAGS) $(DS)SDL_Helpers.cpp -o $(DS)SDL_Helpers.o

GamePanel.o: $(DS)GamePanel.cpp $(DH)GamePanel.h $(DH)AudioPlayer.h
	$(CC) -c $(CFLAGS) $(DS)GamePanel.cpp -o $(DS)GamePanel.o

GameStateManager.o: $(DS)GameStateManager.cpp $(DH)GameStateManager.h $(DH)GameState.h
	$(CC) -c $(CFLAGS) $(DS)GameStateManager.cpp -o $(DS)GameStateManager.o

Background.o: $(DS)Background.cpp $(DH)Background.h $(DH)GamePanel.h $(DH)SDL_Helpers.h
	$(CC) -c $(CFLAGS) $(DS)Background.cpp -o $(DS)Background.o

GameState.o: $(DS)GameState.cpp $(DH)GameStateManager.h $(DH)GameState.h $(DH)Background.h\
			 $(DH)TileMap.h $(DH)SDL_Helpers.h $(DH)Numbers.h\
			 $(DH)AudioPlayer.h $(DH)BgObject.h $(DH)Animation.h\
			 $(DH)Letters.h $(DH)Pig_Player.h
	$(CC) -c $(CFLAGS) $(DS)GameState.cpp -o $(DS)GameState.o

Tile.o: $(DS)Tile.cpp $(DH)Tile.h
	$(CC) -c $(CFLAGS) $(DS)Tile.cpp -o $(DS)Tile.o

TileMap.o: $(DS)TileMap.cpp $(DH)TileMap.h $(DH)Tile.h $(DH)GamePanel.h $(DH)SDL_Helpers.h
	$(CC) -c $(CFLAGS) $(DS)TileMap.cpp -o $(DS)TileMap.o

MapObject.o: $(DS)MapObject.cpp $(DH)MapObject.h $(DH)TileMap.h $(DH)Tile.h $(DH)GamePanel.h $(DH)SDL_Helpers.h\
			$(DH)Animation.h
	$(CC) -c $(CFLAGS) $(DS)MapObject.cpp -o $(DS)MapObject.o

Animation.o: $(DS)Animation.cpp $(DH)Animation.h $(DH)SDL_Helpers.h
	$(CC) -c $(CFLAGS) $(DS)Animation.cpp -o $(DS)Animation.o

AudioPlayer.o: $(DS)AudioPlayer.cpp $(DH)AudioPlayer.h
	$(CC) -c $(CFLAGS) $(DS)AudioPlayer.cpp -o $(DS)AudioPlayer.o

BgObject.o: $(DS)BgObject.cpp $(DH)BgObject.h $(DH)MapObject.h $(DH)SDL_Helpers.h
	$(CC) -c $(CFLAGS) $(DS)BgObject.cpp -o $(DS)BgObject.o

Pig_Player.o: $(DS)Pig_Player.cpp $(DH)Pig_Player.h $(DH)MapObject.h $(DH)SDL_Helpers.h\
          $(DH)AudioPlayer.h
	$(CC) -c $(CFLAGS) $(DS)Pig_Player.cpp -o $(DS)Pig_Player.o

Cake.o: $(DS)Cake.cpp $(DH)Cake.h $(DH)MapObject.h $(DH)SDL_Helpers.h $(DH)AudioPlayer.h
	$(CC) -c $(CFLAGS) $(DS)Cake.cpp -o $(DS)Cake.o

Numbers.o: $(DS)Numbers.cpp $(DH)Numbers.h $(DH)SDL_Helpers.h
	$(CC) -c $(CFLAGS) $(DS)Numbers.cpp -o $(DS)Numbers.o

InputBuffer.o: $(DS)InputBuffer.cpp $(DH)InputBuffer.h
	$(CC) -c $(CFLAGS) $(DS)InputBuffer.cpp -o $(DS)InputBuffer.o

Letters.o: $(DS)Letters.cpp $(DH)Letters.h $(DH)SDL_Helpers.h
	$(CC) -c $(CFLAGS) $(DS)Letters.cpp -o $(DS)Letters.o

HamJam_2017.o: $(DS)HamJam_2017.cpp $(DH)SDL_Helpers.h $(DH)GamePanel.h
	$(CC) -c $(CFLAGS) $(DS)HamJam_2017.cpp -o $(DS)HamJam_2017.o


clean:
	rm $(OBJS)

