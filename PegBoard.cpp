/*
Author: Jonathan Drouillard
Description: An algorithm to solve the Cracker Barrel peg board.
Due Date: 1/20/12
Date Created: 1/20/12
Date Last Modified: 1/20/12
*/

/*
Resources Used:
    http://en.wikipedia.org/wiki/Breadth-first_search
*/

/*
Acknoledgements:
    Bill Booth advised me to use a queue to store valid empty locations.
*/

#include <iostream>
#include <stack>
#include <queue>
using namespace std;

//Used to signify direction a peg is moving in
//R = Right, DR = Down Right, DL = Down Left, L = Left
//UL = Up Left, UR = Up Right
enum Direction {R/*0*/, DR/*1*/, DL/*2*/, L/*3*/, UL/*4*/, UR/*5*/};

//Used to signify the state of a location on the board.
//A peg can be empty or used.
//An unused location on the board cannot hold pegs.
enum Peg {EMPTY/*0*/, USED/*1*/, UNUSED/*2*/};

//Used to hold <<location row, location col>, Direction> for a peg.
typedef pair<pair<int, int>, int> choice;

//A stack to hold a history of Moves.
typedef stack<choice> Moves; //Stack of <<location r,c> Direction>

//A queue to hold valid moves to EMPTY pegs from USED pegs.
typedef queue<choice> ValidMoves;

//************************************************************
// name jump
// description: Moves a peg on the board at row, col in the  *
//              specified direction.                         *
// return: void                                              *
// pre: dir is a valid Direction from the Direction enum.    *
//      row, col are within bounds of the board.             *
//      The move in dir for row, col results in a move       *
//      within the bounds of the board.                      *
// post: If the move was valid, moves a peg by setting its   *
//       original location to UNUSED.  Also setting the peg  *
//       it jumped to UNUSED.  Finally setting the location  *
//       at which the peg landed to USED.                    *
//                                                           *
//************************************************************
void jump(int board[5][11], int row, int col, Direction dir);

//************************************************************
// name: undo
// description: Moves a peg on the board at row, col in the  *
//              specified direction.                         *
// return: void                                              *
// pre: dir is a valid Direction from the Direction enum.    *
//      row, col are within bounds of the board.             *
//      The move in dir for row, col results in a move       *
//      within the bounds of the board.                      *
// post: If the move was valid, moves a peg by setting its   *
//       original location to USED.  Also setting the peg    *
//       it jumped to USED.  Finally setting the location    *
//       at which the peg originally landed to UNUSED.       *
//                                                           *
//************************************************************
void undo(int board[5][11], int row, int col, Direction dir);

//************************************************************
// name: validMove                                           *
// description: Checks if a peg can move from Direction i    *
//              to emptyRow, emptyCol on the board.          *
// return: boolean                                           *
// pre: emptyPegRow, emptyPegCol is an EMPTY peg.            *
//      i is a valid Direction.                              *
// post: Returns true if it was a valid move.                *
//       Returns false if it was an invalid move.            *
//       Modified legalMoveRow and legalMoveCol              *
//       will contain the Row which can                      *
//       move to emptyRow, emptyCol in a jump.               *
//                                                           *
//************************************************************
bool validMove(int board[5][11], Direction i, int emptyPegRow,
               int emptyPegCol, int &legalMoveRow, int&legalMoveCol);

//************************************************************
// name: findValidMoves                                      *
// description: Finds all valid moves (empty pegs)           *
//              on the board.                                *
// return:  integer                                          *
// pre: ValidMoves is an empty queue.                        *
// post: Modifies the validMoves queue to contain all        *
//       empty pegs which were found on the board.           *
//       Returns an integer with the count of empty pegs.    *
//                                                           *
//************************************************************
int findValidMoves(int board[5][11], ValidMoves &validMoves);

//************************************************************
// name: solve                                               *
// description: Solves a valid peg board using recursion and *
//              breadth first search.                        *
// return: boolean                                           *
// pre: row and col are within bounds of the board.          *
//      dir is a valid Direction.                            *
// post: If a solution is found, the function returns true.  *
//       If no solution is found, the function returns false.*
//       History is modified to contain the list of moves,   *
//       which is a solution from end to start.              *
//                                                           *
//************************************************************
bool solve(int board[5][11], Moves &history, int row, int col, Direction dir);


