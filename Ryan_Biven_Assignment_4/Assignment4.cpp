// EECS 348 Assignment 4
// Description: A C++ program that generates all possible solutions for a sudoku puzzle
// Given an input of puzzle from a text files
// Inputs: puzzle1.txt, puzzle2.txt, puzzle3.txt, puzzle4.txt, puzzle5.txt, 
// Outputs: All solutions for each puzzle in the terminal
// Colaborators: Ryan Biven
// Sources: Claude and gemini
// Author: Ryan Biven
// Creation Date: 10/15
// Revision Date: 10/15
// Revisions: Checking if puzzle is valid before attempting
// command line input instead of hardcoded files names
// seperate board and solver classes
#include <iostream> // stream io operations
#include <fstream> // file stream operations
#include <string> // string class library

class Board { // board object encapsulating grid data
private: // private board attributes
    static const int SIZE = 9; // board size constraint 9x9
    int grid[SIZE][SIZE]; // 2d array grid storage

public: // public board operations
    Board() { // board default constructor
        for (int r = 0; r < SIZE; ++r) { // iterate grid rows
            for (int c = 0; c < SIZE; ++c) { // iterate grid columns
                grid[r][c] = 0; // initialize cell to zero
            }
        }
    }

    int getCell(int r, int c) const { // getter for board cell value
        return grid[r][c]; // return integer value at location
    }

    void setCell(int r, int c, int val) { // setter for board cell value
        grid[r][c] = val; // assign new value to location
    }

    bool isValid(int row, int col, int num) const { // check validity of candidate number
        for (int i = 0; i < SIZE; ++i) { // loop board dimension
            if ((i != col && grid[row][i] == num) || (i != row && grid[i][col] == num)) { // row and column collision check
                return false; // collision found
            }
        }
        int startRow = (row / 3) * 3; // compute block row offset
        int startCol = (col / 3) * 3; // compute block col offset
        for (int r = 0; r < 3; ++r) { // loop block rows
            for (int c = 0; c < 3; ++c) { // loop block cols
                int currentRow = startRow + r; // calculate actual row index
                int currentCol = startCol + c; // calculate actual col index
                if ((currentRow != row || currentCol != col) && grid[currentRow][currentCol] == num) { // block collision check
                    return false; // collision inside block found
                }
            }
        }
        return true; // candidate placement is valid
    }

    bool isInitialValid() const { // verify board has no initial duplicate rule violations
        for (int r = 0; r < SIZE; ++r) { // row iteration loop
            for (int c = 0; c < SIZE; ++c) { // col iteration loop
                if (grid[r][c] != 0) { // test non empty square
                    if (!isValid(r, c, grid[r][c])) { // test duplicate rule violation
                        return false; // invalid starting configuration
                    }
                }
            }
        }
        return true; // valid initial board state
    }

    bool findEmptyCell(int& row, int& col) const { // locate next blank cell
        for (row = 0; row < SIZE; ++row) { // row iteration loop
            for (col = 0; col < SIZE; ++col) { // col iteration loop
                if (grid[row][col] == 0) { // check zero representation
                    return true; // found blank cell location
                }
            }
        }
        return false; // board contains no blank cells
    }

    bool loadFromFile(const std::string& filename) { // load grid data from file
        std::ifstream inFile(filename); // open input file stream
        if (!inFile.is_open()) { // test stream open state
            std::cerr << "Error: Could not open file " << filename << "\n"; // error output stream
            return false; // file open failure
        }
        char ch; // temp character holder
        for (int r = 0; r < SIZE; ++r) { // row iteration loop
            for (int c = 0; c < SIZE; ++c) { // col iteration loop
                if (!(inFile >> ch)) { // read character from file
                    std::cerr << "Error: Invalid file format in " << filename << "\n"; // format error output
                    return false; // file format error
                }
                if (ch == '_') { // underscore character check
                    grid[r][c] = 0; // treat underscore as zero
                } else if (ch >= '1' && ch <= '9') { // character digit check
                    grid[r][c] = ch - '0'; // convert char to int
                } else { // fallback character handler
                    grid[r][c] = 0; // set unknown char to zero
                }
            }
        }
        inFile.close(); // close input file stream
        return true; // successful board load
    }

    void print() const { // display current board state
        for (int r = 0; r < SIZE; ++r) { // row iteration loop
            for (int c = 0; c < SIZE; ++c) { // col iteration loop
                if (grid[r][c] == 0) { // check zero value
                    std::cout << "_ "; // print underscore for empty
                } else { // non zero value
                    std::cout << grid[r][c] << " "; // print number value
                }
            }
            std::cout << "\n"; // print row line break
        }
    }
};

class SudokuSolver { // solver coordinator class
private: // private solver attributes
    Board board; // board object managing grid state
    std::string filename; // puzzle filename storage
    int solutionCount; // counts valid solutions

    void solveRecursive() { // depth first recursive search
        int row, col; // blank cell coordinates
        if (!board.findEmptyCell(row, col)) { // test if grid completed
            solutionCount++; // increment solution counter
            std::cout << "\nSolution #" << solutionCount << ":\n"; // header for solution
            board.print(); // print completed board
            return; // backtrack to search for additional solutions
        }
        for (int num = 1; num <= 9; ++num) { // iterate digits 1 through 9
            if (board.isValid(row, col, num)) { // test candidate digit
                board.setCell(row, col, num); // place candidate digit
                solveRecursive(); // recursive search call
                board.setCell(row, col, 0); // backtrack reset cell
            }
        }
    }

public: // public solver operations
    SudokuSolver(const std::string& file) : filename(file), solutionCount(0) {} // constructor initialization

    bool loadPuzzle() { // load puzzle delegation method
        return board.loadFromFile(filename); // forward load call to board object
    }

    void solve() { // execute solving routine
        std::cout << "========================================\n"; // print top border
        std::cout << "Puzzle File: " << filename << "\n"; // display puzzle file name
        std::cout << "----------------------------------------\n"; // print section divider
        std::cout << "Original Puzzle:\n"; // label original board output
        board.print(); // print original board

        if (!board.isInitialValid()) { // check initial board validity
            std::cout << "\nPuzzle is invalid\n"; // print invalid puzzle message
            std::cout << "========================================\n\n"; // print bottom border
            return; // exit early on invalid input board
        }

        solveRecursive(); // initiate depth first search

        if (solutionCount == 0) { // test if zero solutions found
            std::cout << "\nNo solution found\n"; // print no solution notice
        }
        std::cout << "========================================\n\n"; // print bottom border
    }
};

int main(int argc, char* argv[]) { // main entry point accepting cli args
    if (argc < 2) { // check if minimum argument count present
        std::cerr << "Usage: " << argv[0] << " <puzzle1.txt> [puzzle2.txt ...]\n"; // display CLI usage instructions
        return 1; // return error code
    }

    for (int i = 1; i < argc; ++i) { // loop over command line argument filenames
        SudokuSolver solver(argv[i]); // create solver instance for filename
        if (solver.loadPuzzle()) { // attempt loading puzzle file
            solver.solve(); // execute solver routine
        }
    }

    return 0; // successful execution termination
}