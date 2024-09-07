#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GRID_SIZE 20
#define SNAKE_INITIAL_LENGTH 4 
#define MAX_SNAKE_LENGTH (SCREEN_WIDTH / GRID_SIZE) * (SCREEN_HEIGHT / GRID_SIZE)


typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	Point* body;
	int length;
	int direction; /* 0 : up , 1: right , 2: down , 3: left */
} Snake;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
Snake snake;
Point food;
bool game_over = false;


bool init_game(void);
void cleanup(void);
void handle_events(void);
void update_game(void);
void render(void);


int main()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL Could not initialize! SDL_Error : %s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("Snake Game" , SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		printf("Renderer could not be created! SDL_ERROR: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	if(!init_game())
	{
		cleanup();
		return 1;
	}

	while (!game_over)
	{
		handle_events();
		update_game();
		render();
		SDL_Delay(100);
	}
	
	cleanup();
	return  0;
}

bool init_game()
{
	srand(time(NULL));

	snake.body = malloc(sizeof(Point) * MAX_SNAKE_LENGTH);
	if (snake.body == NULL)
	{
		printf("Failed to allocate memory for snake body\n");
		return false;
	}

	snake.length = SNAKE_INITIAL_LENGTH;
	snake.direction = 1;

	for (int i = 0; i < snake.length; i++)
	{
		snake.body[i].x = (SCREEN_WIDTH / 2) - i * GRID_SIZE;
		snake.body[i].y = SCREEN_HEIGHT / 2;
	}
	
	food.x = (rand() % (SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE;
	food.y = (rand() % (CREEN_HEIGHT / GRID_SIZE)) * GRID_SIZE;

	return true;


}


























