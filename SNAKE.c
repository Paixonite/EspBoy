//BIBLIOTECAS

    #include <conio.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    #include <windows.h>
    #include <stdbool.h>

//CONSTANTS

    // Time in miliseconds between frames
    #define SPEED 75
    #define BOARD_LENGTH 17
    #define BOARD_WIDTH 18
    #define SNAKE_HEAD_CHAR 'Q' 
    #define SNAKE_BODY_CHAR 'o' // Code 1
    #define BOARD_CHAR ' ' // Code 0
    #define APPLE_CHAR '@' // Code 2
    #define WALL_CHAR '#'  // Code -1
    #define PAIXAO 69

//STRUCTS

    struct snakeSegment{
        int x, y;
    };

    struct sApple{
        int x, y;
    };

//GLOBAL VARIABLES  

    int board [BOARD_LENGTH][BOARD_WIDTH]; 

//PROTOTYPES

    // Initializes game's board and variables
    void initGame(struct sApple *apple, char *direction, int *score, int *currentLength, struct snakeSegment *snake);

    // Prints the snake and apple in the screen
    void printsBoard(int score, struct snakeSegment *snake);

    // Changes board length and width (unused)
    void startMenu();

    // Alters value of direction to 'W','A','S', or 'D'
    void readsMovement(char *direction);

    // Returns 1 if you are alive, 0 if you died
    int moves(char direction, struct sApple *apple, int *currentLength, int *score, struct snakeSegment *snake);

    // Returns apple cordinates
    struct sApple spawnsApple();

    // Returns 1 to play and 0 to exit
    int gameOverScreen(int score,  int highscores[], int iterationCounter);

    // Return 1 to play and 0 to exit
    int showHighscores(int highscores[], int iterationCounter);

//MAIN

    int main() {
        struct sApple apple;
        struct snakeSegment snake[BOARD_LENGTH * BOARD_WIDTH];
        char direction;
        int score;
        int currentLength;
        int highscores [10] = {0};
        int iterationCounter = 0;

        do{
            initGame(&apple, &direction, &score, &currentLength, snake);
            while (moves(direction, &apple, &currentLength, &score, snake)){
                printsBoard(score, snake);
                Sleep(SPEED);
                readsMovement(&direction);
            }
            printf("\7");
            highscores[iterationCounter] = score;
            iterationCounter++;
        }while(gameOverScreen(score, highscores, iterationCounter));
        return PAIXAO;
    }

