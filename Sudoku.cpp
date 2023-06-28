/***********************************************************************
* Program:
*    Project 13
*    Professor Christensen, CS124
* Author:
*    Klevin Doda
* Summary: 
*    Reads from the file given by user and stores it in the board.
*    Interacts with user to play SUDOKU game. Offers this options:
*    to edit a square, checks for proper input, displays the board
*    writes board to another file, show possibilities for a position etc.
*    Differences from Project 13: More user friendly for error inputs
*    validates the file while reading it, displays read-only values in
*    green, allows user to modify write and read values after filling 
*    them for the first time.
*
************************************************************************/
#include <iostream>
#include <cassert> 
#include <fstream> 
#include <stdlib.h> //exit

using namespace std;
#define SIZE 9

void getSourceFile(char fileName[]);
void getDestinationFile(char fileName[]);
bool readFile(const char fileName[], int board[][SIZE]);
void isValueValid(const int board[][SIZE], int value, int row, int col);
void interact(int board[][SIZE]);
void displayInstructions();
void displayBoard(const int board[][SIZE]);
char getInstruction();
bool isValidInstruction(const char inst);
void editSquare(int board[][SIZE]);
bool getSquare(const int board[][SIZE], char pos[]);
void getSquareCoordinates(char pos[], int &row, int &col);
bool canInsertValue(const int board[][SIZE], int value, int row, int col);
void showPossibleValues(const int board[][SIZE]);
void computeValues(const int board[][SIZE], int row, int col);
int getLeftCorner(int x);
void writeToFile(const int board[][SIZE]);
void convertPosToIndexes(char pos[], int &row, int &col);


/***********************************************************************
* Prompts user for file name. Reads the file into the board. Fills board
* with 0 values. Invokes interact function to allow user to play SUDOKU
************************************************************************/
int main()
{
   int board[SIZE][SIZE];
   bool lookUp[SIZE][SIZE]; //to check for read write
  
   for (int i = 0;i < SIZE;++i)
      for (int j = 0;j < SIZE;++j)
         board[i][j] = 0; //filling with zero
   //important for isValueFilled function because we do not want 
   //board to have random values
   
   char fileName[50];
   getSourceFile(fileName);

   if (!readFile(fileName, board)) //print error
   {
      cout << "Error reading file!" << endl;
      return 1;
   }
   
   interact(board);
   return 0;
}

/***********************************************************************
* Prompts user for source file name, and stores in the array
************************************************************************/
void getSourceFile(char fileName[])
{
   cout << "Where is your board located? ";
   cin >> fileName;
}

/***********************************************************************
* Prompts user for destination file name, and stores in the array
************************************************************************/
void getDestinationFile(char fileName[])
{
   cout << "What file would you like to write your board to: ";
   cin >> fileName;
}

/***********************************************************************
* Reads the file into the board. If an error occurred while opening file
* returns false otherwise returns true. If value read from the file is
* duplicate terminates program and shows an error message
************************************************************************/
bool readFile(const char fileName[], int board[][SIZE])
{
   ifstream fin(fileName);
   
   if (fin.fail())
      return false;
   
   int i; //row 
   int j; //col
   int value;
   
   for (i = 0;i < SIZE;++i)
   {
      for (j = 0;j < SIZE;++j)
      {
         fin >> value;
         if (value != 0)
            isValueValid(board, value, i, j); 
         //if it is not valid program will terminate
         
         board[i][j] = value; //store it if program didn't terminate
      }
   }
   
   assert(i == SIZE && j == SIZE);
   return true; 
}

/***********************************************************************
* Check if the value is valid to enter in the board. If the value
* is not valid it displays an error message and terminates the program
* Board's values should be all zero.It's important for the logic.
************************************************************************/
void isValueValid(const int board[][SIZE], int value, int row, int col)
{
   //check the row and col the same time
   for (int i = 0;i < SIZE;++i)
   {
      if (board[i][col] == value)
      {
         cout << "ERROR: Duplicate value '" << value << "' in row '" 
              << i + 1 << "'" << endl;
         exit(1); // terminate program
      }
      
      
      if (board[row][i] == value)
      {
         cout << "ERROR: Duplicate value '" << value << "' in columns '"
              << (char)(col + 65) << "'" << endl;
         exit(1); // terminate program
      }
      
   }
   row = getLeftCorner(row);
   col = getLeftCorner(col);
   
   int rowDownCorner = row + 2;
   int colRightCorner = col + 2;
   
   //search the 3 x 3 square
   for (int i = row; i <= rowDownCorner; ++i)
      for (int j = col; j <= colRightCorner; ++j)
         if (board[i][j] == value)
         {
            cout << "ERROR: Duplicate value '" << value 
                 << "' in inside square represented by '"
                 << (char)(j + 65) << i + 1 << "'" << endl;
            exit(1); // terminate program
         }
}

