// game.h
#ifndef GAME_H
#define GAME_H

#define BOARD_SIZE_X 6
#define BOARD_SIZE_Y 6

// Initialize the board
void initialize_board(void);

// Place a ship on the board
int place_ship(int orientation);
int ascii_to_coordinates(int output1, int output2, int *x, int *y);
// Print the board for debugging purposes
void print_board(void);
void initialize_board_enemy(void);
int check_winner(char board[BOARD_SIZE_X][BOARD_SIZE_Y]);
void set_enemy_board(void);
void check_hit(char board[BOARD_SIZE_X][BOARD_SIZE_Y], int update_queues);
void print_enemy_board(void);
extern int test_x, test_y;
extern int orientation;
extern char user_board[BOARD_SIZE_X][BOARD_SIZE_Y];
extern char enemy_board[BOARD_SIZE_X][BOARD_SIZE_Y];

extern int y,x;
#endif // GAME_H
