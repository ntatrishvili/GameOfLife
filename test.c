#include<stdio.h>
#include <stdlib.h>

//the board can be random or read from the file (specification 2.2)
typedef enum boardType {
    ReadFromFile, 
    Randomized,
    Error
} boardType;

//randomizes the board
int fillBoardRandom(int width, int height, char** arr){
    srand(time(NULL));
    int r = rand();
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            if(r%3!=0){
                arr[i][j] = ' ';
                printf("%c",arr[i][j]);
            }
            else{
                arr[i][j] = '@';
            }
            r = rand();
        }
    }
    //TO DO ERRORS
    if(1<0){
        return 0;
    }
    return 1;
}
//inputs the board from the file
int fillBoardFromFile(int width, int height, char** arr, FILE *fp){
    int status = 1;
    char curr = ' ';
    if(fp== NULL){
        printf("could not find file, please try again! \n");
        return 1;
    }
    //read
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            curr = fgetc(fp);
            if(curr == ' ')
                arr[i][j] = ' ';
            else if(curr == '@')
                arr[i][j] = '@';
            else
                j--;
            
        }
    }
    status = fclose(fp);
    if(status != 0)
        return 1;
    return 0;
}
//prints the board
void printBoard(int width, int height, char**board){
    printf("\n");
    printf("%d %d\n", width, height);
    for (int i = 0; i < width+2; i++){
        printf("-");
    }
    printf("\n");
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            printf("%c", board[i][j]);
        }
    }
    for (int i = 0; i < width+2; i++){
        printf("-");
    }
}
//saves current board to the file
int saveBoardInFile(int width, int height, char** board, FILE *fp){
    int status = 1; 
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            fputc(board[i][j], fp);
        }
        fputc('\n', fp);
    }
    status = fclose(fp);
    if(status != 0)
        return 1;
    return 0;
}
//calculates the next state of the cell
char CellNextState(char current, int neighbourCount){
    return ((neighbourCount == 2 && current == '@') || neighbourCount == 3) ? '@' : ' ';
}
//moves on to the next state
void nextBoard(int width, int height, char** board){
    int neighbourCount = 0;
    
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            if (i>0)
                neighbourCount += board[i-1][j] == '@';
            if (j>0)
                neighbourCount += board[i][j-1] == '@';
            if(i<(height-1))
                neighbourCount += board[i+1][j] == '@';
            if(j<(width-1))
                neighbourCount += board[i][j+1] == '@';
            board[i][j] = CellNextState(board[i][j], neighbourCount);
            neighbourCount = 0;
        }
    }
}
//counts the cells that are alive
int aliveCellCount(int width, int height, char** board){
    int aliveCellsCount = 0;
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            aliveCellsCount += (board[i][j] == '@');
        }
    }
    printf("alivecellscount = %d\n", aliveCellsCount);
    return aliveCellsCount;
}
//appends the current board to the archive file
int addToArchive(int width, int height, char** board){
    FILE *fp;
    fp = fopen("archive.txt", "a");
        if(fp== NULL){
            printf("action could not be saved!!");
            return 0;
        }
    
    int status = 1; 
    fputc('\n', fp);
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            fputc(board[i][j], fp);
        }
        fputc('\n', fp);
    }

    status = fclose(fp);
    if(status != 0)
        return 1;
    return 0;
}



//displays the main rules in the console
void displayRules(){
    printf("Welcome to the two-dimentional world of the Game of Life!!\n");
    printf("Here, small creatures called \'cells\' live neighbouring each other\n");
    printf("They might look simple, but they are just like you in social situations!\n");
    printf("If they have less than 2 neighbours, they die of loneliness\n");
    printf("But if they have more than 3 neighbours, they die of social anxiety\n");
    printf("Otherwise, they are okay!\n");
    printf("Oh, and also, with the right amount of neighbours (exactly 3), a dead cell can come alive!!\n");
    printf("As a three-dimentional being, you can observe their life by looking at their plane of existance step-by-step.\n");
    printf("So, are you ready to explore the wonderous world of the game of life?\n");
    printf("Then follow these instructions. \n\n\n");
}

//asks the user for the instructions
boardType instructions(int *width, int* height){
    char c;
    printf("First, input the size of the world, that your cells live in (width,height) : ");
    scanf("(%d,%d)", width, height);
    printf("\nThen, write F if you have already prepared the initial state of your cells in the File, or write R if you want to view a random Game of Life: ");
    scanf(" %c", &c);
    // if(c=='F' || c=='f'){
    //     printf("please make sure your board")
    // }
    return (c=='F'|| c=='f') ? ReadFromFile : (c=='R' || c=='r') ? Randomized : Error;
}
//fill the board according to the instructions
int fillBoard(int width, int height, char** board, boardType instruction){
    FILE *fp;
    board = (char**)malloc(height*sizeof(char*));
    for(int i = 0; i<height; i++){
        board[i] = (char*)malloc(width*sizeof(char));
    }
    if (instruction == ReadFromFile){
        fp = fopen("input_board.txt", "r");
        if(fp== NULL){
            printf("could not read from the file, please make sure you have correctly inputed the board!");
            return 0;
        }
        printf("The board read from your file: \n");
        fillBoardFromFile(width, height, board, fp);

    }
    else if(instruction == Randomized){
        printf("The randomized world:\n");
        fillBoardRandom(width, height, board);
        fp = fopen("logs.txt", "w");
        saveBoardInFile(width, height, board, fp);
        printBoard(width, height, board);
    }
}
//simulates the process
int simulation(int width, int height, char** board){
    int i = 0;
    int alives = aliveCellCount(width, height, board);
    printf("\n alive cells: %d\n",alives);
    printBoard(width, height, board);
    while(alives > 0 && i<20){
        nextBoard(width,height,board);
        alives = aliveCellCount(width, height, board);
        printBoard(width, height, board);
        addToArchive(width, height, board);
        printf("\n alive cells: %d\n",alives);
        i++;
    }
}

int main(){
    int width, height;
    char **board;

    //show user the main rules
    displayRules();
    //ask for instructions
    boardType instruction= instructions(&width, &height);
    //fill in the board according to these instructions
    fillBoard(width, height, board, instruction);
    //simulate life
    simulation(width, height, board);

    //clear the memory 
    free(board[0]);
    free(board);
    return 0;
}