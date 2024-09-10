// Soemthing make the game_over turn true, I need to figure it out //


#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GRID_SIZE 20 // In Pixels
#define SNAKE_INITIAL_LENGTH 4 // In Points Squares)
#define MAX_SNAKE_LENGTH (SCREEN_WIDTH / GRID_SIZE) * (SCREEN_HEIGHT / GRID_SIZE) // Snake to take all of the screen (You win here)


typedef struct {
	int x;
	int y;
} Square; 

typedef struct {
	Square* body;
	int length;
	int direction; /* 0 : up , 1: right , 2: down , 3: left */
} Snake;


// Create SDL Window + Renderer 
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

Snake snake;
Square food;
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
		printf("%d", game_over);
		SDL_Delay(100);
	}
	
	cleanup();
	return  0;
}

bool init_game()
{
	srand(time(NULL));

	snake.body = malloc(sizeof(Square) * MAX_SNAKE_LENGTH);
	if (snake.body == NULL)
	{
		printf("Failed to allocate memory for snake body\n");
		return false;
	}

	snake.length = SNAKE_INITIAL_LENGTH;
	snake.direction = 1;

	for (int i = 0; i < snake.length; i++)
	{
		snake.body[i].x = (SCREEN_WIDTH / 2) - (i * GRID_SIZE);
		snake.body[i].y = SCREEN_HEIGHT / 2;
	}
	
	// Don't get this down part but it's not important now
	food.x = (rand() % (SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE;
	food.y = (rand() % (SCREEN_HEIGHT / GRID_SIZE)) * GRID_SIZE;

	return true;


}

void cleanup()
{
	if (snake.body != NULL)
	{
		free(snake.body);
		snake.body = NULL;
	}
	
	if (renderer != NULL)
	{
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}

	if (window != NULL)
	{
		SDL_DestroyWindow(window);
		window = NULL;
	}

	SDL_Quit();
}

void handle_events()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			game_over = true;
		} else if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
				case SDLK_UP:
					if (snake.direction != 2) snake.direction = 0;
					break;
				case SDLK_RIGHT:
					if (snake.direction != 3) snake.direction = 1;
					break;
				case SDLK_DOWN:
					if (snake.direction != 0) snake.direction = 2;
					break;
				case SDLK_LEFT:
					if (snake.direction != 1) snake.direction = 3;
					break;
			}
		}



	}

}



void update_game()
{
	Square new_head = snake.body[0];

	switch (snake.direction) {
		case 0: new_head.y -= GRID_SIZE; break;
		case 1: new_head.x += GRID_SIZE; break;
		case 2: new_head.y += GRID_SIZE; break;
		case 3: new_head.x -= GRID_SIZE; break;
	}

	if (new_head.x < 0 || new_head.x >= SCREEN_WIDTH || new_head.y < 0 || new_head.y >= SCREEN_HEIGHT)
	{
		printf("Game Over");
		game_over= true;
		return;
	}

	for(int i = 1; i < snake.length ; i++)
	{
		if (new_head.x == snake.body[i].x && new_head.y == snake.body[i].y)
		{
			printf("Collision with the snake");
			game_over = true;
			return;
		}
	}

	if (new_head.x == food.x && new_head.y == food.y)
	{
		if (snake.length < MAX_SNAKE_LENGTH)
		{
			Square* new_body = realloc(snake.body, sizeof(Square) * (snake.length + 1));
			if (new_body == NULL)
			{
				printf("Failed to generate new body \n");
				game_over = true;
				return;
			}
			snake.body = new_body;
			snake.length++;
		}
		else {
			game_over = true;
			return;
		}
		food.x = (rand() % (SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE;
		food.y = (rand() % (SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE;
	} else { // Move the snake forward
		for (int i = snake.length - 1; i > 0 ; i--)
		{
			snake.body[i] = snake.body[i-1];
		}
	}

	snake.body[0] = new_head;

}


void render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	for (int i = 0; i < snake.length; i++)
	{
		SDL_Rect rect = {snake.body[i].x, snake.body[i].y, GRID_SIZE, GRID_SIZE};
		SDL_RenderFillRect(renderer, &rect);
	}

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_Rect food_rect = {food.x, food.y, GRID_SIZE, GRID_SIZE};
	SDL_RenderFillRect(renderer, &food_rect);

	SDL_RenderPresent(renderer);
}

































