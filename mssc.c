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

// required libraries 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

// define grid parameters
typedef struct{
	int row;
	int col;
	int (*sudoku_grid)[9];
}parameters;

// error related data
enum ERRORS { NO_INFILE, MALLOC_FAILED, READ_FAILED };
char *error_diag[] = 
{
    (char *) "\nUsage: executable-name input-file\n",
    (char *) "\nError - malloc failed",
    (char *) "\nError occured when reading the grid from the file."
};

// function declarations
void *check_subgrid(void *);
void *check_row(void *);
void *check_col(void *);
int read_grid(int (*grid)[9], int, char *);
void error(enum ERRORS);
void help_text(void);

#define THREADCOUNT 11

int main(int argc, char *argv[])
{
    /*
    PURPOSE: checks validity of sudoku solution
    INPUT:  infile: file to read grid from
    OUTPUT: 0       if success
            1       if failure
    */

    // exit if input file hasn't been provided
	if(argc < 2){
		error(NO_INFILE);
		exit(EXIT_FAILURE);
	}

    // print startup text
    help_text();

    // initialize grid
	int sudoku_grid[9][9];

	// Initalize parameters for subgrid evaluation threads
	parameters *data[9];
	int row, col, i = 0;
	for(row = 0; row < 9; row += 3)
	{
		for(col = 0; col < 9; col += 3, ++i)
		{
			data[i] = (parameters *)malloc(sizeof(parameters));
			if(data[i] == NULL)
            {
				error(MALLOC_FAILED);
				exit(EXIT_FAILURE);
			}
			data[i]->row = row;
			data[i]->col = col;
			data[i]->sudoku_grid = sudoku_grid;
		}
	}

	// Validate all sudoku grids in file
	pthread_t tid[THREADCOUNT];
	int g, p, j, h, retCode, check_flag = 0, t_status[THREADCOUNT];
    if(read_grid(sudoku_grid, g, argv[1]))
    {
        error(READ_FAILED);
        exit(EXIT_FAILURE);
    }

    // Create threads for subgrid validation 
    for(p = 0; p < 9; ++p){
        if((retCode = pthread_create(&tid[p], NULL, check_subgrid, (void *)data[p])))
        {
            fprintf(stderr, "Error - pthread_create() return code: %d\n", retCode);
            exit(EXIT_FAILURE);
        }
    }


    // Create threads for row and column validation
    if((retCode = pthread_create(&tid[9], NULL, check_row, (void *)data[0])))
    {
        fprintf(stderr, "Error - pthread_create() return code: %d\n", retCode);
        exit(EXIT_FAILURE);
    }
    if((retCode = pthread_create(&tid[10], NULL, check_col, (void *)data[0])))
    {
        fprintf(stderr, "Error - pthread_create() return code: %d\n", retCode);
        exit(EXIT_FAILURE);
    }

    // Join all threads so main waits until the threads finish
    for(j = 0; j < THREADCOUNT; ++j)
    {
        if((retCode = pthread_join(tid[j], (void *)&t_status[j])))
        {
            fprintf(stderr, "Error - pthread_join() return code: %d\n", retCode);
            exit(EXIT_FAILURE);
        }
    }

    // Check the status returned by each thread
    for(h = 0; h < THREADCOUNT; ++h)
    {
        if(t_status[h])
        {
            check_flag = 1;
            break;
        }
    }

    if(check_flag)
    {
        printf("Sudoku Puzzle Invalid\n");
    }else
    {
        printf("Sudoku Puzzle Valid\n");
    }
    check_flag = 0;

	// Free memory and close the file
	int k;
	for(k = 0; k < 9; ++k)
    {
		free(data[k]);
	}

	return 0;
}

void *check_subgrid(void *data)
{
    /*
    PURPOSE: checks validity of sudoku subgrid
    INPUT:  data    starting coordinates of subgrid to check
    OUTPUT: 1       if success
            0       if failure
    */
	int digit_check[10] = {0};
	parameters *params = (parameters *)data;
	int row, col;

	for(row= params->row; row< params->row + 3; ++row)
    {
		for(col = params->col; col < params->col + 3; ++col)
        {
			if(digit_check[params->sudoku_grid[row][col]] == 1)
            {
				return (void *)-1; // Invalid sudoku subgrid
			}
			digit_check[params->sudoku_grid[row][col]] = 1;
		}
	}
	return (void *)0; // Valid sudoku subgrid
}

