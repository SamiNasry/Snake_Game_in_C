// Seems good for now, I need to add score track and special fruits + maange the file better it's getting long and messy // 


#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GRID_SIZE 20 // In Pixels
#define SNAKE_INITIAL_LENGTH 4 // In Points Squares)
#define MAX_SNAKE_LENGTH (SCREEN_WIDTH / GRID_SIZE) * (SCREEN_HEIGHT / GRID_SIZE) // Snake to take all of the screen (You win here)
#define MAX_QUEUE_SIZE 16

typedef struct {
	int x;
	int y;
} Square; 

typedef struct {
	Square* body;
	int length;
	int direction; /* 0 : up , 1: right , 2: down , 3: left */
} Snake;

typedef struct {
	int directions[MAX_QUEUE_SIZE];
	int front;
	int rear;
	int size;
} InputQueue;


// Create SDL Window + Renderer 
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

Snake snake;
Square food;
InputQueue inputQueue;
bool game_over = false;


bool init_game(void);
void cleanup(void);
void handle_events(void);
void update_game(void);
void render(void);
void generate_food(void);
void init_queue(InputQueue* queue);
bool is_queue_empty(InputQueue* queue);
bool is_queue_full(InputQueue* queue);
void enqueue(InputQueue* queue, int direction);
int dequeue(InputQueue* queue);


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
	// Game Loop
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


void init_queue(InputQueue* queue)
{
	queue->front = 0;
	queue->rear = -1;
	queue->size = 0;
}


bool is_queue_empty(InputQueue* queue)
{
	return queue->size == 0;
}

bool is_queue_full(InputQueue* queue)
{
	return queue->size == MAX_QUEUE_SIZE;
}

void enqueue(InputQueue* queue, int direction)
{
	if (is_queue_full(queue)) return;

	queue->rear = (queue->rear +1) % MAX_QUEUE_SIZE;
	queue->directions[queue->rear] = direction;
	queue->size++;
}

int dequeue(InputQueue* queue)
{
	if (is_queue_empty(queue)) return -1;

	int direction = queue->directions[queue->front];
	queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
	queue->size--;
	return direction;
}


// Game Initialization
bool init_game()
{
	

	snake.body = malloc(sizeof(Square) * MAX_SNAKE_LENGTH);
	if (snake.body == NULL)
	{
		printf("Failed to allocate memory for snake body\n");
		return false;
	}

	snake.length = SNAKE_INITIAL_LENGTH; // Snake Initial Length
	snake.direction = 1; // Sanke start going to the right 

	for (int i = 0; i < snake.length; i++)
	{
		snake.body[i].x = (SCREEN_WIDTH / 2) - (i * GRID_SIZE);
		snake.body[i].y = SCREEN_HEIGHT / 2;
	}
	
	// Generate First Food
	generate_food();

	init_queue(&inputQueue);

	return true;


}

void generate_food()
{
	srand(time(NULL));
	bool valid_position = false;
	while(!valid_position)
	{
		food.x = (rand() % (SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE;
		food.y = (rand() % (SCREEN_HEIGHT / GRID_SIZE)) * GRID_SIZE;
		
		valid_position = true;

		if (food.x >= SCREEN_WIDTH || food.y >= SCREEN_HEIGHT)
			valid_position = false;
		
		for (int i = 0; i < snake.length ; i++)
		{
			if (food.x == snake.body[i].x && food.y == snake.body[i].y )
			{
				valid_position = false;
				break;
			}
		}
	}
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
			int new_direction = -1;
			switch (event.key.keysym.sym)
			{
				case SDLK_UP:
					if (snake.direction != 2) new_direction = 0;
					break;
				case SDLK_RIGHT:
					if (snake.direction != 3) new_direction = 1;
					break;
				case SDLK_DOWN:
					if (snake.direction != 0) new_direction = 2;
					break;
				case SDLK_LEFT:
					if (snake.direction != 1) new_direction = 3;
					break;
			}

			if (new_direction != -1 && !is_queue_full(&inputQueue)) {
				enqueue(&inputQueue, new_direction);
			}
		}
	}
}

void update_game()
{
	static Uint32 last_update_time = 0;
	Uint32 current_time = SDL_GetTicks();
	if (current_time - last_update_time < 50)
	{
		return;
	}

	last_update_time = current_time;


	if(!is_queue_empty(&inputQueue))
	{
		int new_direction = dequeue(&inputQueue);

		if ((new_direction + 2) % 4 != snake.direction)
		{
			snake.direction = new_direction;
		}
	}

	Square new_head = snake.body[0];

	switch (snake.direction) {
		case 0: new_head.y -= GRID_SIZE; break;
		case 1: new_head.x += GRID_SIZE; break;
		case 2: new_head.y += GRID_SIZE; break;
		case 3: new_head.x -= GRID_SIZE; break;
	}

	if (new_head.x < 0 || new_head.x >= SCREEN_WIDTH || new_head.y < 0 || new_head.y >= SCREEN_HEIGHT)
	{
		printf("Game Over, Out of bound. x : %d , y : %d" , new_head.x , new_head.y);
		game_over= true;
		return;
	}

	for(int i = 1; i < snake.length ; i++)
	{
		if (new_head.x == snake.body[i].x && new_head.y == snake.body[i].y)
		{
			printf("Collision with itself. x : %d , y : %d" , new_head.x, new_head.y);
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
		
		generate_food();
		
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

