/*
Data Abstraction
    Creates 2d integer array board for the layout of the board using the Peg enum types.
    Creates Moves (Stack of a pair of pairs all of type int) to hold the moves of the solution.
    Creates orderedHistory to hold the inverse of Moves for printing a start to finish solution.
Input
    There is no input.
Process/Implementation
    We create a peg board using an enumerated type to specify the
    layout of the board in a 2d array.
    A history stack is created to save the winning moves.
    We call the solve function with the initial board,
    which returns a modified history stack.
    We reverse this history stack and print the contents of the
    reverse ordered stack in board format.
    If the history stack is empty, there is no solution.
Output
    If there is a solution, writes the solution stage by stage to standard out.
    If there is no solution, explains that no solution was found.
Assumptions
    That the triangle is placed in a 5x11 2d integer array with Peg enum types.
    The triangle is laid out with a column of UNUSED on the start and end columns.
    The bottom layer of the triangle has an UNUSED after every USED or EMPTY.
    The triangle is built up from this bottom layer, following the same logic.
*/
int main() {
    //The board holds the peg puzzle to be solved.
    int board[5][11] = { {UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,   USED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED },
                         {UNUSED, UNUSED, UNUSED, UNUSED,   USED, UNUSED,   USED, UNUSED, UNUSED, UNUSED, UNUSED },
                         {UNUSED, UNUSED, UNUSED,   USED, UNUSED,   USED, UNUSED,   USED, UNUSED, UNUSED, UNUSED },
                         {UNUSED, UNUSED,   USED, UNUSED,   USED, UNUSED,   USED, UNUSED,   USED, UNUSED, UNUSED },
                         {UNUSED,   USED, UNUSED,   USED, UNUSED,  EMPTY, UNUSED,   USED, UNUSED,   USED, UNUSED }
                       };
    //Stores the history of attempted moves.
    //If this stack is not empty after the solve function,
    //A solution exists in history in end to start order.
    Moves history;

    //Calls the solve function with -1 inputs for row, col, and direction.
    //-1 for these 3 parameters are base cases which tell the solver to
    //not jump, but just find valid moves on the first run.
    solve(board, history, -1, -1, static_cast<Direction>(-1));

    //Stores the history of attempted moves from the history stack.
    //By popping them off history into this stack,
    //we get the start to end order of the solution.
    Moves orderedHistory;

    //Check that the history stack has elements.
    //If it is non zero, then, it holds a solution.
    if(history.size() > 0) {
        //Push each element from history onto the ordered history stack
        while(!history.empty()) {
            orderedHistory.push(history.top());
            history.pop();
        }
        //Print the board with the initial configuration.
        for(int r = 0; r < 5; r++) { //for each row in the board
            for(int c = 0; c < 11; c++) { //loop through each column for each row
                if(board[r][c] == UNUSED) //If unused, print a space.
                    cout << " ";
                if(board[r][c] == EMPTY) //If empty, print a X.
                    cout << "X";
                if(board[r][c] == USED) //If used, print a O.
                    cout << "O";
            }
            cout << endl; //Print an endline after each row.
        }

        //Loop through the ordered history stack.
        while(!orderedHistory.empty()) {
            //Perform a jump on our original board, following each step
            //as described in the ordered history stack.
            jump(board, orderedHistory.top().first.first, orderedHistory.top().first.second, static_cast<Direction>(orderedHistory.top().second));
            //Print the original board after the jump is performed.
            for(int r = 0; r < 5; r++) {
                for(int c = 0; c < 11; c++) {
                    if(board[r][c] == UNUSED)
                        cout << " ";
                    if(board[r][c] == EMPTY)
                        cout << "X";
                    if(board[r][c] == USED)
                        cout << "O";
                }
                cout << endl;
            }
            cout << endl;

            //Remove the move from history,
            //because we have followed the move and printed it.
            orderedHistory.pop();
        }
    }
    else //If the history stack is empty, there is no solution.
        cout << "No solution found" << endl;

    cin.get();cin.get();
    //Exit the program with a successful return code.
    return EXIT_SUCCESS;
}


