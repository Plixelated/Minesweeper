// --------------------------------
// application: minesweeper
// file: cli.c
// Author: Mia Fraser
// Date: 11/27/2023
// --------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// ----------------------------------
// global definitions
#define MAXTOKENCOUNT 20
#define MAXTOKENLENGTH 20
#define MAXLINELENGTH 400

char line[100], linecopy[100];
char tokens[5][20];
char* p;
int tokencount;

int rows;
int cols;
int mines;
int cellcount;

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
void init_cells(cell *c){
    c->position = cellcount;
    c->adjcount = 0;
    c->mined = 0;
    c->covered = 1;
    c->flagged = 0;
}

void display_cell(cell *c){
    if (c->flagged == 1) printf("P      ");
    else if (c-> covered == 1) printf("■ "); //printf("%d      ", c->position);
}

void command_new(int r, int c, int m){
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
}

void command_show(void){
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            display_cell(&board[i][j]);
        }
        printf("\n");
    }
}

int processcommand(char tokens[][MAXTOKENLENGTH], int tokencount) {

    int rows = atoi(tokens[1]);
    int cols = atoi(tokens[2]);
    int mines = atoi(tokens[3]);
    
    if (strcmp(tokens[0],"new") == 0) {
        command_new(rows, cols, mines);
    }
    else if (strcmp(tokens[0],"show") == 0) {
        command_show();
    }
    else if (strcmp(tokens[0],"quit") == 0) {
        return 0;
    }
    else{
        printf("Invalid entry, please try again\n");
    }
    return 1;
}

int rungame(void) {
    char line[MAXLINELENGTH];
    char tokens[MAXTOKENCOUNT][MAXTOKENLENGTH];

    while (1) {
        int tokencount;
        int result;
        printf(">> ");
        getLine(line, MAXLINELENGTH);
        gettokens(line, tokens, &tokencount);
        result = processcommand(tokens, tokencount);

        if (result == 0) break;
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
