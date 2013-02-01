#include <stdio.h>      /* All input and output done through standard streams. */
#include <stdlib.h>     /* For exit() and [s]rand(). */
#include <string.h>     /* For strlen to calculate some digits. */
#include <ctype.h>      /* For toupper() to compare characters. */
#include <time.h>       /* Seeding random numbers for AI. */

#define GRID_SIZE 3
#define GRID_BLANK_CHAR ' '
#define PLAYER_1_CHAR 'X'
#define PLAYER_2_CHAR 'O'
#define MAX_STRING_SIZE 80
#define COORDINATE_ADJUSTMENT -1

char tictactoe_grid[GRID_SIZE][GRID_SIZE];      /* The game grid is always symmetrical. */

int getPlayerInfo(char key, char goal) {
    /* N - key=character, return=number
     * C - key=number, return=character
     * Get player information. By supplying either the player number
     * or the player's grid character and specifying what piece of
     * information you want, you can obtain the other piece of
     * information.
     * Returns the relevant information. Exits program with a fatal
     * error if no match if found. */
    char playerCharTable[][2] = { { 1, 'X' },
                                  { 2, 'O' } };
    int i;
    /* Have number, want character. */
    if( toupper(goal) == 'C' ) {
        for( i = 0; i < 2; i++ ) {
            if( playerCharTable[i][0] == key ) {
                return playerCharTable[i][1];
            }
        }
    /* Have character, want number. */
    } else if( toupper(goal) == 'N' ) {
        for( i = 0; i < 2; i++ ) {
            if( playerCharTable[i][1] == key ) {
                return playerCharTable[i][0];
            }
        }
    /* Supplied bad information within the code. */
    } else {
        puts("Fatal error: Invalid player information.");
        exit(1);
    }
    /* Will never reach this. */
    return 0;
}

int getGridCell(int x, int y) {
    /* Get the character in a cell represented by the given coordinates.
     * Return -1 if error (i.e., coordinates out of grid bounds), and
     * cell's value (character) otherwise. */
    if( (x < GRID_SIZE) && (y < GRID_SIZE) ) {
        return tictactoe_grid[x][y];
    } else {
        return -1;
    }
}

int setGridCell(int x, int y, char c) {
    /* Set the cell represented by the given coordinates to the
     * character given.
     * Return -1 if error (i.e., coordinates out of grid bounds),
     * and the character set otherwise. */
    if( (x < GRID_SIZE) && (y < GRID_SIZE) ) {
        tictactoe_grid[x][y] = c;
        return c;
    } else {
        return -1;
    }
}

int isFullGrid() {
    /* Loops through the entire grid. Returns 0 if a blank
     * character is found. Returns 1 if no blank characters
     * are found (i.e., grid is full). */
    int i, j;
    for( i = 0; i < GRID_SIZE; i++ ) {
        for( j = 0; j < GRID_SIZE; j++ ) {
            if( getGridCell(i, j) == GRID_BLANK_CHAR ) {
                return 0;
            }
        }
    }
    /* Grid is full. */
    return 1;
}

int clearInputStreamLine() {
    /* Discards all input from stdin until it reaches the end
     * of the line.
     * NOTE: SHOULD ONLY BE USED WHEN IT IS CERTAIN THAT AT LEAST
     * ONE END-OF-LINE CHARACTER IS LEFT IN THE BUFFER. Will give user
     * a blank prompt for input otherwise. */
    while( getchar() != '\n' );
    return 0;
}

int resetGrid() {
    /* Wipes the entire grid by replacing each cell with the grid
     * blank character. If somehow something goes wrong and it
     * doesn't loop all of the way through, -1 is returned.
     * Otherwise, the grid size is returned. */
    int i, j;
    for( i = 0; i < GRID_SIZE; i++ ) {
        for( j = 0; j < GRID_SIZE; j++ ) {
            setGridCell(i, j, GRID_BLANK_CHAR);
        }
    }
    if( (i == (GRID_SIZE)) && (j == (GRID_SIZE)) ) {
        return GRID_SIZE;
    } else {
        return -1;
    }
}

