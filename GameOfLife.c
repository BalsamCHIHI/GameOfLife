#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600
#define CELL_SIZE 10
#define GRID_WIDTH (WIDTH / CELL_SIZE)
#define GRID_HEIGHT (HEIGHT / CELL_SIZE)

typedef struct {
    int alive;
    SDL_Color color;
} Cell;

void init_grid(Cell grid[GRID_WIDTH][GRID_HEIGHT]) {
    srand(time(NULL));
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            grid[x][y].alive = rand() % 2;
            grid[x][y].color = (SDL_Color){rand() % 256, rand() % 256, rand() % 256, 255};
        }
    }
}

void print_initial_alive_cells(Cell grid[GRID_WIDTH][GRID_HEIGHT]) {
    printf("Initial alive cells:\n");
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            if (grid[x][y].alive) {
                printf("Cell[%d][%d] - Color: (%d, %d, %d)\n", x, y, grid[x][y].color.r, grid[x][y].color.g, grid[x][y].color.b);
            }
        }
    }
}

int count_neighbors(Cell grid[GRID_WIDTH][GRID_HEIGHT], int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            int nx = (x + dx + GRID_WIDTH) % GRID_WIDTH;
            int ny = (y + dy + GRID_HEIGHT) % GRID_HEIGHT;
            count += grid[nx][ny].alive;
        }
    }
    return count;
}

SDL_Color get_dominant_color(Cell grid[GRID_WIDTH][GRID_HEIGHT], int x, int y) {
    SDL_Color colors[8];
    int color_count = 0;

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            int nx = (x + dx + GRID_WIDTH) % GRID_WIDTH;
            int ny = (y + dy + GRID_HEIGHT) % GRID_HEIGHT;
            if (grid[nx][ny].alive) {
                colors[color_count++] = grid[nx][ny].color;
            }
        }
    }

    if (color_count == 0) {
        return (SDL_Color){0, 0, 0, 255};
    }

    int max_count = 0;
    SDL_Color dominant_color = colors[0];
    for (int i = 0; i < color_count; i++) {
        int count = 0;
        for (int j = 0; j < color_count; j++) {
            if (colors[i].r == colors[j].r && colors[i].g == colors[j].g && colors[i].b == colors[j].b) {
                count++;
            }
        }
        if (count > max_count) {
            max_count = count;
            dominant_color = colors[i];
        }
    }

    return dominant_color;
}

void update_grid(Cell grid[GRID_WIDTH][GRID_HEIGHT]) {
    Cell new_grid[GRID_WIDTH][GRID_HEIGHT];
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            int neighbors = count_neighbors(grid, x, y);
            new_grid[x][y] = grid[x][y];
            if (grid[x][y].alive) {
                if (neighbors < 2 || neighbors > 3) {
                    new_grid[x][y].alive = 0;
                }
            } else {
                if (neighbors == 3) {
                    new_grid[x][y].alive = 1;
                    new_grid[x][y].color = get_dominant_color(grid, x, y);
                }
            }
        }
    }
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            grid[x][y] = new_grid[x][y];
        }
    }
}

void draw_grid(SDL_Renderer *renderer, Cell grid[GRID_WIDTH][GRID_HEIGHT]) {
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            if (grid[x][y].alive) {
                SDL_SetRenderDrawColor(renderer, grid[x][y].color.r, grid[x][y].color.g, grid[x][y].color.b, 255);
                SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Cell grid[GRID_WIDTH][GRID_HEIGHT];
    init_grid(grid);
    print_initial_alive_cells(grid);

    int running = 1;
    int delay = 100;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        update_grid(grid);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        draw_grid(renderer, grid);
        SDL_RenderPresent(renderer);

        SDL_Delay(delay);
        if (delay > 10) {
            delay -= 1; // Accelerate the game over time
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