//FUNCTIONS

    void initGame(struct sApple *apple, char *direction, int *score, int *currentLength, struct snakeSegment *snake){
        *score = 0;
        *currentLength = 4;
        *direction = 0;

        // Initializes board
        for (int i = 0; i < BOARD_LENGTH; ++i)
            for (int j = 0; j < BOARD_WIDTH; ++j)
                board[i][j] = 0;

        // Sets snake
        for (int i = 0; i < *currentLength; i++)
            board[(snake[i].x = (BOARD_LENGTH / 2))] [snake[i].y = 2] = 1;
    
        // Sets first apple
        board[apple->x = (BOARD_LENGTH / 2)][apple->y = (BOARD_WIDTH - 3)] = 2;

        // Sets walls
        for (int i = 0; i < BOARD_LENGTH; ++i){
            board[i][0] = -1;
            board[i][BOARD_WIDTH - 1] = -1;    
        }
        for (int i = 0; i < BOARD_WIDTH; ++i){
            board[0][i] = -1;
            board[BOARD_LENGTH - 1][i] = -1; 
        }
    }

    void printsBoard(int score, struct snakeSegment *snake){
        system("cls");
        printf("\033[33mSCORE: %i\n\033[0m", score);
        for (int i = 0; i < BOARD_LENGTH; ++i){
            for (int j = 0; j < BOARD_WIDTH; ++j)
                switch (board[i][j]){
                    case -1: //WALL
                        printf("%c ", WALL_CHAR);
                        break;

                    case 0: // BOARD
                        printf("%c ", BOARD_CHAR);;
                        break;

                    case 1: // SNAKE
                        if(snake[0].x == i && snake[0].y == j)
                            printf("\033[92m%c \033[0m", SNAKE_HEAD_CHAR);
                        else
                            printf("\033[92m%c \033[0m", SNAKE_BODY_CHAR);
                        break;

                    case 2: // APPLE
                        printf("\033[31m%c \033[0m", APPLE_CHAR);
                        break;
                }
            putchar('\n');
        }
    }

    void startMenu(){
        int input;
        printf("*************************\n");
        printf("WELCOME TO THE SNAKE GAME\n");
        printf("*************************\n");
        printf("Select the map size:\n");
        printf("(1) Small\n");
        printf("(2) Medium\n");
        printf("(3) Big\n");
        scanf("%i", &input);

        switch (input){
            case 1:
                //10x10
                break;
            case 2:
                //20x20
                break;
            case 3:
                //30x30
                break;
        }
    }

    void readsMovement(char *direction){
        if (_kbhit()) {
            char key = _getch();

            switch (key) {
                case 72: // CIMA
                    if (*direction != 'S')
                        *direction = 'W';
                    break;

                case 75: // ESQUERDA
                    if (*direction != 'D')
                        *direction = 'A';
                    break;

                case 80: // BAIXO
                    if (*direction != 'W')
                        *direction = 'S';
                    break;

                case 77: // DIREITA
                    if (*direction != 'A')
                        *direction = 'D';
                    break;
            }
        }
    }

    int moves(char direction, struct sApple *apple, int *currentLength, int *score, struct snakeSegment *snake){

        int collided;
            
        // Moves the body
        for (int i = (*currentLength - 1); i > 0; i--)
            snake[i] = snake[i - 1];

        // Moves the head, stores what is has collided into
        switch (direction){
            case 'W':
                switch (board[snake[0].x - 1][snake[0].y]){
                    case -1:
                        collided = -1;
                        break;
                    case 0:
                        collided = 0;
                        snake[0].x --;
                        break;
                    case 1:
                        collided = 1;
                        break;
                    case 2:
                        snake[0].x --;
                        collided = 2;
                        break;
                }
                break;

            case 'A':
                switch (board[snake[0].x][snake[0].y - 1]){
                    case -1:
                        collided = -1;
                        break;
                    case 0:
                        collided = 0;
                        snake[0].y --;
                        break;
                    case 1:
                        collided = 1;
                        break;
                    case 2:
                        snake[0].y --;
                        collided = 2;
                        break;
                }
                break;

            case 'S':
                switch (board[snake[0].x + 1][snake[0].y]){
                    case -1:
                        collided = -1;
                        break;
                    case 0:
                        collided = 0;
                        snake[0].x ++;
                        break;
                    case 1:
                        collided = 1;
                        break;
                    case 2:
                        snake[0].x ++;
                        collided = 2;
                        break;
                }
                break;

            case 'D':
                switch (board[snake[0].x][snake[0].y + 1]){
                    case -1:
                        collided = -1;
                        break;
                    case 0:
                        collided = 0;
                        snake[0].y ++;
                        break;
                    case 1:
                        collided = 1;
                        break;
                    case 2:
                        snake[0].y ++;
                        collided = 2;
                        break;
                }
                break;
        }

        // Grows tail
        if (collided == 2){
            (*currentLength)++;
            snake[*currentLength] = snake[(*currentLength) - 1];
        }

        // Resets board
        for (int i = 1; i < BOARD_LENGTH - 1; ++i)
            for (int j = 1; j < BOARD_WIDTH - 1; ++j)
                board[i][j] = 0;

        // Puts snake and apple (if not eaten) in board
        for (int i = 0; i < (*currentLength); ++i)
            board[snake[i].x][snake[i].y] = 1;
        if (collided != 2)
            board[apple->x][apple->y] = 2;

        switch (collided){
            case -1:
                return 0;
                break;
            case 0:
                return 1;
                break;
            case 1:
                return 0;
                break;
            case 2:
                (*score) ++;
                *apple = spawnsApple();
                return 1;
                break;
        }
    }

    struct sApple spawnsApple(){
        struct sApple apple; 
        do{
            srand(clock());
            apple.x = rand() % BOARD_LENGTH ;
            srand(clock());
            apple.y = rand() % BOARD_WIDTH;
        }while(board[apple.x][apple.y] != 0);
        return apple;
    }

    int gameOverScreen(int score, int highscores[], int iterationCounter){
        int input;
        system("cls");
        printf("\033[1;31m************************\n");
        printf("       GAME OVER!\n");
        printf("       Score: %i\n", score);
        printf("************************\033[0m\n");
        printf("(1) Play again\n");
        printf("(2) Highscores\n");
        printf("(3) Exit game\n");
        do{
            scanf("%i", &input);
            switch(input){
                case 1:
                    return 1;
                    break;
                case 2:
                    if(showHighscores(highscores, iterationCounter))
                        return 1;
                    else
                        return 0;
                    break;
                case 3:
                    return 0;
                    break;
                default:
                    printf("ERROR: command not found.\n");
                    break;
            }
        }while(input != 1 && input != 2 && input != 3);
    }

    int showHighscores(int highscores[], int iterationCounter){
        int input;
        printf("- HIGHSCORES -\n");
        system("cls");
        for (int i = 0; i < iterationCounter; ++i)
            printf("%i - %i\n", i + 1, highscores[i]);
        printf("\n(1) Play again\n");
        printf("(2) Exit game\n");
        do{
            scanf("%i", &input);
            switch(input){
                case 1:
                    return 1;
                    break;
                case 2:
                    return 0;
                    break;
                default:
                    printf("ERROR: command not found.\n");
                    break;
            }
        }while(input != 1 && input != 2);
    }