int performAIMove() {
    /* TERRIBLE AI.
     * Seeds a random n umber generator, generates x and y coords,
     * checks to see if those coords are taken, if not moves there.
     * This is a purely random AI and is technically never even
     * guaranteed to find a move if somehow you don't randomly
     * generate an empty cell after billions of tries.
     * Cells are numbered left-to-right, top-to-bottom, using the
     * numbers 0 through GRID_SIZE. */
    srand( time(NULL) );
    int random_cell, random_x, random_y;
    do {
        random_cell = rand() % (GRID_SIZE*GRID_SIZE);
        /* Determine row and column placement based on grid numbering. */
        random_x = random_cell / GRID_SIZE;
        random_y = random_cell - (random_x * GRID_SIZE);
    } while( getGridCell(random_x, random_y) != GRID_BLANK_CHAR );
    /* Get the appropriate char and make the move. */
    char ai_char = getPlayerInfo(2, 'c');
    setGridCell(random_x, random_y, ai_char);
    printf("\nTerrAI selects <%d,%d>\n", random_x+1, random_y+1);
    return 0;
}

int adjustCoords(int * x, int * y, int adjustment) {
    /* Adjusts the coordinates the player entered to fit the grid's
     * structure in the code. Probably won't ever be anything other
     * than -1. Returns 0 on success, non-zero (-1) on failure. */
    *x += adjustment;
    *y += adjustment;
    return 0;
}

int promptNextMove(int player, int * x, int * y) {
    /* Prompts the player for their move, breaks the string into the
     * two coordinates, and sets both x and y to their respective
     * values. Checks for correct input along the way. Recursive on
     * bad input. Returns 0 on good input, but cannot return without
     * good input. */
    printf("\nPlayer %d, enter your move (x,y): ", player);
    char input[MAX_STRING_SIZE];
    char format_string[MAX_STRING_SIZE] = "";
    sprintf(format_string, "%%%ds", MAX_STRING_SIZE);
    if( fscanf(stdin, format_string, input) != 1 ) {
        puts("Invalid move coordinate: No input detected. Please use form (x,y).");
    } else {
        if( sscanf(input, "%d,%d", x, y) != 2 ) {
            puts("Invalid move coordinate: Did not detect x or y. Please use form (x,y).");
        } else {
            adjustCoords(x, y, COORDINATE_ADJUSTMENT);
            switch( getGridCell(*x, *y) ) {
                case -1:
                    puts("Invalid move coordinate: Cell out of bounds.");
                    break;
                case GRID_BLANK_CHAR:
                    /* The 'true' return. Cleans up stdin first. */
                    clearInputStreamLine();
                    return 0;
                default:
                    puts("Invalid move coordinate: Cell already taken.");
            }
        }
    }
    /* Will only reach this part if there is some error. Cleans up stdin. */
    clearInputStreamLine();
    return promptNextMove(player, x, y);
}

int drawGrid() {
    /* Draws the current state of the grid to the scene. Goes
     * line-by-line, printing either spaces or symbols as
     * necessary. Calculates how many lines to print based
     * on how many cells there are (calculated from the grid size).
     * Returns 0 on success, non-zero (-1) on failure.*/
    int i, j;
    printf("\n");
    /* Print cells, one row at a time. */
    for( i = 0; i < GRID_SIZE; i++ ) {
        /* When starting each row after the first, print a --- line. */
        if( i > 0 ) {
            for( j = 0; j < GRID_SIZE; j++ ) {
                printf("---");
                /* Account for column separators. */
                if( j < GRID_SIZE-1 ) {
                    printf("-");
                }
            }
            /* Advance to next line to print the cell. */
            printf("\n");
        }
        int line;
        /* Each cell is actually 3 lines to give a nice padding. */
        for( line = 0; line < 3; line++ ) {
            for( j = 0; j < GRID_SIZE; j++ ) {
                /* When starting each colun after first, print | separator. */
                if( j > 0 ) {
                    printf("|");
                }
                /* Only the middle line shows the cell's mark. */
                if( line == 1 ) {
                printf(" %c ", getGridCell(i, j) );
                /* All other cells print padding. */
                } else {
                    printf("   ");
                }
            }
            /* Advance to next line of cell. */
            printf("\n");
        }
    }
    /* If somehow something goes wrong, return a non-zero value. */
    if( (i == GRID_SIZE-1) && (j == GRID_SIZE-1) ) {
        return 0;
    } else {
        return -1;
    }
}

