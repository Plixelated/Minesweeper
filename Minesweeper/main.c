// --------------------------------
// application: minesweeper
// file: cli.c
// Author: Mia Fraser
// Date: 11/27/2023
// --------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
// ----------------------------------
// global definitions
#define MAXTOKENCOUNT 20
#define MAXTOKENLENGTH 20
#define MAXLINELENGTH 400
#define LEGALCMDCOUNT 8
//Input Verification
char *legalcommands[] = {"new", "show", "quit", "uncover", "flag", "unflag", "debug:cover", "debug:uncover"};
int legalcommandargs[] = {3, 0 , 0, 2, 2, 2, 0 , 0};

char line[100], linecopy[100];
char tokens[5][20];
char* p;
int tokencount;

int rows;
int cols;
int mines;
int cellcount;

int win_lose;
//1 win, 0 lose, -1 continue

// sizing constants

// definition of “cell” struct
struct cell {
    int position;
    int adjcount;
    int mined;
    int covered;
    int flagged;
};
typedef struct cell cell;

cell **board;

// function prototypes
// for all functions defined
// in this file

// ----------------------------------
// ---------------------------------
// input processing functions


int getLine(char line[], int maxlinelen) {
    fgets(line, maxlinelen, stdin);
    int linelen = strlen(line);
    if (line[linelen - 1] == '\n') line[linelen - 1] = '\0';
    return 0;
}

int gettokens(char line[], char tokens[][MAXTOKENLENGTH], int* count) {
    strcpy(linecopy, line);
    p = strtok(linecopy, " ");
    tokencount = 0;

    while (p != NULL) {
        strcpy(tokens[tokencount], p);

        p = strtok(NULL, " ");
        tokencount++;
    }
    return 0;
}

// ---------------------------------
// ---------------------------------

int get_random(int range){
    return ((int)floor((float)range*rand()/RAND_MAX))%range;
}

void init_cells(cell *c){
    c->position = cellcount;
    c->adjcount = 0;
    c->mined = 0;
    c->covered = 1;
    c->flagged = 0;
}

void cell_adjacency(cell current, int r, int c){
    int neighborcount = 8;
    int rowneighbors[] = {-1, -1, 0, 1, 1,  1,  0, -1};
    int colneighbors[] = {0,   1, 1, 1, 0, -1, -1, -1};
    int minecount = current.adjcount;
    
    for (int d = 0; d < neighborcount; d++){
        int rn = r + rowneighbors[d];
        int cn = c + colneighbors[d];
        
        if (0 <= rn && rn < rows && 0 <= cn && cn < cols){
            if (board[rn][cn].mined == 1){
                minecount++;
            }
        }
    }
    board[r][c].adjcount = minecount;
}

void display_cell(cell *c){
    if (c->flagged == 1) printf("%3s","P");
    else if (c-> covered == 1) printf("%3s", "/");
    else if (c-> mined == 1) printf("%3s", "*");
    else if (c -> adjcount > 0) printf("%3d", c->adjcount);
    else printf("%3s",".");
}

void command_new(int r, int c, int m){
    if (board != NULL) board = NULL;
    
    rows = r;
    cols = c;
    mines = m;
    
    //allocate iniital "row vector"
    board = (cell**) malloc(sizeof(cell *)* rows);
    
    //for each element of row vector, allocate a row
    for (int i = 0; i < rows; i ++){
        board[i] = (cell *) malloc(sizeof(cell) * cols);
    }
    
    //for each cell, call init_cell() to set it's inital values
    for (int i = 0; i < rows; i ++){
        for (int j = 0; j < cols; j++){
            init_cells(&board[i][j]);
            cellcount++;
        }
    }
    
    //ADD MINES
    //pack mines
    for (int p = 0; p < m; p++){
        int r_pack = p/cols;
        int c_pack = p%cols;
        board[r_pack][c_pack].mined = 1;
    }
    //shuffle mines
    int shuffle = m*50;
    for(int s = 0; s < shuffle; s++){
        //Get first cell
        int r1 = get_random(rows-1);
        int c1 = get_random(cols-1);
        //get second cell
        int r2 = get_random(rows-1);
        int c2 = get_random(cols-1);
        //get mined status
        int x = board[r1][c1].mined;
        int y = board[r2][c2].mined;
        //Swap
        board[r1][c1].mined = y;
        board[r2][c2].mined = x;
        
    }
    
    //CALCULATE CELL ADJACENCY
    for (int i = 0; i < rows; i ++){
        for (int j = 0; j < cols; j++){
            cell_adjacency(board[i][j], i, j);
        }
    }
    
}

void command_show(void){
    for (int i = -1; i < rows; i++){
        for (int j = -1; j < cols; j++){
            if (i == -1){
                printf("%3d", j+1);
            }
            else if(j == -1){
                printf("%3d", i+1);
                if (i+1 == 0)
                    printf("%3s", " ");
            }
            else{
                display_cell(&board[i][j]);
            }
        }
        printf("\n");
    }
}

