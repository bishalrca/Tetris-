#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 300
#define SCREEN_HEIGHT 600
#define BLOCK_SIZE 30
#define ROWS (SCREEN_HEIGHT / BLOCK_SIZE)
#define COLS (SCREEN_WIDTH / BLOCK_SIZE)

// Colors
typedef struct {
    Uint8 r, g, b;
} Color;

Color colors[] = {
    {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0},
    {255, 165, 0}, {128, 0, 128}, {0, 255, 255}
};

// Tetromino shapes
int tetrominoes[7][4][4] = {
    // O Shape
    {{1, 1, 0, 0},
     {1, 1, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    // I Shape
    {{0, 1, 0, 0},
     {0, 1, 0, 0},
     {0, 1, 0, 0},
     {0, 1, 0, 0}},

    // S Shape
    {{0, 1, 1, 0},
     {1, 1, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    // Z Shape
    {{1, 1, 0, 0},
     {0, 1, 1, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    // L Shape
    {{0, 1, 0, 0},
     {0, 1,0, 0},
     {0, 1, 1, 0},
     {0, 0, 0, 0}},

    // J Shape
    {{0, 0, 1, 0},
     {0, 0, 1, 0},
     {0, 1, 1, 0},
     {0, 0, 0, 0}},

    // T Shape
    {{0, 1, 0, 0},
     {1, 1, 1, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}}
};

// Game variables
int board[ROWS][COLS] = {0};
int currentPiece, posX = COLS / 2 - 1, posY = 0;
int gameOver = 0;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

// Function to draw a block
void drawBlock(int x, int y, Color color) {
    SDL_Rect block = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &block);
}

// Function to check collision
int checkCollision(int x, int y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetrominoes[currentPiece][i][j] && (y + i >= ROWS || x + j < 0 || x + j >= COLS || board[y + i][x + j])) {
                return 1;
            }
        }
    }
    return 0;
}

// Place piece on the board
void placePiece() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetrominoes[currentPiece][i][j]) {
                board[posY + i][posX + j] = currentPiece + 1;
            }
        }
    }
    posX = COLS / 2 - 1;
    posY = 0;
    currentPiece = rand() % 7;
    if (checkCollision(posX, posY)) {
        gameOver = 1;
    }
}

// Function to render the game
void renderGame() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw board
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (board[i][j]) {
                drawBlock(j, i, colors[board[i][j] - 1]);
            }
        }
    }

    // Draw current piece
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetrominoes[currentPiece][i][j]) {
                drawBlock(posX + j, posY + i, colors[currentPiece]);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

// Add this new function to rotate a tetromino
void rotatePiece() {
    // Store the original piece
    int temp[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[i][j] = tetrominoes[currentPiece][i][j];
        }
    }

    // Rotate the piece 90 degrees clockwise
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            tetrominoes[currentPiece][i][j] = temp[3 - j][i];
        }
    }

    // Check if rotation causes collision
    if (checkCollision(posX, posY)) {
        // If it does, revert to original orientation
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                tetrominoes[currentPiece][i][j] = temp[i][j];
            }
        }
    }
}

// Modify the gameLoop function to handle rotation
void gameLoop() {
    SDL_Event event;
    int running = 1, speed = 200, lastMove = SDL_GetTicks();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (!checkCollision(posX - 1, posY)) {
                            posX--;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (!checkCollision(posX + 1, posY)) {
                            posX++;
                        }
                        break;
                    case SDLK_DOWN:
                        if (!checkCollision(posX, posY + 1)) {
                            posY++;
                        }
                        break;
                    case SDLK_UP:    // Add rotation handling
                        rotatePiece();
                        break;
                }
            }
        }

        if (SDL_GetTicks() - lastMove > speed) {
            if (!checkCollision(posX, posY + 1)) {
                posY++;
            } else {
                placePiece();
            }
            lastMove = SDL_GetTicks();
        }

        renderGame();

        if (gameOver) {
            printf("Game Over!\n");
            SDL_Delay(2000);
            running = 0;
        }

        SDL_Delay(50);
    }
}

int main() {
    srand(time(NULL));
    currentPiece = rand() % 7;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Tetris in C with SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        printf("Failed to initialize SDL2\n");
        return 1;
    }

    gameLoop();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