int getWinStatus() {
    /* Checks all rows, columns, and diagonals for a win. If it finds
     * one, return the player number that won. If it found a full board
     * and no winner, return 0 (tie). If the game hasn't ended yet,
     * return -1. */
    int i, j;
    char winning_character;
    for( i = 0; i < GRID_SIZE; i++ ) {
        /* Check row win */
        for( j = 0; j < GRID_SIZE; j++ ) {
            if( getGridCell(i, j) == GRID_BLANK_CHAR ) {
                break;
            }
            if( getGridCell(i, j) != getGridCell(i, 0) ) {
                break;
            }
        }
        if( j == GRID_SIZE ) {
            winning_character = getGridCell(i, 0);
            return getPlayerInfo(winning_character, 'n');
        }
        /* Check column win */
        for( j = 0; j < GRID_SIZE; j++ ) {
            if( getGridCell(j, i) == GRID_BLANK_CHAR ) {
                break;
            }
            if( getGridCell(j, i) != getGridCell(0, i) ) {
                break;
            }
        }
        if( j == GRID_SIZE ) {
            winning_character = getGridCell(i, 0);
            return getPlayerInfo(winning_character, 'n');
        }
    }
    /* Check diagonal down win */
    for( i = 0, j = 0; i < GRID_SIZE; i++, j++ ) {
        if( getGridCell(i, j) == GRID_BLANK_CHAR ) {
            break;
        }
        if( getGridCell(i, j) != getGridCell(0, 0) ) {
            break;
        }
    }
    if( i == GRID_SIZE ) {
        winning_character = getGridCell(0, 0);
        return getPlayerInfo(winning_character, 'n');
    }
    /* Check diagonal up win */
    for( i = GRID_SIZE-1, j = 0; j < GRID_SIZE; i--, j++ ) {
        if( getGridCell(i, j) == GRID_BLANK_CHAR ) {
            break;
        }
        if( getGridCell(i, j) != getGridCell(GRID_SIZE-1, 0) ) {
            break;
        }
    }
    if( j == GRID_SIZE ) {
        winning_character = getGridCell(GRID_SIZE-1, 0);
        return getPlayerInfo(winning_character, 'n');
    }
    /* Found no wins. Check for tie. */
    if( isFullGrid() ) {
        return 0;
    }
    /* Game hasn't ended. */
    return -1;
}

int playGame(int num_players) {
    /* The game loop. Every time it loops, it:
     * A) Checks to see if a player won.
     * B) Draws the grid.
     * C) Has the player perform their move, gluing together the prompt and the command.
     * -OR-
     * D) Has the AI perform their move.
     * E) Updates which player's turn it current is.
     * Once the game has determined a winer, it reports back who won (player #). */
    int current_player, next_player;
    char win_status;
    current_player = 1;
    next_player = 2;
    while( (win_status = getWinStatus()) < 0 ) {
        drawGrid();
        if( (num_players == 2) || (current_player < 2) ) {  /* Player moves, not AI */
            int input_x, input_y;
            promptNextMove(current_player, &input_x, &input_y);
            if( current_player == 1 ) {
                setGridCell(input_x, input_y, PLAYER_1_CHAR);
            } else {
                setGridCell(input_x, input_y, PLAYER_2_CHAR);
            }
        } else {
            performAIMove();
        }
        /* Next player's turn */
        current_player = next_player;
        if( ++next_player > 2 ) {
            next_player = 1;
        }
    }
    /* If the code reaches this point, that means the game
     * is over. */
    drawGrid();
    return win_status;
}

int startGame(int num_players) {
    /* Checks the number of players, resets the grid, and then
     * launches the game. Passes on the return value of playGame(),
     * which should be the player that won the game. */
    if( (num_players < 1) || (num_players > 2) ) {
        puts("This game only supports 1 or 2 players.");
        return -1;
    }
    if( resetGrid() < 0 ) {
        puts("Fatal error: Unable to reset grid.");
        exit(1);
    }
    return playGame(num_players);
}

int main(void) {
    /* The entry point of the program.
     * Greets the user, then prompts for input. Handles the input,
     * either starting the game or quitting the program. Program
     * can only return 0 unless it exits due to some error. */
    printf("\nWelcome to Tic-Tac-Toe, by Drainlike Games!");
    char input;
    int result;
    int scores[] = { 0, 0 ,0 };    /* Ties, wins, losses. */
    while(1) {
        printf("\n\nHow many players ([1] or [2], [q] to quit)? ");
        input = getchar();
        switch( input ) {
            case '1':
            case '2':
                clearInputStreamLine();
                result = startGame(input - '0');     /* Gives the integer value of the char. */
                if( (result >= 0) && (result <= 2) ) {
                    scores[result]++;
                    printf("\nSCORE - Player 1: %d, Player 2: %d, Ties: %d", scores[1], scores[2], scores[0]);
                } else {
                    printf("\nError retrieving result. Win not recorded.");
                }
                break;
            case EOF:
            case 'q':
                /* True exit. */
                printf("\nThank you for playing!");
                return 0;
            default:
                printf("\nInvalid number of players: Please choose [1], [2], or [q].");
                clearInputStreamLine();
        }
    }
    /* The code won't ever actually reach here, but
     * I like to end main with a return. */
    return 0;
}
