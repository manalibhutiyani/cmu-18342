/** @file game.c
 *
 * @brief A simple snake game
 *
 * press i for UP
 * press j for LEFT
 * press l for RIGHT
 * press k for DOWN
 *
 * @author Hsueh-Hung Cheng <hsuehhuc@andrew.cmu.edu>
 * @date   2014-11-07
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define NULL ( (void *) 0)
#define INIT_SPEED 200           /* how long refresh once */
#define MAX_SPEED 100
#define SPEED_STEP 10
#define GAME_BOARD_W (1 << 6)
#define GAME_BOARD_H (1 << 4)
#define SNAKE_MAX_LEN GAME_BOARD_W * GAME_BOARD_H
#define START_X 30
#define START_Y 10
#define LEFT    'j'
#define RIGHT   'l'
#define UP      'i'
#define DOWN    'k'
#define SPACE   ' '
#define WALL    '*'
#define SNAKE   'o'
#define FOOD    'a'
#define DEAD_SYM 'X'
#define NEWLINE 0xA
#define DEAD    0
#define ALIVE   1

/*
 * Game structure, which contains all information about this game
 */
typedef struct {
    /* additional space in the second dimension is for newline */
    char board[GAME_BOARD_H][GAME_BOARD_W + 1];
    /* for telling puts when to stop printing */
    char end;
    /* save the position of the snake body */
    char snake[SNAKE_MAX_LEN][2];
    char direction;
    int status;
    /*
     * snake[snake_index] = the positin of head
     * snake[snake_index - length + 1] = the positin of tail
     */
    int snake_length;
    int snake_index;
    /*
     * acting as a random seed
     * since there is no random function, set this variable with
     * the value of OS time
     */
    unsigned seed;
} Game;

Game g_game;

/*
 * put_food - put a food in an available space
 */
void put_food(Game *game) {
    unsigned seed = game->seed;
    int snake_index = game->snake_index;
    int food_pos_x = game->snake[snake_index][0] + seed;
    int food_pos_y = game->snake[snake_index][1] + seed;
    do {
        food_pos_x++;
        food_pos_y++;
        food_pos_x = (food_pos_x) & (GAME_BOARD_W - 1);
        food_pos_y = (food_pos_y) & (GAME_BOARD_H - 1);
        /* avoid puting the food too near the wall */
    } while (game->board[food_pos_y][food_pos_x] != SPACE ||
            food_pos_y <= 3 || food_pos_x <= 3 ||
            food_pos_x >= GAME_BOARD_W - 3 ||
            food_pos_y >= GAME_BOARD_H - 3);
    game->board[food_pos_y][food_pos_x] = FOOD;
}


/*
 * update_game - this function is executed periodically, and
 *      its tasks includ moving the snake and update other
 *      things on the game board
 */
void update_game(void *var) {
    Game *game = (Game *)var;
    int snake_length = game->snake_length;
    int snake_index = game->snake_index;
    short new_x = game->snake[snake_index][0];
    short new_y = game->snake[snake_index][1];

    /* move based on the direction */
    switch (game->direction) {
        case UP:
            new_y--;
            break;
        case DOWN:
            new_y++;
            break;
        case LEFT:
            new_x--;
            break;
        case RIGHT:
            new_x++;
            break;
    }
    /*
     * check if game over
     * two conditions:
     *   1) the snake bumps into the wall
     *   2) the snake touches its body
     */
    if (game->board[new_y][new_x] == SNAKE ||
            game->board[new_y][new_x] == WALL) {
        game->status = DEAD;
    } else {
        /* update the snake */
        if (++snake_index >= SNAKE_MAX_LEN) {
            snake_index = 0;
        }
        game->snake[snake_index][0] = new_x;
        game->snake[snake_index][1] = new_y;

        if (game->board[new_y][new_x] == FOOD) {
            game->snake_length += 1;
            put_food(game);
        } else {
            /* move the tail */
            int tail_index = snake_index - snake_length;
            if (tail_index < 0) {
                tail_index += SNAKE_MAX_LEN;
            }
            short tail_x = game->snake[tail_index][0];
            short tail_y = game->snake[tail_index][1];
            game->board[tail_y][tail_x] = SPACE;
        }

        /* move the head */
        game->board[new_y][new_x] = SNAKE;
        game->snake_index = snake_index;
    }
    return;
}

/*
 * init_game - this function is called once before the game starts
 *      in order to initialize all members in Game structure
 */
void init_game(Game *game) {
    int i, j;
    // initialize the border and inside
    for (i = 0; i < GAME_BOARD_H; i++) {
        for (j = 0; j <= GAME_BOARD_W; j++) {
            if (j == GAME_BOARD_W) {
                game->board[i][j] = NEWLINE;
            } else if (i == 0 || i == GAME_BOARD_H - 1 ||
                    j == 0 || j == GAME_BOARD_W - 1) {
                game->board[i][j] = WALL;
            } else {
                game->board[i][j] = SPACE;
            }
        }
    }
    game->status = ALIVE;
    game->end = '\0';
    game->direction = RIGHT;

    // initialize the position of the snake
    game->snake_index = 0;
    game->snake_length = 1;
    game->snake[0][0] = START_X;
    game->snake[0][1] = START_Y;
    game->board[START_Y][START_X] = SNAKE;

    put_food(game);
}


int main(int argc, char** argv)
{
    char command;
    char direction;
    int speed = INIT_SPEED;
    init_game(&g_game);
    /* call update_game every speed mseconds */
    period(speed, update_game, &g_game);

    printf("GAME START...\n");

    while (g_game.status != DEAD) {
        g_game.seed = time();
        read(STDIN_FILENO, &command, 1);
        printf("\b \b");
        /*
         * check if command is valid
         * for example, if the snake is heading left,
         * command right is invalid
         */
        direction = g_game.direction;
        if (direction == DOWN && command == UP) {
            continue;
        } else if (direction == UP && command == DOWN) {
            continue;
        } else if (direction == RIGHT && command == LEFT) {
            continue;
        } else if (direction == LEFT && command == RIGHT) {
            continue;
        } else if (command != LEFT && command != RIGHT &&
                command != UP && command != DOWN) {
            continue;
        }

        /* update the direction */
        g_game.direction = command;

        /*
         * speed up
         * when eating one food, the period of exectuing update_game
         * is decreased. smaller value of speed means the snake moves
         * faster
         */
        int new_speed = INIT_SPEED - (SPEED_STEP * g_game.snake_length);
        if (speed != new_speed && speed > MAX_SPEED) {
            speed = new_speed;
            period(speed, my_func, &g_game);
        }
    }
    /*
     * uninstall the function
     */
    period(-1, NULL, &g_game);
    printf("DEAD\n");
	return 0;
}
