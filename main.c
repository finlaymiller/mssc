/*
    - CSCI3120 Operating Systems Assignment 2
    - Multithreaded Sudoku Solution Checker
    - This program takes a 9x9 grid of numbers 1-9 and assesses whether or not
      they are a valid Sudoku solution. It implements multithreading as
      described below:
        * One thread to check each column and confirm that it contains one of
        each digit
        * One thread to check each row and confirm that it contains one of
        each digit
        * Nine threads to check each of the 3x3 sub-grids and confirm that they
        contain each digit
    - Written by Finlay Miller B00675696  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define THREADCOUNT 11

int read_grid(int (*grid)[9], char *);
void *check_subgrid(void *);
void *check_row(void *);
void *check_col(void *);

typedef struct
{
    int row;
    int col;
    int *grid[9];
} parameters;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Please specify the input filename.\n");
        return 1;
    }

    int grid[9][9], threadStatus[THREADCOUNT], check = 0;

    parameters *data[9];
    int i = 0;

    // create grid object
    for (int row = 0; row < 9; row += 3)
    {   
        for (int col = 0; col < 9; col += 3)
        {
            // create grid
            data[i] = (parameters *) malloc(sizeof(parameters));

            // break if grid creation failed
            if (data[i] == NULL)
            {
                printf("MALLOC failed.\n");
                return 1;
            }

            data[i]->col = col;
            data[i]->row = row;
        }
    }

    // fill grid object
    if(read_grid(grid, argv[1]))
    {
        printf("Failed to read grid.\n");
        return 1;
    }

    pthread_t tid[THREADCOUNT];

    // create subgrid check thread
    for(int g = 0; g < 9; g++)
    {
        if(pthread_create(&tid[g], NULL, check_subgrid, (void *)data[0]))
            printf("Error creating subgrid check thread.\n");
    }

    // create row check thread
    if(pthread_create(&tid[9], NULL, check_row, (void *)data[0]))
        printf("Error creating row check thread.\n");

    // create column check thread
    if(pthread_create(&tid[10], NULL, check_col, (void *)data[0]))
        printf("Error creating column check thread.\n");

    // check return status of each thread
    for (int t = 0; t < THREADCOUNT; t++)
    {
        if(threadStatus[t])
        {
            check = 1;
            break;
        }
    }
    
    if (check)
    {
        printf("Sudoku Solution Invalid.\n");
    }
    else
    {
        printf("Sudoku Solution Valid.\n");
    }
    

}

/*
    PURPOSE: checks validity of sudoku subgrid
    INPUT:  d       starting coordinates of subgrid to check
    OUTPUT: 1       if success
            0       if failure
*/
void *check_subgrid(void * d)
{
    int foundDigits[10] = {0};
    parameters *params = (parameters *) d;

    // check each value in subgrid
    for (int r = params->row; r < params->row + 3; r++)
    {
        for (int c = params->col; c < params->col + 3; c++)
        {
            // check to see if the current digit has been found before
            if (foundDigits[params->grid[r][c]] == 1)
                // invalid subgrid
                return (void *) 0;
            
            // add it if it hasn't 
            foundDigits[params->grid[r][c]] = 1;
        }
    }
    // valid subgrid
    return 1;
}

/*
    PURPOSE: Reads sudoku grid from file
    INPUT:  grid    Location to store the grid to
            input   File to read the grid from
    OUTPUT: 0       if success
            1       if failure
*/
int read_grid(int (*grid)[9], char *file)
{
    char val;
    int r, c, valCount;
    FILE *f = fopen(file, "r");

    while(fread(&val, 1, 1, f) > 0 && valCount < 81)
    {
        if (val != '\n')
        {
            // make sure that value being read is a digit
            if (!isdigit(val)) return 1;

            // store value as integer
            grid[r][c] = val - '0';
            // move to next column
            c++;
            // if last column, move back to first col
            // next row
            if (c == 9)
            {
                c = 0;
                r++;
            }
            valCount ++;
        }
        else return 1;
    }

    return 0;
}

/*
    PURPOSE: checks validity of sudoku subgrid
    INPUT:  d       starting coordinates of subgrid to check
    OUTPUT: 1       if success
            0       if failure
*/
void *check_subgrid(void *d)
{
    int foundDigits[10] = {0};
    parameters *params = (parameters *) d;

    // check each value in subgrid
    for (int r = params->row; r < params->row + 3; r++)
    {
        for (int c = params->col; c < params->col + 3; c++)
        {
            // check to see if the current digit has been found before
            if (foundDigits[params->grid[r][c]] == 1)
                // invalid subgrid
                return (void *) 0;
            
            // add it if it hasn't 
            foundDigits[params->grid[r][c]] = 1;
        }
    }
    // valid subgrid
    return 1;
}

/*
    PURPOSE: checks validity of sudoku row
    INPUT:  d       starting coordinates of row to check
    OUTPUT: 1       if success
            0       if failure
*/
void *check_row(void *d)
{
    int foundDigits[10] = {0};
    parameters *params = (parameters *) d;

    // check each value in subgrid
    for (int r = 0; r < 9; r++)
    {
        for (int c = 0; c < 9; c++)
        {
            // check to see if the current digit has been found before
            if (foundDigits[params->grid[r][c]] == 1)
                // invalid subgrid
                return (void *) 0;
            
            // add it if it hasn't 
            foundDigits[params->grid[r][c]] = 1;
        }
        // reset check array for next row
        memset(foundDigits, 0, sizeof(int)*10);
    }
    // valid subgrid
    return 1;
}

/*
    PURPOSE: checks validity of sudoku column
    INPUT:  d       starting coordinates of column to check
    OUTPUT: 1       if success
            0       if failure
*/
void *check_col(void *d)
{
    int foundDigits[10] = {0};
    parameters *params = (parameters *) d;

    // check each value in subgrid
    for (int c = 0; c < 9; c++)
    {
        for (int r = 0; r < 9; r++)
        {
            // check to see if the current digit has been found before
            if (foundDigits[params->grid[r][c]] == 1)
                // invalid subgrid
                return (void *) 0;
            
            // add it if it hasn't 
            foundDigits[params->grid[r][c]] = 1;
        }
        // reset check array for next row
        memset(foundDigits, 0, sizeof(int)*10);
    }
    // valid subgrid
    return 1;
}