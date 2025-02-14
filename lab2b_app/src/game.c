// game.c
#include "game.h"
#include <stdio.h>
#include <ctype.h>
#include <time.h>   // For seeding srand()
#define EMPTY '.' // Symbol for an empty cell
#define SHIP 'X'  // Symbol for a ship cell

char user_board[BOARD_SIZE_X][BOARD_SIZE_Y];
char enemy_board[BOARD_SIZE_X][BOARD_SIZE_Y];

// List to track targeted cells around a hit
int immediate_queue[100][2];
int immediate_queue_size = 0;
int secondary_queue[100][2];
int secondary_queue_size = 0;




// Initialize the board
void initialize_board(void) {
    for (int i = 0; i < BOARD_SIZE_X; i++) {
        for (int j = 0; j < BOARD_SIZE_Y; j++) {
            user_board[i][j] = EMPTY;
        }
    }
}

void initialize_board_enemy(void) {
    for (int i = 0; i < BOARD_SIZE_X; i++) {
        for (int j = 0; j < BOARD_SIZE_Y; j++) {
            enemy_board[i][j] = EMPTY;
        }
    }
}

// Helper function to convert y-coordinate from char to index
int y_to_index(char y) {
    y = toupper(y);
    if (y >= 'A' && y <= 'F') {
        return y - 'A';
    }
    return -1; // Invalid input
}

int test = 0;
int test_x = 0; // Global variable for x-coordinate
int test_y = 0; // Global variable for y-coordinate

// Helper function to convert ASCII output to coordinates
int ascii_to_coordinates(int output1, int output2, int *x, int *y) {
    int temp_x = y_to_index((char)output2); // Convert x-coordinate (letter)
    int temp_y = output1 - '0';            // Convert y-coordinate (number)

    if (temp_x == -1 || temp_y < 0 || temp_y >= BOARD_SIZE_X) {
        xil_printf("Invalid coordinates: output1 = %c, output2 = %c\n", (char)output1, output2);
        return -1; // Failure
    }

    *x = temp_x; // Update x-coordinate via pointer
    *y = temp_y; // Update y-coordinate via pointer

    // Store and print test values
    test_x = *x;
    test_y = *y;

    return 0; // Success
}

// Function to check if a coordinate on the enemy board hits a ship
void check_hit(char board[BOARD_SIZE_X][BOARD_SIZE_Y], int update_queues) {
    // Perform bounds check
    xil_printf("Attempting to fire at %d, %d...\r\n", test_x, test_y);
    if (test_x < 0 || test_x >= BOARD_SIZE_Y || test_y < 1 || test_y >= BOARD_SIZE_X) {
        xil_printf("Error: Coordinates out of bounds (X = %d, Y = %d).\r\n", test_x, test_y);
        return;
    }

    // Check the board cell
    if (board[test_y][test_x] == SHIP) {
        board[test_y][test_x] = 'H'; // Mark as hit
        xil_printf("Hit! A ship was hit at coordinates (X = %d, Y = %d).\r\n", test_x, test_y);

        // Update target queues only if update_queues == 1
        if (update_queues == 1) {
            add_target_queues(test_x, test_y);
        }
    } else if (board[test_y][test_x] == EMPTY) {
        board[test_y][test_x] = 'M'; // Mark as miss
        xil_printf("Miss! No ship at coordinates (X = %d, Y = %d).\r\n", test_x, test_y);
    } else {
        xil_printf("Error: Cell already targeted (X = %d, Y = %d).\r\n", test_x, test_y);
    }
}
void add_target_queues(int x, int y) {
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (int i = 0; i < 4; i++) {
        int nx = x + directions[i][0];
        int ny = y + directions[i][1];

        // Add immediate neighbor
        if (nx >= 0 && nx < BOARD_SIZE_Y && ny >= 1 && ny < BOARD_SIZE_X &&
            user_board[ny][nx] != 'H' && user_board[ny][nx] != 'M') {
            immediate_queue[immediate_queue_size][0] = nx;
            immediate_queue[immediate_queue_size][1] = ny;
            immediate_queue_size++;
            xil_printf("Added to Immediate Queue: (%d, %d)\r\n", nx, ny);
        }

        // Add secondary neighbor
        int nnx = x + 2 * directions[i][0];
        int nny = y + 2 * directions[i][1];
        if (nnx >= 0 && nnx < BOARD_SIZE_Y && nny >= 1 && nny < BOARD_SIZE_X &&
            user_board[nny][nnx] != 'H' && user_board[nny][nnx] != 'M') {
            secondary_queue[secondary_queue_size][0] = nnx;
            secondary_queue[secondary_queue_size][1] = nny;
            secondary_queue_size++;
            xil_printf("Added to Secondary Queue: (%d, %d)\r\n", nnx, nny);
        }
    }
}

