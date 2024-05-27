#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { NOUGHTS, CROSSES, BORDER, EMPTY };
enum { HUMANWIN, COMPWIN, DRAW };

const int Directions[4] = { 1, 5, 4, 6 };

const int ConvertTo25[9] = {
    6, 7, 8,
    11, 12, 13,
    16, 17, 18
};

int ply = 0;
int positions = 0;
int maxPly = 0;

int GetNumForDir(int startSq, const int dir, const int *board, const int us) {
    int found = 0;
    while (board[startSq] != BORDER) {
        if (board[startSq] != us) {
            break;
        }
        found++;
        startSq += dir;
    }
    return found;
}

int FindThreeInARow(const int *board, const int ourindex, const int us) {
    int DirIndex = 0;
    int Dir = 0;
    int threeCount = 1;

    for (DirIndex = 0; DirIndex < 4; ++DirIndex) {
        Dir = Directions[DirIndex];
        threeCount += GetNumForDir(ourindex + Dir, Dir, board, us);
        threeCount += GetNumForDir(ourindex + Dir * -1, Dir * -1, board, us);
        if (threeCount == 3) {
            break;
        }
        threeCount = 1;
    }
    return threeCount;
}

int FindThreeInARowAllBoard(const int *board, const int us) {
    int threeFound = 0;
    int index;
    for (index = 0; index < 9; ++index) {
        if (board[ConvertTo25[index]] == us) {
            if (FindThreeInARow(board, ConvertTo25[index], us) == 3) {
                threeFound = 1;
                break;
            }
        }
    }
    return threeFound;
}

int EvalForWin(const int *board, const int us) {
    if (FindThreeInARowAllBoard(board, us) != 0)
        return 1;
    if (FindThreeInARowAllBoard(board, us ^ 1) != 0)
        return -1;

    return 0;
}

int MinMax(int *board, int side) {
    int MoveList[9];
    int MoveCount = 0;
    int bestScore = -2;
    int score = -2;
    int bestMove = -1;
    int Move;
    int index;

    if (ply > maxPly) maxPly = ply;
    positions++;

    if (ply > 0) {
        score = EvalForWin(board, side);
        if (score != 0) {
            return score;
        }
    }

    for (index = 0; index < 9; ++index) {
        if (board[ConvertTo25[index]] == EMPTY) {
            MoveList[MoveCount++] = ConvertTo25[index];
        }
    }

    for (index = 0; index < MoveCount; ++index) {
        Move = MoveList[index];
        board[Move] = side;

        ply++;
        score = -MinMax(board, side ^ 1);
        if (score > bestScore) {
            bestScore = score;
            bestMove = Move;
        }
        board[Move] = EMPTY;
        ply--;
    }

    if (MoveCount == 0) {
        bestScore = FindThreeInARowAllBoard(board, side);
    }

    if (ply != 0)
        return bestScore;
    else
        return bestMove;
}

void InitialiseBoard(int *board) {
    int index = 0;

    for (index = 0; index < 25; ++index) {
        board[index] = BORDER;
    }

    for (index = 0; index < 9; ++index) {
        board[ConvertTo25[index]] = EMPTY;
    }
}

void PrintBoard(const int *board) {
    int index = 0;
    char pceChars[] = "OX|-";

    printf("\n\nBoard:\n\n");
    for (index = 0; index < 9; ++index) {
        if (index != 0 && index % 3 == 0) {
            printf("\n\n");
        }
        printf("%4c", pceChars[board[ConvertTo25[index]]]);
    }
    printf("\n");
}

int HasEmpty(const int *board) {
    int index = 0;

    for (index = 0; index < 9; ++index) {
        if (board[ConvertTo25[index]] == EMPTY) return 1;
    }
    return 0;
}

void MakeMove(int *board, const int sq, const int side) {
    board[sq] = side;
}

int GetNextBest(const int *board) {
    int ourMove = ConvertTo25[4];
    if (board[ourMove] == EMPTY) {
        return ourMove;
    }

    int index = 0;
    ourMove = -1;

    for (index = 0; index < 4; index++) {
        ourMove = ConvertTo25[index * 2];
        if (board[ourMove] == EMPTY) {
            break;
        }
        ourMove = -1;
    }

    return ourMove;
}

int GetWinningMove(int *board, const int side) {
    int ourMove = -1;
    int winFound = 0;
    int index = 0;

    for (index = 0; index < 9; ++index) {
        if (board[ConvertTo25[index]] == EMPTY) {
            ourMove = ConvertTo25[index];
            board[ourMove] = side;

            if (FindThreeInARow(board, ourMove, side) == 3) {
                winFound = 1;
            }
            board[ourMove] = EMPTY;
            if (winFound == 1) {
                break;
            }
            ourMove = -1;
        };
    }
    return ourMove;
}

int GetComputerMove(int *board, const int side) {
    ply = 0;
    positions = 0;
    maxPly = 0;
    int best = MinMax(board, side);
    printf("Finished Searching positions:%d maxDepth:%d bestMove:%d\n", positions, maxPly, best);
    return best;
}

int GetHumanMove(const int *board) {
    char userInput[64];

    int moveOk = 0;
    int move = -1;

    while (moveOk == 0) {
        printf("Please enter a move from 1 to 9:");
        if (!fgets(userInput, 64, stdin)) continue;

        if (strlen(userInput) != 2) {
            printf("Invalid input length\n");
            continue;
        }

        if (sscanf(userInput, "%d", &move) != 1) {
            move = -1;
            printf("Invalid input\n");
            continue;
        }

        if (move < 1 || move > 9) {
            move = -1;
            printf("Move out of range\n");
            continue;
        }

        move--; // Zero indexing

        if (board[ConvertTo25[move]] != EMPTY) {
            move = -1;
            printf("Square not available\n");
            continue;
        }
        moveOk = 1;
    }
    printf("Making Move...%d\n", (move + 1));
    return ConvertTo25[move];
}

void RunGame() {
    int GameOver = 0;
    int Side = CROSSES;
    int LastMoveMade = 0;
    int board[25];

    InitialiseBoard(board);
    PrintBoard(board);

    while (!GameOver) {
        if (Side == NOUGHTS) {
            LastMoveMade = GetHumanMove(board);
            MakeMove(board, LastMoveMade, Side);
            Side = CROSSES;
        } else {
            LastMoveMade = GetComputerMove(board, Side);
            MakeMove(board, LastMoveMade, Side);
            Side = NOUGHTS;
            PrintBoard(board);
        }

        if (FindThreeInARow(board, LastMoveMade, Side ^ 1) == 3) {
            printf("Game over!\n");
            GameOver = 1;
            if (Side == NOUGHTS) {
                printf("Computer Wins\n");
            } else {
                printf("Human Wins\n");
            }
        }

        if (!HasEmpty(board)) {
            printf("Game over!\n");
            GameOver = 1;
            printf("It's a draw\n");
        }
    }
    PrintBoard(board);
}

int main() {
    srand(time(NULL));
    RunGame();
    return 0;
}