void jump(int board[5][11], int row, int col, Direction dir) {
    //The current row and column is the peg that is moving.
    //The jumping peg always becomes empty.
    board[row][col] = EMPTY;
    //Switch the direction and decide where the jumping peg is moving to.
    switch(dir) {
        //The row which is set to EMPTY is the peg that is jumped
        //by the jumping peg.
        //The row which is set to USED is the peg that is landed
        //by the jumping peg.
        case R: //Jump Right
            board[row][col-2] = EMPTY;
            board[row][col-4] = USED;
            break;
        case DR: //Jump Down Right
            board[row-1][col-1] = EMPTY;
            board[row-2][col-2] = USED;
            break;
        case DL: //Jump Down Left
            board[row-1][col+1] = EMPTY;
            board[row-2][col+2] = USED;
            break;
        case L: //Jump Left
            board[row][col+2] = EMPTY;
            board[row][col+4] = USED;
            break;
        case UL: //Jump Up Left
            board[row+1][col+1] = EMPTY;
            board[row+2][col+2] = USED;
            break;
        case UR: //Jump Up Right
            board[row+1][col-1] = EMPTY;
            board[row+2][col-2] = USED;
            break;
        default:
            cout << "Unknown direction.  Exiting program.\n";
            exit(EXIT_FAILURE);
    }
}


void undo(int board[5][11], int row, int col, Direction dir) {
    //The current row and column is the peg that originally did a jump,
    //and we are now bringing it back to its pre-jump state.
    //The jumping peg is set to USED again from EMPTY.
    board[row][col] = USED;
    switch(dir) {
        //The row which is set to USED is the peg that was originally jumped
        //by the jumping peg.
        //The row which is set to EMPTY is the peg that the jumping peg
        //originally jumped to.
        case R: //Undo Jump Right
            board[row][col-2] = USED;
            board[row][col-4] = EMPTY;
            break;
        case DR: //Undo Jump Down Right
            board[row-1][col-1] = USED;
            board[row-2][col-2] = EMPTY;
            break;
        case DL: //Undo Jump Down Left
            board[row-1][col+1] = USED;
            board[row-2][col+2] = EMPTY;
            break;
        case L: //Undo Jump Left
            board[row][col+2] = USED;
            board[row][col+4] = EMPTY;
            break;
        case UL: //Undo Jump Up Left
            board[row+1][col+1] = USED;
            board[row+2][col+2] = EMPTY;
            break;
        case UR: //Undo Jump Up Right
            board[row+1][col-1] = USED;
            board[row+2][col-2] = EMPTY;
            break;
        default:
            cout << "Unknown direction.  Exiting program.\n";
            exit(EXIT_FAILURE);
    }
}

bool validMove(int board[5][11], Direction i, int emptyPegRow, int emptyPegCol, int &legalMoveRow, int&legalMoveCol) {
    /*We are checking if there is a valid move from an empty peg*/
    //Initialize legal Moves to the emptyPeg location.
    legalMoveRow = emptyPegRow;
    legalMoveCol = emptyPegCol;
    switch(i) {
        //Throughout each case of direction,
        //we check if there is a legal move to the empty spot.
        //We perform bounds checking before each movement.
        //We go ahead and modify legalMoveRow or legalMoveCol
        //to the row and/or column to which the jump would be performed.
        //We check if the jumping peg and the peg to be jumped are both USED.
        //If they are both used, we return TRUE, else FALSE.
        //We already know the emptyPegRow and emptyPegCol point to an empty peg.
        case R: //Right
            if(emptyPegCol+4 > 9)
                return false;
            legalMoveCol += 4;
            return ((board[emptyPegRow][emptyPegCol+2] == USED) ? true : false)
                && ((board[emptyPegRow][emptyPegCol+4] == USED) ? true : false);
            break;
        case DR: //Down Right
            if(emptyPegRow+2 > 4 || emptyPegCol+2 > 9)
                return false;
            legalMoveRow += 2;
            legalMoveCol += 2;

            return ((board[emptyPegRow+1][emptyPegCol+1] == USED) ? true : false)
                && ((board[emptyPegRow+2][emptyPegCol+2] == USED) ? true : false);
            break;
        case DL: //Down Left
            if(emptyPegRow+2 > 4 || emptyPegCol-2 < 1)
                return false;
            legalMoveRow += 2;
            legalMoveCol -= 2;
            return ((board[emptyPegRow+1][emptyPegCol-1] == USED) ? true : false)
                && ((board[emptyPegRow+2][emptyPegCol-2] == USED) ? true : false);
            break;
        case L: //Left
            if(emptyPegCol-4 < 1)
                return false;
            legalMoveCol -= 4;
            return ((board[emptyPegRow][emptyPegCol-2] == USED) ? true : false)
                && ((board[emptyPegRow][emptyPegCol-4] == USED) ? true : false);
            break;
        case UL: //Up Left
            if(emptyPegRow-2 < 0 || emptyPegCol-2 < 1)
                return false;
            legalMoveRow -= 2;
            legalMoveCol -= 2;
            return ((board[emptyPegRow-1][emptyPegCol-1] == USED) ? true : false)
                && ((board[emptyPegRow-2][emptyPegCol-2] == USED) ? true : false);
            break;
        case UR: //Up Right
            if(emptyPegRow-2 < 0 || emptyPegCol+2 > 9)
                return false;
            legalMoveRow -= 2;
            legalMoveCol += 2;
            return ((board[emptyPegRow-1][emptyPegCol+1] == USED) ? true : false)
                && ((board[emptyPegRow-2][emptyPegCol+2] == USED) ? true : false);
            break;
        default:
            cout << "Unknown direction.  Exiting program.\n";
            exit(EXIT_FAILURE);
    }
}