// Place a ship on the board
int place_ship(int orientation) {
    xil_printf("Confirm in place_ship()!\r\n");

    // Determine ship placement based on orientation
    switch (orientation) {
        case 0: // Vertical Down
            if (test_y + 2 >= BOARD_SIZE_X) { // Check bounds
                xil_printf("Error: Ship out of bounds vertically (down).\r\n");
                return -1; // Ship placement failed
            }
            for (int i = 0; i < 3; i++) {
                if (user_board[test_y + i][test_x] != EMPTY) { // Check collision
                    xil_printf("Error: Collision detected at (%d, %d).\r\n", test_y + i, test_x);
                    return -1; // Ship placement failed
                }
            }
            for (int i = 0; i < 3; i++) { // Place the ship
                user_board[test_y + i][test_x] = SHIP;
            }
            break;

        case 1: // Horizontal Right
            if (test_x + 2 >= BOARD_SIZE_Y) { // Check bounds
                xil_printf("Error: Ship out of bounds horizontally (right).\r\n");
                return -1; // Ship placement failed
            }
            for (int i = 0; i < 3; i++) {
                if (user_board[test_y][test_x + i] != EMPTY) { // Check collision
                    xil_printf("Error: Collision detected at (%d, %d).\r\n", test_y, test_x + i);
                    return -1; // Ship placement failed
                }
            }
            for (int i = 0; i < 3; i++) { // Place the ship
                user_board[test_y][test_x + i] = SHIP;
            }
            break;

        case 2: // Vertical Up
            if (test_y - 2 < 1) { // Check bounds
                xil_printf("Error: Ship out of bounds vertically (up).\r\n");
                return -1; // Ship placement failed
            }
            for (int i = 0; i < 3; i++) {
                if (user_board[test_y - i][test_x] != EMPTY) { // Check collision
                    xil_printf("Error: Collision detected at (%d, %d).\r\n", test_y - i, test_x);
                    return -1; // Ship placement failed
                }
            }
            for (int i = 0; i < 3; i++) { // Place the ship
                user_board[test_y - i][test_x] = SHIP;
            }
            break;

        case 3: // Horizontal Left
            if (test_x - 2 < 0) { // Check bounds
                xil_printf("Error: Ship out of bounds horizontally (left).\r\n");
                return -1; // Ship placement failed
            }
            for (int i = 0; i < 3; i++) {
                if (user_board[test_y][test_x - i] != EMPTY) { // Check collision
                    xil_printf("Error: Collision detected at (%d, %d).\r\n", test_y, test_x - i);
                    return -1; // Ship placement failed
                }
            }
            for (int i = 0; i < 3; i++) { // Place the ship
                user_board[test_y][test_x - i] = SHIP;
            }
            break;

        default:
            xil_printf("Error: Invalid orientation.\r\n");
            return -1; // Ship placement failed
    }

    xil_printf("Ship placed successfully!\r\n");
    print_board(); // Print the updated board for debugging
    return 0; // Success
}

// Print the board for debugging purposes
void print_board(void) {
    printf("  A B C D E F\n");
    for (int i = 1; i < BOARD_SIZE_X; i++) {
        printf("%d ", i);
        for (int j = 0; j < BOARD_SIZE_Y; j++) {
            printf("%c ", user_board[i][j]);
        }
        printf("\n");
    }
}

// Print the enemy board for debugging purposes
void print_enemy_board(void) {
    printf("  A B C D E F\n");
    for (int i = 1; i < BOARD_SIZE_X; i++) {
        printf("%d ", i);
        for (int j = 0; j < BOARD_SIZE_Y; j++) {
            printf("%c ", enemy_board[i][j]);
        }
        printf("\n");
    }
}


//Use this function to statically set enemy board
void set_enemy_board(void) {
    // Set the first ship at A1, A2, A3
    enemy_board[1][0] = SHIP;
    enemy_board[2][0] = SHIP;
    enemy_board[3][0] = SHIP;

    // Set the second ship at C1, C2, C3
    enemy_board[1][2] = SHIP;
    enemy_board[2][2] = SHIP;
    enemy_board[3][2] = SHIP;

        // Set the third ship at D1, D2, D3
    enemy_board[1][3] = SHIP;
    enemy_board[2][3] = SHIP;
    enemy_board[3][3] = SHIP;

    xil_printf("Enemy board ships set statically.\r\n");
    print_enemy_board();

}
void user_fires(int x, int y) {
    test_x = x;
    test_y = y;

    xil_printf("User fires at coordinates: (%d, %d)\r\n", x, y);
    check_hit(enemy_board, 0); // Do not update queues
}

//enemy chooses random location to shoot at
// TODO: Implement heatmap logic to decide what location to shoot at
void enemy_fires(void) {
    int random_x, random_y;

    // Seed the random number generator with a fixed value
    srand(42);

    if (immediate_queue_size > 0) {
        random_x = immediate_queue[0][0];
        random_y = immediate_queue[0][1];
        for (int i = 1; i < immediate_queue_size; i++) {
            immediate_queue[i - 1][0] = immediate_queue[i][0];
            immediate_queue[i - 1][1] = immediate_queue[i][1];
        }
        immediate_queue_size--;
    } else if (secondary_queue_size > 0) {
        random_x = secondary_queue[0][0];
        random_y = secondary_queue[0][1];
        for (int i = 1; i < secondary_queue_size; i++) {
            secondary_queue[i - 1][0] = secondary_queue[i][0];
            secondary_queue[i - 1][1] = secondary_queue[i][1];
        }
        secondary_queue_size--;
    } else {
        while (1) {
            random_x = rand() % BOARD_SIZE_Y;
            random_y = 1 + rand() % (BOARD_SIZE_X - 1);

            if (user_board[random_y][random_x] != 'H' && user_board[random_y][random_x] != 'M') {
                break;
            }
        }
    }

    test_x = random_x;
    test_y = random_y;

    xil_printf("Computer fires at coordinates: (%d, %d)\r\n", random_x, random_y);
    check_hit(user_board, 1); // Pass 1 to indicate queues should be updated
}



int check_winner(char board[BOARD_SIZE_X][BOARD_SIZE_Y]) {
    int hit_count = 0;

    // Iterate through the board to count 'H' cells
    for (int i = 1; i < BOARD_SIZE_X; i++) { // Rows: 1 to BOARD_SIZE_X - 1
        for (int j = 0; j < BOARD_SIZE_Y; j++) { // Columns: 0 to BOARD_SIZE_Y - 1
            if (board[i][j] == 'H') {
                hit_count++;
            }

            // If 9 hits are found, declare a winner
            if (hit_count == 9) {
                return 1; // Winner found
            }
        }
    }

    return 0; // No winner yet
}