void *check_row(void *data)
{
    /*
    PURPOSE: checks validity of sudoku row
    INPUT:  data    starting coordinates of row to check
    OUTPUT: 1       if success
            0       if failure
    */
	int digit_check[10] = {0};
	parameters *params = (parameters *)data;
	int row, col;

	for(row= 0; row< 9; ++row)
    {
		for(col = 0; col < 9; ++col)
        {
			if(digit_check[params->sudoku_grid[row][col]] == 1)
            {
				return (void *)-1; // Invalid sudoku rows
			}
			digit_check[params->sudoku_grid[row][col]] = 1;
		}
		// Reinitalize check array for next row
		memset(digit_check, 0, sizeof(int)*10);
	}
	return (void *)0; // Valid sudoku rows
}

void *check_col(void *data)
{
    /*
    PURPOSE: checks validity of sudoku column
    INPUT:  d       starting coordinates of column to check
    OUTPUT: 1       if success
            0       if failure
    */
	int digit_check[10] = {0};
	parameters *params = (parameters *)data;
	int row, col;

	for(row= 0; row< 9; ++row)
    {
		for(col = 0; col < 9; ++col)
        {
			if(digit_check[params->sudoku_grid[col][row]] == 1)
            { // Ensure the digit doesn't appear more than once in the subgrid
				return (void *)-1; // Invalid sudoku columns
			}
			digit_check[params->sudoku_grid[col][row]] = 1;
		}
		// Reinitalize check array for next column
		memset(digit_check, 0, sizeof(int)*10);
	}
	return (void *)0; // Valid sudoku columns
}


int read_grid(int (*grid)[9], int grid_no, char *infile)
{
    /*
    PURPOSE: Reads sudoku grid from file
    INPUT:  grid    Location to store the grid to
            input   File to read the grid from
    OUTPUT: 1       if success
            0       if failure
    */
	char entry;
	int row = 0, col = 0;
	FILE *file = fopen(infile, "r");

	while((fread(&entry, 1, 1, file)) > 0)
    {
		if(entry != '\n' && entry != ' ' && entry != '\r')
        { // Ignore newline
			if(isdigit(entry))
            {
				grid[row][col] = entry - '0'; // Store integer representation
				++col;
				if(col == 9){
					col = 0;
					++row;
				}
			}
			else{ // A non-digit character was read
				return -1;
			}
		}
	}

    // close file & exit
	fclose(file);
	return 0;
}

void error(enum ERRORS err)
{
    /*
    PURPOSE: prints errors to screen then exits
    INPUT:  integer based on ERRORS enum
    OUTPUT:
    */

    /* Display diagnostic and abort */
    printf("\n%s\n", error_diag[err]);
    exit(EXIT_FAILURE);
}

void help_text(void)
{
    /*
    PURPOSE: prints initial help text
    INPUT:
    OUTPUT:
    */

    printf( "\n\t * ------------------------------------- *\n"
            "\t *       SUDOKU SOLVER VERSION 0.4       *\n"
            "\t *       FINLAY MILLER B00675696         *\n"
            "\t *   USAGE: executable-name input-file   *\n"
            "\t *   INPUT FILE MUST HAVE FORMAT BELOW   *\n"
            "\t *                                       *\n"
            "\t *           N N N N N N N N N           *\n"
            "\t *           N N N N N N N N N           *\n"
            "\t *           N N N N N N N N N           *\n"
            "\t *           N N N N N N N N N           *\n"
            "\t *           N N N N N N N N N           *\n"
            "\t *           N N N N N N N N N           *\n"
            "\t *           N N N N N N N N N           *\n"
            "\t *           N N N N N N N N N           *\n"
            "\t *           N N N N N N N N N           *\n"
            "\t *                                       *\n"
            "\t *   WHERE EACH N IS AN INTEGER BETWEEN  *\n"
            "\t *   ZERO AND NINE                       *\n"
            "\t * ------------------------------------- *\n\n");
    return;
}