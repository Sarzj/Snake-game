#include <curses.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>

int snakePos[100][2]; // Store positions of the snake
int snakeLen = 3;
int direction = KEY_RIGHT; // Holds direction of snake 
float snakeSpeed = 1 ; 

#define trophies 1 // amount of trophies


void drawBorders() { // makes a line on the top and bottom as well as across to first and last of each row and does the same - Joseph G

    for (int i = 0; i < COLS; i++) {
        addstr("#");
    }
    for (int i = 1; i < LINES; i++) {
        move(i, COLS - 1);
        addstr("#");
        move(i, 0);
        addstr("#");
    }
    for (int i = 0; i < COLS; i++) {
        addstr("#");
    }
}
struct Trophy { // structure of the throphies - Julio S
    int x, y;
    int consumed;
    int Tvalue;
    
};

struct Trophy trophy[trophies];


// Function to clear the previous position of the trophy on the screen
void clearTrophy(int x, int y) {
    move(y, x);
    addch(' '); // Clear the character at the previous position
}

// makes random pointers for the trophy - Julio S
void setup_trophy() {
    int i;
    clearTrophy(trophy[0].x, trophy[0].y);

    for(i=0; i<trophies; i++) {
        trophy[i].x =   rand() % (COLS-2)+1;
        trophy[i].y =   rand() % (LINES-2)+1;
        trophy[i].consumed = 0;
        trophy[i].Tvalue = (rand() % 9) + 1;
    }
}


void moveSnake() { //Changes direction of snake thru output of user input - Joseph G
    int x = snakePos[0][0];
    int y = snakePos[0][1];

    // Move the head
    switch (direction) {
    case KEY_UP:
        x--;
        break;
    case KEY_DOWN:
        x++;
        break;
    case KEY_LEFT:
        y--;
        break;
    case KEY_RIGHT:
        y++;
        break;
    }

    // collision checkers we will need to add trophies to this once we find out what we want that to look like - Joseph G
    //I may want to rewrite this to just check the charcter being overwritten by the head of the snake later if we can find a way to do that but this works for now 
    if (x <= 0 || x >= LINES-1 || y <= 0 || y >= COLS-1) { // checks bounds 
        endwin();
        exit(0);
    }
    for (int i = 1; i < snakeLen; i++) { //checks if snake overlaps with itself - Joseph G
        if (x == snakePos[i][0] && y == snakePos[i][1]) {
            endwin();
            exit(0);
        }
    }

    // Erase no longer needed parts of the snake - Joseph G
    move(snakePos[snakeLen - 1][0], snakePos[snakeLen - 1][1]);
    addstr(" ");

    // Move body of snake
    for (int i = snakeLen - 1; i > 0; i--) {
        snakePos[i][0] = snakePos[i - 1][0];
        snakePos[i][1] = snakePos[i - 1][1];
    }

    snakePos[0][0] = x;
    snakePos[0][1] = y;
}

int main() {
    srand(time(0)); // setup randomizer 

    //initialize screen and do general setup of removing echo and cursor - Joseph G
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    clear();
    noecho();

    // The following allows for inputs to be read every time the input is encountered - Joseph G
    struct termios term;
    tcgetattr(STDIN_FILENO, & term);
    term.c_lflag &= ~ICANON;
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, & term);

    int randomDir = (rand() % 3) + 1; //randomize the starting direction - Joseph G
    switch (randomDir) {
        case 1:
            direction = KEY_RIGHT;
        break;
        case 2:
            direction = KEY_UP;
        break;
        case 3: 
            direction = KEY_DOWN;
        break;
    }
    
    int halfPerim = COLS + LINES;//for win condition - Joseph
    int trophyTimer = 0; // for checking how long the current trophy has been on screen - Joseph G

    drawBorders();
  
    snakePos[0][0] = LINES / 2; snakePos[0][1] = COLS / 2;// Place snake at center - Joseph G
    
    //Draw rest of the snake - Joseph G
    for (int i = 1; i < snakeLen; i++) {
        snakePos[i][0] = snakePos[i - 1][0];
        snakePos[i][1] = snakePos[i - 1][1] - 1;
    }
    setup_trophy(); // set ups the pointers for the trophy - Julio S

    while (1) {
        
        if (snakeLen >= halfPerim){ // ends the game if the length of the snakes is = to Perimeter/2 - Julio S
            move(LINES/2, COLS/2);
            addstr("You win!");
            refresh();
            usleep(3000000);
            endwin();
            return 0;
        }

        moveSnake();
        // Draw snake on position - Joseph G
        for (int i = 0; i < snakeLen; i++) {
            move(snakePos[i][0], snakePos[i][1]);
            addstr("o");
        }
        
        //prints the trophies at set_trophy location - Julio S
        for(int i=0; i<trophies; i++) {
            if( !trophy[i].consumed ) {
                move(trophy[i].y , trophy[i].x);
                int number= trophy[i].Tvalue;
                char str[4];
                snprintf(str, sizeof(str), "%d", number);
                addstr(str); 
            }
        }

        //consume the trophy at the location of the head - Julio S
        for(int i=0; i<trophies; i++) {
            if( !trophy[i].consumed ) {
                if( trophy[i].x == snakePos[i][1] && trophy[i].y == snakePos[i][0] ) {
                    trophy[i].consumed = 1;
                    snakeLen= snakeLen + trophy[i].Tvalue;
                    snakeSpeed = snakeSpeed + (trophy[i].Tvalue * 0.02); // increase snake speed by .02 for value of growth - Joseph G
                    setup_trophy();  // spawns a new throphy - Julio S  
                }
            }
        }



        // Check for user input, turn it into a proper output for changing direction- Joseph G
        int ch = getch();
        switch (ch) {
        case KEY_UP:
        case 'w':
        case 'W':
            if (direction != KEY_DOWN) // so user cannot go back into themselves
                direction = KEY_UP;
            break;
        case KEY_DOWN:
        case 's':
        case 'S':
            if (direction != KEY_UP) // so user cannot go back into themselves
                direction = KEY_DOWN;
            break;
        case KEY_LEFT:
        case 'a':
        case 'A':
            if (direction != KEY_RIGHT) // so user cannot go back into themselves
                direction = KEY_LEFT;
            break;
        case KEY_RIGHT:
        case 'd':
        case 'D':
            if (direction != KEY_LEFT) // so user cannot go back into themselves
                direction = KEY_RIGHT;
            break;
        }

        //change trophy location at random interval of 1 to 9 seconds - Joseph Grenier
        if (trophyTimer > 10) {//only goes if greater than 1 sec
            int chance = (rand() % 1000) + 1;
            if(chance == 4 || trophyTimer >= 90){//triggers if chance met or its been over 9 sec 
            setup_trophy();
            trophyTimer = 0; // Reset timer
            }
        }

        //100000 sec -> 0.1  seconds | 1000000 sec -> 1 second | 9000000 sec -> 9 seconds
        usleep(100000/snakeSpeed); // snake speed / overall game speed 
        trophyTimer++;
        move(0,0);
        addstr("#");
        refresh();
    }
}
