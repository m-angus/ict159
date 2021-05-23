#include <stdio.h>
#include <string.h>
#include <stdbool.h>

const int SIZE = 15;

//this struct stores the data directly from file
typedef struct customer
{
	char name[SIZE];
	int cash;
} customerType;

//this struct stores the quantity of each coin returned to each customer
typedef struct change
{
	int fifty;
	int twenty;
	int ten;
	int five;
} changeType;

//this struct stores final data after all calculations have been processed
typedef struct output
{
	char name[SIZE];
	int cash;
	int fifty;
	int twenty;
	int ten;
	int five;
} outputType;

//this function fills a struct with data from a file
void Fill_Struct(customerType *nameCash, FILE *coinFile)
{
	int i;
	for(i = 0; i < SIZE; i++)
	{
		fscanf(coinFile, "%s %d", nameCash[i].name, &nameCash[i].cash); //stores strings and ints from file in struct variables
	}	
	return;
}

//this function opens, error checks and closes the file.
int Open_Close_File(int argc, char *argv[], FILE *coinFile, customerType *nameCash)
{
	if(argc == 1) // checks num of CLI arguments
	{
		printf("No file name given!\n"); //error message for not enough CLI arguments 
	}
	else if(argc == 2) // checks num of CLI arguments
	{
		printf("Missing a file name!\n"); //error message for not enough CLI arguments 
	}
	else
	{
		coinFile = fopen(argv[1], "r"); //opens file to be read from 

		if(coinFile == NULL) //checks if file is able to be opened
		{
			perror("Failed to open file"); //error message
			return -1;
		}
		Fill_Struct(nameCash, coinFile); //this function fills a struct with data from a file
	}
	fclose(coinFile); //closes file
	return(0);
}

//this function calculates the coins to be given in change
void Calculate_Change(customerType *nameCash, changeType *change)
{
	int i; 
	int changeHolder;

	for(i = 0; i < SIZE; i++)
	{
		//initialising all elements to zero
		change[i].fifty = 0;
		change[i].twenty = 0;
		change[i].ten = 0;
		change[i].five = 0;
		/*initalising var to store nameCash[i].cash values to do 
		calculations on so that nameCash[i].cash remains intact*/
		changeHolder = nameCash[i].cash; 
		/*these while loops use a counter to calculate the least amount of 
		coins that can be retured for the given change amounts*/
		while(changeHolder >= 50)
		{
			changeHolder = changeHolder - 50;
			change[i].fifty++;	
		}
		while(changeHolder >= 20)
		{
			changeHolder = changeHolder - 20;
			change[i].twenty++;
		}
		while(changeHolder >= 10)
		{
			changeHolder = changeHolder - 10;
			change[i].ten++;
		}
		while(changeHolder >= 5)
		{
			changeHolder = changeHolder - 5;
			change[i].five++;
		}
	}
	return;
}

//this displays the menu, name and coin amounts for each customer the user requests to see, and adds up values for use in output .csv file
int Disp_Menu_Customer(customerType *nameCash, changeType *change, int outputLength, outputType *output)
{
	char option[SIZE];
	int length;
	int i; 
	int j = 0;
	int k = 0;
	bool noName; //var to determine whether input name is included in the txt file
	outputLength = 0;
	do
	{		
		noName = false;
		printf("1. Enter name\n2. Exit\n"); //displays menu options
		if(fgets(option, SIZE, stdin) != NULL) //gets menu option input from user and error checks
		{
		}
		if(*option == '1') //condition for menu option "1. Enter name"
		{
			printf("Write the name here: "); //prompts user for name
			if(fgets(option, SIZE, stdin) != NULL) //gets name input from user and error checks
			{
				length = strlen(option); //used to adjust name input size for comparison in condition statements below
				option[length - 1] = '\0';//as above comment
			}
			for(i = 0; i < SIZE; i++)
			{
				if(strncmp(option, nameCash[i].name, SIZE) == 0)
				{
					outputLength = outputLength + 1; //this var is used to define the length of the arrays used when writing to the .csv file
					noName = true;
					printf("\n\tCustomer:\n"
						   "\t%s %d cents\n\n"
						   "\tChange:\n", nameCash[i].name, nameCash[i].cash); //displays requested data 
					strcpy(output[j].name, nameCash[i].name); //copies the string from customerType nameCash to outputType output (for use in .csv file)
					output[j].cash = nameCash[i].cash;
					if(change[i].fifty > 0)
					{
						printf("\t50 cents: %d\n", change[i].fifty); //displays requested data 
						output[j].fifty = change[i].fifty; //copies the var from changeType change to outputType output (for use in .csv file)
					}
					if(change[i].twenty > 0)
					{
						printf("\t20 cents: %d\n", change[i].twenty); //displays requested data 
						output[j].twenty = change[i].twenty; //copies the var from changeType change to outputType output (for use in .csv file)
					}
					if(change[i].ten > 0)
					{
						printf("\t10 cents: %d\n", change[i].ten); //displays requested data 
						output[j].ten = change[i].ten; //copies the var from changeType change to outputType output (for use in .csv file)
					}
					if(change[i].five > 0)
					{
						printf("\t5 cents: %d\n", change[i].five); //displays requested data 
						output[j].five = change[i].five; //copies the var from changeType change to outputType output (for use in .csv file)
					}
					printf("\n");
				}
			}
			if(noName == false && *option != '2') //error detection for name input
			{
				printf("\nName: %s\nNot Found\n\n", option); //displays error to user
				j--; //decrements counter for output[j].name array (input data list) so that the list lenght only increases for valid names
			}
			j++; //increments counter for output[j].name array (input data list)
		}
	}while(*option != '2');	
	return(outputLength);
}

