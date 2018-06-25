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
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define THREADCOUNT 11

int read_grid(int (*grid)[9], int FILE *input);

typedef struct
{
    int row;
    int col;
} parameters;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Please specify the input filename.\n");
        return 1;
    }

    int grid[9][9];

    FILE *input = fopen(argv[1], "r");

    parameters *data[9];
    int i = 0;

    // create grid object
    for (int col = 0; col < 9; col += 3)
    {
        for (int row = 0; row < 9; row += 3)
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

    // validate grid
    pthread_t tid[THREADCOUNT];
    
}


/*
    PURPOSE: Reads sudoku grid from file
    INPUT:  grid    Location to store the grid to
            input   File to read the grid from
    OUTPUT: 0       if success
            1       if failure
*/
int read_grid(int (*grid)[9], int FILE *input)
{
    char val;
    int r, c, valCount;

    while(fread(&val, 1, 1, input) > 0 && valCount < 81)
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