#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#define FPS 24
#define WIDTH 1200
#define HEIGHT 800
#define GRID_SIZE 20
#define COLS (WIDTH  / GRID_SIZE)
#define ROWS (HEIGHT / GRID_SIZE)

void draw_grid(int width, int height, int grid_size, int line_width) {
    for (int x = 0; x < width / grid_size; x++) {
        for (int y = 0; y < height / grid_size; y++) {
            int posX = x * grid_size;
            int posY = y * grid_size;
            DrawRectangle(posX+line_width, posY+line_width, grid_size-line_width, grid_size-line_width, BLACK);
        }
    }
}

void draw_hud(const int generation) {
    char buffer[50];
    sprintf(buffer, "Generation: %d", generation);

    DrawText(buffer, 10, 10, 28, SKYBLUE);

}

typedef struct {
    int generation;
    int** cells;
    int** neighbors;
} Game;

void draw_life(const Game* game) {
    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            if (game->cells[x][y])
                DrawRectangle(x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE, GREEN);
        }
    }
}

void calculate_neighbors(Game* game) {
    int** cells = game->cells;
    int** neighbors = game->neighbors;

    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            int neighbor_count = 0;

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    int x_wrapped = ((x + dx) % COLS + COLS) % COLS;
                    int y_wrapped = ((y + dy) % ROWS + ROWS) % ROWS;

                    neighbor_count += cells[x_wrapped][y_wrapped];
                }
            }
            neighbors[x][y] = neighbor_count;
        }
    }
}

void advance_gen(Game* game) {
    game->generation++;
    if (game->generation < 0) {
        return;
    }

    calculate_neighbors(game);

    int** cells = game->cells;
    int** neighbors = game->neighbors;


    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            bool alive = cells[x][y] == 1;
            int neighbor_count = neighbors[x][y];

            // Rule 1: "Underpopulation"
            if (alive && neighbor_count < 2) {
                cells[x][y] = 0;
            }

            // Rule 3: "Overpopulation"
            if (alive && neighbor_count > 3) {
                cells[x][y] = 0;
            }

            // Rule 4: "Reproduction"
            if (!alive && neighbor_count == 3) {
                cells[x][y] = 1;
            }
        }
    }
}

Game* init_game() {
    Game* ptr = (Game*)malloc(sizeof(*ptr));
    if (ptr == NULL) {
        return NULL;
    }

    ptr->generation = -3;

    int** cells = (int**)malloc(COLS * sizeof(int*));
    if (cells == NULL) {
        return NULL;
    }

    for (int i = 0; i < COLS; i++) {
        cells[i] = (int*)malloc(ROWS * sizeof(int));
        if (cells[i] == NULL) return NULL;
    }

    ptr->cells = cells;

    int** neighbors = (int**)malloc(COLS * sizeof(int*));
    if (neighbors == NULL) {
        return NULL;
    }

    for (int i = 0; i < COLS; i++) {
        neighbors[i] = (int*)malloc(ROWS * sizeof(int));
        if (neighbors[i] == NULL) return NULL;
    }

    ptr->neighbors = neighbors;

    
    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            cells[x][y] = 0;
            neighbors[x][y] = 0;
        }
    }



    // Block
    ptr->cells[12][17] = 1;
    ptr->cells[13][17] = 1;
    ptr->cells[12][16] = 1;

    // Blinker
    ptr->cells[2][10] = 1;
    ptr->cells[3][10] = 1;
    ptr->cells[4][10] = 1;

    // Glider
    ptr->cells[22][10] = 1;
    ptr->cells[23][10] = 1;
    ptr->cells[24][10] = 1;
    ptr->cells[24][9] = 1;
    ptr->cells[23][8] = 1;

    return ptr;
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Game of Life");
    SetTargetFPS(FPS);
    
    Game* game = init_game();
    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            draw_grid(WIDTH, HEIGHT, GRID_SIZE, 0);
            draw_hud(game->generation);
            draw_life(game);
        }
        EndDrawing();

        advance_gen(game);
    }

    CloseWindow();

    return 0;
}