//this function compares the input lines and removes repeated names and adds total coins
int Add_Common_Names(outputType *output, int outputLength, int repeat)
{
	int i;
	int j;
	int k;
	repeat = 0; //var that stores number of times a name input is repeated 
	int startCash[outputLength];

	for(i = 0; i < outputLength; i++) //loop that initialises var starCash to hold original cash values for each customer 
	{
		startCash[i] = output[i].cash;
	} 
	for(i = 0; i < outputLength; i++) //increments the first name for comparison
	{
		for(j = i + 1; j < outputLength - repeat; j++) //increments the second name to be compared with the first
		{
			if(strncmp(output[i].name, output[j].name, SIZE) == 0) //checks if the first and second names match
			{   
				repeat = repeat + 1; //if there is a match the match counter is incremented
				
				for(k = j; k < outputLength; k++) //increments remaining list for reshuffling 
				{
					strcpy(output[k].name, output[k + 1].name); //shuffles all of the names up one line so that there are no blank lines remaining
				}
				for(k = j; k < outputLength; k++)
				{
					output[k].cash = output[k + 1].cash; //shuffles all of the change totals up one line so that there are no blank lines remaining
				}

				output[i].cash = output[i].cash + startCash[j]; //adds total change for each customer 
				output[i].fifty = output[i].fifty + output[j].fifty; //adds total 50c coins for same customer
				for(k = j; k < outputLength; k++)
				{
					output[k].fifty = output[k + 1].fifty; //shuffles all of the fifty coins up one line so that there are no blank lines remaining
				}
				output[i].twenty = output[i].twenty + output[j].twenty; //adds total 20c coins for same customer
				for(k = j; k < outputLength; k++)
				{
					output[k].twenty = output[k + 1].twenty; //shuffles all of the twenty coins up one line so that there are no blank lines remaining
				}
				output[i].ten = output[i].ten + output[j].ten; //adds total 10c coins for same customer
				for(k = j; k < outputLength; k++)
				{
					output[k].ten = output[k + 1].ten; //shuffles all of the ten coins up one line so that there are no blank lines remaining
				}
				output[i].five = output[i].five + output[j].five; //adds total coins for same customer
				for(k = j; k < outputLength; k++)
				{
					output[k].five = output[k + 1].five; //shuffles all of the five coins up one line so that there are no blank lines remaining
				}
				i--; //These i & j decrement the counter so that
				j--; //no line is skipped during the comparisons
			}
		}
	}
	return(repeat);
}

//this funciton writes to the .csv file
void Write_To_File(FILE *infoFile, char *argv[], outputType *output, int outputLength, int repeat)
{
	int i; //initialised int for loop
	int size = outputLength - repeat; //used to determine number of inputs to write to file (removes number of repeated lines for total lines in output)
	infoFile = fopen(argv[2], "w"); //opens .csv for writing to 
	for(i = 0; i < size; i++) //loop to write data to file
	{
		fprintf(infoFile, "%s,%d,%d,%d,%d,%d\n", output[i].name, output[i].cash, output[i].fifty, output[i].twenty, output[i].ten, output[i].five); //all of the inputs to be included for each line in file
	}
	fclose(infoFile); //closes .csv file
	return;
}

int main(int argc, char *argv[])
{
	FILE *coinFile; //declares file pointer var to be read from 
	FILE *infoFile; //declares file pointer var to be written to
	customerType nameCash[SIZE]; //struct var array for data read from coinFile
	changeType change[SIZE]; //struct var array to store numbers of coin change
	int SIZE_TWO; //initialise for use in num of outputs in .csv file
	int outputLength; //num for total name inputs by user from the menu
	outputType output[outputLength]; //struct var array for data that will be output to .csv file
	char option[SIZE]; //array var to store user option after menu prompt
	int repeat; //var for counting number of user input lines that are repeated

	Open_Close_File(argc, argv, coinFile, nameCash);//this function opens, error checks and closes the file.
	Calculate_Change(nameCash, change); //this function calculates the coins to be given in change
	outputLength = Disp_Menu_Customer(nameCash, change, outputLength, output); //this displays the menu, name and coin amounts for each customer the user requests to see, and adds up values for use in output .csv file
	repeat = Add_Common_Names(output, outputLength, repeat); //this function compares the input lines and removes repeated names and adds total coins
	Write_To_File(infoFile, argv, output, outputLength, repeat); //this funciton writes to the .csv file
	return(0);
}