/***********************************************************************
* Displays instruction and the board on the screen. Continually 
* prompts user until he gives Q. Invokes methods according to user's
* input.
************************************************************************/
void interact(int board[][SIZE])
{
   displayInstructions(); //the first time
   displayBoard(board);
   
   char inst; //instruction
   
   do
   {
      //inst will upperCase and valid
      inst = getInstruction();
      //returns valid inst
      
      switch (inst)
      {
         case '?':
            displayInstructions();
            break;
         case 'D':
            displayBoard(board);
            break;
         case 'E':
            editSquare(board);
            break;
         case 'S':
            showPossibleValues(board);
            break;
         case 'Q':
            writeToFile(board);
            break;
      }
   } while (inst != 'Q');
}

/***********************************************************************
* Displays the table of instruction on screen.
************************************************************************/
void displayInstructions()
{
   cout << "Options:" << endl
        << "   ?  Show these instructions" << endl
        << "   D  Display the board" << endl
        << "   E  Edit one square" << endl
        << "   S  Show the possible values for a square" << endl
        << "   Q  Save and Quit" << endl << endl;
}

/***********************************************************************
* Displays board on screen according to the instructions. Green if
* value is read only.
************************************************************************/
void displayBoard(const int board[][SIZE])
{
   cout << "   A B C D E F G H I" << endl;
   
   for (int i = 0;i < SIZE;++i)
   {
      if (i != 0 && i % 3 == 0)
         cout << "   -----+-----+-----" << endl;
      
      cout << i + 1 << " ";
      
      for (int j = 0;j < SIZE;++j)
      {
         cout << (j != 0 && j % 3 == 0 ? "|" : " ");
         if (board[i][j] == 0)
            cout << " ";
         else if (board[i][j] > 0)
            cout << "\E[22;32m" << board[i][j] << "\E[0m";
         else
            cout << - board[i][j]; //double negative = positive
         
      }
      cout << endl;
   }

}

/***********************************************************************
* Prompts user for instruction until he enter a valid one. Returns 
* the valid instruction.
************************************************************************/
char getInstruction()
{
   char inst;
   do
   {
      cout << endl << "> ";
      cin >> inst; 
      
      inst = (char)toupper(inst);   
      if (isValidInstruction(inst))
         return inst;
      else
         cout << "ERROR: Invalid command" << endl;
   } while (true);
}

/***********************************************************************
* Validates the instruction is is correct or invalid. Returns true
* if valid, otherwise false.
************************************************************************/
bool isValidInstruction(const char inst) 
{
   switch (inst) 
   {
      case '?':
      case 'D':
      case 'E':
      case 'S':
      case 'Q':
         return true;
      default:
         return false;
   } 
}

/***********************************************************************
* Prompts user for the value to insert in the board. Inserts the value
* if it can be inserted according to SUDOKU rules else displays ERROR
************************************************************************/
void editSquare(int board[][SIZE])
{
   char pos[3]; //position in board 
   bool valid = getSquare(board, pos);
   
   
   if (valid)
   {
      int value; //the value that user enters for Sudoku
      int row; //row to insert
      int col; //col to insert
      convertPosToIndexes(pos, row, col);
      
      cout << "What is the value at '" << pos << "': ";
      cin >> value;
      
      //inserting a negative value to differentiate from read only
      if (canInsertValue(board, value, row, col))
         board[row][col] = - value; 
      else
         cout << "ERROR: Value '" << value << "' in square '"
              << pos << "' is invalid" << endl;
   }  
}

/***********************************************************************
* Prompts user for the square coordinates until he enters a valid one.
* Displays Error message if the position is read only and returns 
* false, else returns true. Position is received as an array by reference
************************************************************************/
bool getSquare(const int board[][SIZE], char pos[])
{
   int row; //row to insert
   int col; //col to insert
   getSquareCoordinates(pos, row, col);
   //pos is filled here
   
   if (board[row][col] > 0) //if it is read only
   {
      cout << "ERROR: Square '" << pos << "' is read-only" << endl;
      return false; 
   }
   else
      return true;   
}

