#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// the board can be random or read from the file (specification 2.2)
typedef enum boardType
{
    ReadFromFile,
    Randomized,
    Error
} boardType;

typedef struct Board
{
    int width;
    int height;
    char **grid;
} Board;

typedef struct BoardList
{
    Board *board;
    struct BoardList *next;
} BoardList;

/// THESE ARE THE SMALL HELPER FUNCTIONS FOR READING THE INSTRUCTIONS AND THE SIMULATION
// randomizes the board
void fillBoardRandom(Board *board)
{
    srand(time(NULL));
    int r = rand();
    for (int i = 0; i < board->height; i++)
    {
        for (int j = 0; j < board->width; j++)
        {
            if (r % 5 != 0)
            {
                (board->grid)[i][j] = ' ';
                printf("%c", (board->grid)[i][j]);
            }
            else
            {
                (board->grid)[i][j] = '@';
            }
            r = rand();
        }
    }
}
// inputs the board from the file
int fillBoardFromFile(Board *board, FILE *fp)
{
    int status = 1;
    char curr = ' ';
    if (fp == NULL)
    {
        printf("could not find file, please try again! \n");
        return 1;
    }
    // read
    for (int i = 0; i < board->height; i++)
    {
        for (int j = 0; j < board->width; j++)
        {
            curr = fgetc(fp);
            if (curr == ' ')
                (board->grid)[i][j] = ' ';
            else if (curr == '@')
                (board->grid)[i][j] = '@';
            else
                j--;
        }
    }
    status = fclose(fp);
    if (status != 0)
        return 1;
    return 0;
}
// prints the board
void printBoard(Board *board)
{
    printf("\n");
    for (int i = 0; i < board->width + 2; i++)
    {
        printf("-");
    }
    printf("\n");
    for (int i = 0; i < board->height; i++)
    {
        printf("|");
        for (int j = 0; j < board->width; j++)
        {
            if ((board->grid)[i][j] == ' ')
            {
                printf(" ");
            }
            else if ((board->grid)[i][j] == '@')
            {
                printf("▓");
            }
        }
        printf("|\n");
    }
    for (int i = 0; i < board->width + 2; i++)
    {
        printf("-");
    }
}
// saves current board to the file
int saveBoardInFile(Board *board, FILE *fp)
{
    if (fp == NULL)
    {
        printf("action could not be saved!!");
        return 0;
    }
    int status = 1;
    for (int i = 0; i < board->height; i++)
    {
        for (int j = 0; j < board->width; j++)
        {
            fputc((board->grid)[i][j], fp);
        }
        fputc('\n', fp);
    }
    fputc('\n', fp);
    status = fclose(fp);
    if (status != 0)
        return 1;
    return 0;
}
// calculates the next state of the cell
char CellNextState(char current, int neighbourCount)
{
    return ((neighbourCount == 2 && current == '@') || neighbourCount == 3) ? '@' : ' ';
}
// moves on to the next state
void nextBoard(Board *board)
{
    int neighbourCount = 0;
    // making a temporary board to store next states
    Board *temp;
    temp = (Board *)malloc(sizeof(Board));
    temp->grid = (char **)malloc((board->height) * sizeof(char *));
    for (int i = 0; i < board->height; i++)
    {
        (temp->grid)[i] = (char *)malloc(board->width * sizeof(char *));
    }

    for (int i = 0; i < board->height; i++)
    {
        for (int j = 0; j < board->width; j++)
        {
            if (i > 0 && j > 0)
                neighbourCount += (board->grid)[i - 1][j - 1] == '@';
            if (i < (board->height - 1) && j < (board->width - 1))
                neighbourCount += (board->grid)[i + 1][j + 1] == '@';
            if (i > 0 && j < (board->width - 1))
                neighbourCount += (board->grid)[i - 1][j + 1] == '@';
            if (i < (board->height - 1) && j > 0)
                neighbourCount += (board->grid)[i + 1][j - 1] == '@';
            if (i > 0)
                neighbourCount += (board->grid)[i - 1][j] == '@';
            if (j > 0)
                neighbourCount += (board->grid)[i][j - 1] == '@';
            if (i < (board->height - 1))
                neighbourCount += (board->grid)[i + 1][j] == '@';
            if (j < (board->width - 1))
                neighbourCount += (board->grid)[i][j + 1] == '@';
            (temp->grid)[i][j] = CellNextState((board->grid)[i][j], neighbourCount);
            neighbourCount = 0;
        }
    }
    for (int i = 0; i < board->height; i++)
    {
        for (int j = 0; j < board->width; j++)
        {
            (board->grid)[i][j] = (temp->grid)[i][j];
        }
    }
}
// counts the cells that are alive
int aliveCellCount(Board *board)
{
    int aliveCellsCount = 0;
    for (int i = 0; i < board->height; i++)
    {
        for (int j = 0; j < board->width; j++)
        {
            aliveCellsCount += ((board->grid)[i][j] == '@');
        }
    }
    return aliveCellsCount;
}