int check_for_win(void){
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < cols; c++){
            if(board[r][c].mined == 1 && board[r][c].flagged == 0)
                return -1; //false
            else if (board[r][c].mined == 0 && board[r][c].covered == 1)
                return -1; //false
        }
    }
    printf("Congratulations! You Won!\n");
    command_show();
    return 1; //true
}


void command_uncover(int r, int c){
    if (r < 0 || c < 0){
        printf("Invalid Location Please Try Again\n");
        return;
    }
    board[r][c].covered = 0;
    
    if (board[r][c].mined == 1){
        win_lose = 0;
        printf("Oh No! You Uncovered a Mine!\nTry Again.\n");
        return;
    }

    if (board[r][c].adjcount == 0){
        //Get Neighbors
        int rowneighbors[] = {-1, -1, 0, 1, 1,  1,  0, -1};
        int colneighbors[] = {0,   1, 1, 1, 0, -1, -1, -1};
        //For each neighbor
        int neighborcount = 8;
        
        for (int d = 0; d < neighborcount; d++){
            int next_row = r + rowneighbors[d];
            int next_col = c + colneighbors[d];
            //If there is a valid index
            if (0 <= next_row && next_row < rows && 0 <= next_col && next_col < cols){ //&& rn < neighborcount && cn < neighborcount ){
                //If it is covered
                if (board[next_row][next_col].covered == 1){
                    //Uncover it
                    command_uncover(next_row, next_col);//, ++rn, ++cn);
                }
            }
        }
    }
    
    win_lose = check_for_win();
}

void command_flag(int r, int c){
    win_lose = check_for_win();
    
    if (board[r][c].flagged == 0){
        board[r][c].flagged = 1;
    }
    else{
        printf("Cell Already Flagged\n");
    }
    
}

void command_unflag(int r, int c){
    if (board[r][c].flagged == 1){
        board[r][c].flagged = 0;
    }
    else{
        printf("Cell Is Not Flagged\n");
    }
}

void command_uncoverall(void){
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < cols; c++){
            board[r][c].covered = 0;
        }
    }
}

void command_coverall(void){
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < cols; c++){
            board[r][c].covered = 1;
        }
    }
}

int input_verification(char tokens[][MAXTOKENLENGTH], int tokencount){
    int cmdlegal = -1;
    for(int i = 0; i < LEGALCMDCOUNT; i++){
        if (strcmp(tokens[0], legalcommands[i]) == 0){
            cmdlegal = i;
            break;
        }
    }
    if(cmdlegal >= 0 && tokencount-1 == legalcommandargs[cmdlegal]){
        return 1;
    }
    else{
        printf("Invalid entry, please try again\n");
        return 0;
    }
}

int processcommand(char tokens[][MAXTOKENLENGTH], int tokencount) {
    
    int rows = atoi(tokens[1]);
    int cols = atoi(tokens[2]);
    int mines = atoi(tokens[3]);
    
    if (strcmp(tokens[0],"new") == 0) { //NEW BOARD
        command_new(rows, cols, mines);
        if (win_lose  != -1)
            win_lose = -1;
    }
    else if (strcmp(tokens[0],"show") == 0 && win_lose == -1) { //SHOW BOARD
        command_show();
    }
    else if (strcmp(tokens[0],"quit") == 0) { //QUIT GAME
        return -1; //quit
    }
    else if (strcmp(tokens[0],"uncover") == 0 && win_lose == -1) { //UNCOVER CELL
        command_uncover(rows-1, cols-1);
    }
    else if (strcmp(tokens[0],"flag") == 0 && win_lose == -1) { //FLAG CELL
        command_flag(rows-1, cols-1);
    }
    else if (strcmp(tokens[0],"unflag") == 0 && win_lose == -1) { //UNFLAC CELL
        command_unflag(rows-1, cols-1);
    }
    else if (strcmp(tokens[0],"debug:cover") == 0) { //COVER ALL CELLS
        command_coverall();
    }
    else if (strcmp(tokens[0],"debug:uncover") == 0) { //UNCOVER ALL CELLS
        command_uncoverall();
    }
    else if (win_lose != -1){
        printf("Start A New Game To Continue.\n");
    }

    return 1;
}

int rungame(void) {
    win_lose = -1;
    char line[MAXLINELENGTH];
    char tokens[MAXTOKENCOUNT][MAXTOKENLENGTH];
    srand(time(0));
    
    while (1) {
        int result;
        int valid;
        printf(">> ");
        getLine(line, MAXLINELENGTH);
        gettokens(line, tokens, &tokencount);
        valid = input_verification(tokens, tokencount);
        if (valid > 0){
            result = processcommand(tokens, tokencount);
        }
        
        if (result == -1) break;
    }
    
    return 1;
}
// ---------------------------------
// ---------------------
// main function

int main(void) {
    rungame();
    return 0;
}