int findValidMoves(int board[5][11], ValidMoves &validMoves) {
    //Used to check if the amount of empty pegs is number of pegs-1.
    int emptyCount = 0;
    for(int row = 0; row < 5; row++) //loop through each row
        for(int col = 0; col < 11; col++) //loop through each column
            if(board[row][col] == EMPTY) { //if the current peg is EMPTY
                emptyCount++; //Increment the empty count.
                for(int i = R; i <= UR; i++) { //Check every direction of the EMPTY peg.
                    int moveRow, moveCol; //Use this to store the peg which can jump.
                    //Check if the EMPTY peg in the current direction i is
                    //a valid move.  If so, push it on the queue.
                    if(validMove(board, static_cast<Direction>(i), row, col, moveRow, moveCol)) {
                        validMoves.push(make_pair(make_pair(moveRow, moveCol), i));
                    }
                }
            }
    //Used to determine if the board is solved.
    return emptyCount;
}

bool solve(int board[5][11], Moves &history, int row, int col, Direction dir) {
    //Used to signify that the solution has been found and recursion can end.
    bool solved = false;
    //Used to hold valid moves for _this_ instance of recursion.
    ValidMoves validMoves;
    
    //Base case. If this is the initial call, do not perform a jump.
    //Instead, move ahead and find the EMPTY peg.
    if(row != -1 && col != -1 && dir != -1) {
        //If this is not the first run,
        //perform a jump on the board at row, col in direction dir.
        jump(board, row, col, dir);
        //Push this jump onto the history table, in the case
        //this is a piece of the solution.
        history.push(make_pair(make_pair(row, col), dir));
    }//Else, find the empty peg(s) and work from there.

    //Call the function to loop through the table finding valid moves.
    //Store the amount empty pegs in emptyCount.
    int emptyCount = findValidMoves(board, validMoves);

    //If emptyCount is 14, then a solution has been found.
    //Only 1 peg is left on the board with "USED".
    if(emptyCount == 14)
        return true;

    //If the valid moves queue is empty, showing that there is no solution,
    //proceed to return false and follow another routine.
    if(validMoves.empty())
        return false; //No more valid moves.

    //Loop through each of the valid moves in the queue.
    //If solved is true, then we have found a solution... stop searching.
    while(!validMoves.empty() && !solved) {
        //Pull the front of the queue
        choice front = validMoves.front();
        //Remove the front from the queue.
        validMoves.pop();
        //Call recursion on this valid location.
        solved = solve(board, history, front.first.first, front.first.second, static_cast<Direction>(front.second));
        //If that recursive call did not solve the puzzle, remove the jump from history.
        if(!solved)
            history.pop();
        //After removing the jump from history, undo the jump from the board
        undo(board, front.first.first, front.first.second, static_cast<Direction>(front.second));
        //Repeat with the next valid move until the queue is empty.
    }

    //If we have a solution from the above loop, return true.
    if(solved == true)
        return true;

    //Not solved, move back a step in recursion.
    return false;
}