/// THESE ARE THE BIG HELPER FUNCTIONS FOR READING THE INSTRUCTIONS AND THE SIMULATION
// displays the main rules in the console
void displayRules()
{
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
// compares the boards
int boardsAreEqual(Board *board1, Board *board2)
{
    if (board1->width != board2->width || board1->height != board2->height || aliveCellCount(board1) != aliveCellCount(board2))
    {
        return 0;
    }
    for (int i = 0; i < board1->height; i++)
    {
        for (int j = 0; j < board1->width; j++)
        {
            if ((board1->grid)[i][j] != (board2->grid)[i][j])
            {
                return 0;
            }
        }
    }
    return 1;
}
// detects the cycle (if it exists) returns the size and writes it in a file
int cycleDetected(BoardList *head, Board *currentBoard)
{
    int len = 0;
    BoardList *lst = head;
    while (lst->next != NULL && lst->next->next != NULL)
    {
        if (boardsAreEqual(lst->board, currentBoard))
        {
            // save the whole cycle in the cycle.txt file
            FILE *fp;
            while (lst->next != NULL)
            {
                len++;
                fp = fopen("cycle.txt", "a");
                saveBoardInFile(lst->board, fp);
                lst = lst->next;
            }
            fp = fopen("cycle.txt", "a");
            fclose(fp);
            return len;
        }
        lst = lst->next;
    }
    return len;
}
// asks the user for the instructions
boardType ChooseStartInstructions(Board *board)
{
    int a = 0, b = 0;
    char c;
    printf("First, input the width of your board: ");
    scanf(" %d", &a);
    while (a <= 0 || a >= 90)
    {
        printf("Invalid input! Please try again...\nRemember, 0<width<90\n");
        scanf(" %d", &a);
    }

    printf("Next, input the height of your board: ");
    scanf(" %d", &b);
    while (b <= 0 || b >= 35)
    {
        printf("Invalid input! Please try again...\n Remember, 0<height<35\n");
        scanf(" %d", &b);
    }
    board->width = a;
    board->height = b;
    printf("\nThen, write F if you have already prepared the initial state of your cells in the File, or write R if you want to view a random Game of Life: ");
    scanf(" %c", &c);
    while (c != 'f' && c != 'F' && c != 'r' && c != 'R')
    {
        printf("Invalid input! Please try again...\n");
        scanf(" %c", &c);
    }
    return (c == 'F' || c == 'f') ? ReadFromFile : (c == 'R' || c == 'r') ? Randomized
                                                                          : Error;
}
// fill the board according to the instructions
int fillBoard(Board *board, boardType instruction)
{
    FILE *fp;
    if (instruction == ReadFromFile)
    {
        fp = fopen("input_board.txt", "r");
        if (fp == NULL)
        {
            printf("could not read from the file, please make sure you have correctly inputed the board!");
            return 0;
        }
        printf("The board read from your file: \n");
        fillBoardFromFile(board, fp);
    }
    else if (instruction == Randomized)
    {
        printf("The randomized world:\n");
        fillBoardRandom(board);
        fp = fopen("input_board.txt", "w");
        saveBoardInFile(board, fp);
    }
    return 1;
}
// adds board to the board list
void addBoardToList(Board *newBoard, BoardList **head)
{
    // allocating the memory for the node
    BoardList *node;
    node = (BoardList *)malloc(sizeof(BoardList));
    node->board = (Board *)malloc(sizeof(Board));
    node->board->grid = (char **)malloc(newBoard->height * sizeof(char *));
    for (int i = 0; i < newBoard->height; i++)
    {
        (node->board->grid)[i] = (char *)malloc(newBoard->width * sizeof(char));
    }
    node->board = newBoard;
    node->next = NULL;
    // add the node to the list
    if (*head == NULL)
    {
        *head = node;
    }
    else
    {
        BoardList *curr = *head;
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        curr->next = node;
    }
}

void printAllBoards(BoardList *lst)
{
    BoardList* temp = lst;
    while (temp != NULL)
    {
        printBoard(temp->board);
        temp = temp->next;
    }
}
// frees the boardList
void freeBoardList(BoardList *head)
{
    BoardList *temp = head;
    while (temp->next != NULL)
    {
        BoardList *nxt = temp->next;
        // for (int i = 0; i < (temp->board->height); i++)
        // {
        //     free((temp->board->grid)[i]);
        // }
        // free(temp->board->grid);
        //free((temp->board));
        free(temp);
        temp = nxt;
    }
}

// simulates the process
int simulation(Board *board)
{
    int i = 0, maxAlives = 0, cycle = 0;
    BoardList *head= NULL;
    // to clear the old archives
    FILE *fp;
    fp = fopen("archive.txt", "w");
    fclose(fp);
    fp = fopen("cycle.txt", "w");
    fclose(fp);

    int alives = aliveCellCount(board);
    while (i < 30)
    {
        addBoardToList(board, &head);
        cycle = cycleDetected(head, board);
        printBoard(board);
        fp = fopen("archive.txt", "a");
        saveBoardInFile(board, fp);
        if (alives > maxAlives)
        {
            maxAlives = alives;
            fp = fopen("maxAlives.txt", "w");
            saveBoardInFile(board, fp);
        }
        else if (alives == maxAlives && !cycle)
        {
            fp = fopen("maxAlives.txt", "a");
            saveBoardInFile(board, fp);
        }
        printf("\n alive cells: %d\n", alives);
        // if (cycle)
        // {
        //     freeBoardList(head);
        //     return cycle;
        // }
        if(alives<=0){
            freeBoardList(head);
            return cycle;
        }
        sleep(1);
        nextBoard(board);
        alives = aliveCellCount(board);
        i++;
    }
    freeBoardList(head);
    return 0;
}

// reading the instructions and simulating the Game of Life
int gameOfLife()
{
    Board *board;
    board = (Board *)malloc(sizeof(Board));
    // show user the main rules
    displayRules();
    // ask for instructions
    boardType instruction = ChooseStartInstructions(board);
    board->grid = (char **)malloc(board->height * sizeof(char *));
    for (int i = 0; i < board->height; i++)
    {
        (board->grid)[i] = (char *)malloc(board->width * sizeof(char));
    }
    // fill in the board according to these instructions
    fillBoard(board, instruction);
    // simulate life
    int cycle = simulation(board);

    // clear the memory
    for (int i = 0; i < board->height; i++)
    {
        free((board->grid)[i]);
    }
    free(board);
    return cycle;
}

/// THESE ARE THE SMALL HELPER FUNCTIONS FOR DISPLAYING THE STATISTICS
int printFile(char *fileName)
{
    FILE *fp;
    fp = fopen(fileName, "r");
    int status = 1;
    char c = ' ';
    if (fp == NULL)
    {
        printf("could not find file, please try again! \n");
        return 1;
    }

    // read
    c = fgetc(fp);
    while (c != EOF)
    {
        if (c == '@')
        {
            printf("▓");
        }
        else
        {
            printf("%c", c);
        }
        c = fgetc(fp);
    }

    status = fclose(fp);
    if (status != 0)
        return 1;
    return 0;
}
// SHOWING THE STATISTICS OF THE GAME
void showStatistics(int cycle)
{
    char menuCode = '0';
    printf("\n\nPhew... This was your simulation! \n");
    while (1)
    {
        printf("Now, What do you want to do with this information? Press the corresponding key. ");
        printf("\nPress 1 to view the board with the most alive cells");
        printf("\nPress 2 to see a cycle in your simulation, if there was one!");
        printf("\nPress 0 to exit\n");
        scanf(" %c", &menuCode);
        switch (menuCode)
        {
        case '0':
            return;
        case '1':
        {
            printFile("maxAlives.txt");
            break;
        }
        case '2':
        {
            printf("%d\n", cycle);
            if (cycle)
            {
                printFile("cycle.txt");
            }
            else
            {
                printf("Sorry! Could not detect cycle in your program!\n\n");
            }
            break;
        }
        default:
        {
            printf("The instruction is not valid, please try again!\n");
            break;
        }
        }
    }
}

int main()
{
    // input instructions and simulate LIFE
    int cycle = gameOfLife();

    // Show statistics and info
    showStatistics(cycle);

    return 0;
}