/***********************************************************************
* Prompts user for the square coordinates until he enters a valid one.
* Displays Error message if the position is invalid and returns 
* Position is received as an array by reference
************************************************************************/
void getSquareCoordinates(char pos[], int &row,int &col)
{
   do
   {
      cout << "What are the coordinates of the square: ";
      cin >> pos;
      
      //storing them in temporary values because when user
      //enter xx we don't want to print XX
      char pos0 = (char)toupper(pos[0]);
      char pos1 = (char)toupper(pos[1]);
      
      if ('A' <= pos1 && pos1 <= 'I' && '1' <= pos0 && pos0 <= '9') 
      {
         //swap only if the input is in this format 1A
         char temp = pos0;
         pos0 = pos1;
         pos1 = temp;
      }
      
      if ('A' <= pos0 && pos0 <= 'I' && '1' <= pos1 && pos1 <= '9')
      {
         pos[0] = pos0; //store to the array
         pos[1] = pos1;
         convertPosToIndexes(pos, row, col);
         return; //valid
      }
      else
         cout << "ERROR: Square '" << pos << "' is invalid" << endl;
      
   } while (true);
}

/***********************************************************************
 * Checks if the value can be inserted at the position in the board.
 *  Returns true if it can else returns false.
 ************************************************************************/
bool canInsertValue(const int board[][SIZE], int value, int row, int col)
{
   if (value < 1 || value > 9)
      return false;
   
   //check the row and col the same time
   for (int i = 0;i < SIZE;++i)
      if (board[i][col] == value || board[row][i] == value)
         return false;
   
   row = getLeftCorner(row);
   col = getLeftCorner(col);
   
   int rowDownCorner = row + 2;
   int colRightCorner = col + 2;
   
   //search the 3 x 3 square
   for (int i = row; i <= rowDownCorner; ++i)
      for (int j = col; j <= colRightCorner; ++j)
         if (board[i][j] == value)
            return false;
   
   return true;
}

/***********************************************************************
* Prompts the user for a position until he provides a valid one.
* Displays the values that can be enter in the position.
************************************************************************/
void showPossibleValues(const int board[][SIZE])
{
   char pos[3]; //position
   int row;
   int col;
   getSquareCoordinates(pos, row, col);
   //row and col will be indexes of the position here
   
   cout << "The possible values for '" << pos << "' are: ";
   computeValues(board, row, col);
}

/***********************************************************************
* Displays the valid values that can be enter in the position on board
************************************************************************/
void computeValues(const int board[][SIZE], int row, int col)
{
   // array to check at the end which values to print
   // values of board are the indexes of the array
   bool values[10]; //all false by default
   
   //set the values which are in the row and col as true
   for (int i = 0;i < SIZE;++i)
   {
      values[board[i][col]] = true;
      values[board[row][i]] = true;
   }
   
   row = getLeftCorner(row);
   col = getLeftCorner(col);
   
   int rowRightCorner = row + 2;
   int colRightCorner = col + 2;
   
   //set the the values which are in the 3 x 3 square as true
   for (int i = row; i <= rowRightCorner; ++i)
      for (int j = col; j <= colRightCorner; ++j)
         values[board[i][j]] = true;
   
   
   //starting from 1 because we do not want the 0 value
   //which index is false it means is valid
   bool first = true; //for commas
   for (int i = 1;i < 10;++i)
   {
      if (!values[i])
      {
         if (first)
            first = false;
         else
            cout << ", ";
         
         cout << i;
      }
   }
   cout << endl;
}

/***********************************************************************
 * Returns the first left index of the 3x3 square according to the input
 ************************************************************************/
int getLeftCorner(int index)
{
   assert(index >= 0 && index <= 8);
   return (index - index % 3);
   //case 0,1,2 returns 1
   //case 3,4,5 returns 3
   //case 6,7,8 returns 6 which are the left corner index for 3X3 square
}

/***********************************************************************
* Prompts user for file name and writes the board in the file 
************************************************************************/
void writeToFile(const int board[][SIZE])
{
   char fileName[50];
   getDestinationFile(fileName);
   
   ofstream fout;
   fout.open(fileName);
   
   for (int i = 0;i < SIZE;++i)
      for (int j = 0;j < SIZE;++j)
         fout << board[i][j] << (j == SIZE - 1 ? '\n' : ' ');
     
   cout << "Board written successfully" << endl;
}

/***********************************************************************
* Receives the array which have the position, row and col by reference
* and converts the position to indexes. 
************************************************************************/
void convertPosToIndexes(char pos[], int &row, int &col)
{
   row = (pos[1] - '0') - 1; //turns it to an int
   col = pos[0] - 65; //subtracts the ASCII value 
}


