#include<stdio.h>
#include <stdlib.h>
#include<time.h>
//#include <unistd.h> //for sleep in Linux
//#include<dos.h> //for sleep in Windows

//the board can be random or read from the file (specification 2.2)
typedef enum boardType {
    ReadFromFile, 
    Randomized,
    Error
} boardType;

//the user can choose one of three kinds of instructions in the exit menu
typedef enum endInstruction {
    exitGame,
    showMax,
    showCycle,
    ErrorInstruction
} endInstruction;

///THESE ARE THE SMALL HELPER FUNCTIONS FOR READING THE INSTRUCTIONS AND THE SIMULATION
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
    for (int i = 0; i < width+2; i++){
        printf("-");
    }
    printf("\n");
    for(int i = 0; i<height; i++){
        printf("|");
        for(int j = 0; j<width; j++){
            printf("%c", board[i][j]);
        }
        printf("|\n");
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
            if(i>0 && j>0)
                neighbourCount += board[i-1][j-1] == '@';
            if(i<(height-1) && j<(width-1))
                neighbourCount += board[i+1][j+1] == '@';
            if(i>0 && j<(width-1))
                neighbourCount += board[i-1][j+1] == '@';
            if(i<(height-1) && j>0)
                neighbourCount += board[i+1][j-1] == '@';
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


///THESE ARE THE BIG HELPER FUNCTIONS FOR READING THE INSTRUCTIONS AND THE SIMULATION
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
boardType StartInstructions(int *width, int* height){
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
        fp = fopen("input_board.txt", "w");
        saveBoardInFile(width, height, board, fp);
        printBoard(width, height, board);
    }
    return 0;
}
//simulates the process
int simulation(int width, int height, char** board){
    int i = 0;
    time_t start, end;
    int alives = aliveCellCount(width, height, board);
    while(alives > 0 && i<20){
        printBoard(width, height, board);
        addToArchive(width, height, board);
        printf("\n alive cells: %d\n",alives);
        sleep(1);
        nextBoard(width,height,board);
        alives = aliveCellCount(width, height, board);
        i++;
    }
    return 0;
}



//reading the instructions and simulating the Game of Life
void gameOfLife(){
    int width, height;
    char **board;

    //show user the main rules
    displayRules();
    //ask for instructions
    boardType instruction= StartInstructions(&width, &height);
    board = (char**)malloc(height*sizeof(char*));
    for(int i = 0; i<height; i++){
        board[i] = (char*)malloc(width*sizeof(char));
    }
    //fill in the board according to these instructions
    fillBoard(width, height, board, instruction);
    //simulate life
    simulation(width, height, board);


    //clear the memory
    for(int i=0; i<height; i++){
        free(board[i]);
    }
    free(board);
}

///THESE ARE THE SMALL HELPER FUNCTIONS FOR DISPLAYING THE STATISTICS
//exit Menu
endInstruction exitInstructions(){
    int menuCode;
    printf("\n\nPhew... This was your simulation! \nNow, What do you want to do with this information? Press the corresponding key. ");
    printf("\nPress 1 to view the board with the most alive cells");
    printf("\nPress 2 to see a cycle in your simulation, if there was one!");
    printf("\nPress 0 to exit\n");
    scanf("%d", &menuCode);
    switch (menuCode){
        case 0: return exitGame; break;
        case 1: return showMax; break;
        case 2: return showCycle; break;
        default: return ErrorInstruction;
    }
    return ErrorInstruction;
}
//prints the board with the maximal number of Alive Cells
void printMaxBoard(){

}
//prints the boards with the cycle
void printCycle(){

}

//SHOWING THE STATISTICS OF THE GAME
void showStatistics(){
    int MenuCode;
    endInstruction inst = exitInstructions();
    while (inst != exitGame){
        switch (inst){
            case 0: return;
            case 1: printMaxBoard(); break;
            case 2: printCycle(); break;
            case 3: /*invalidInstructionError();*/break;
            default: break;
        }
        inst = exitInstructions();
    }
}

//ERRORHANDLING

int main(){
    //input instructions and simulate LIFE
    gameOfLife();
    
    //Show statistics and info
    showStatistics();

    return 0;
}