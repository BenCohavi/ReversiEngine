#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N (8)
#define BLACK (0)
#define WHITE (1)
#define EMPTY (2)

#define PLAYER  (0)
#define MACHINE (1)

#define ASSERT(cnd) if (!(cnd)) {printf("error! (%d)\n", __LINE__); abort(); _getch(); exit(0);}

#define IS_BETWEEN(num, low, high)  (((num) >= (low)) && ((num) <= (high)))

typedef struct
{
    int arr[N][N];
} Board;

typedef struct
{
    int color;
    int y;
    int x;
} Move;

int IsGameOver(const Board * xi_board)
{
    for (int y = 0; y < N; y += 1)
    {
        for (int x = 0; x < N; x += 1)
        {
            if (xi_board->arr[y][x] == EMPTY)
            {
                return 0;
            }
        }
    }

    return 1;
}

void CopyBoard(Board* xo_board, const Board* xi_board)
{
    for (int y = 0; y < N; y += 1)
    {
        for (int x = 0; x < N; x += 1)
        {
            xo_board->arr[y][x] = xi_board->arr[y][x];
        }
    }
}

void PrintBoard(const Board* xi_board)
{
    printf("    0 1 2 3 4 5 6 7\n\n");

    for (int y = 0; y < N; y += 1)
    {
        printf("%d   ", y);

        for (int x = 0; x < N; x += 1)
        {
            if (xi_board->arr[y][x] == BLACK)
            {
                printf("B ");
            }
            else if (xi_board->arr[y][x] == WHITE)
            {
                printf("W ");
            }
            else
            {
                printf("e ");
            }
        }

        printf("  %d\n", y);
    }

    printf("\n");
    printf("    0 1 2 3 4 5 6 7\n\n");
}

void InitBoard(Board* xo_board)
{
    for (int y = 0; y < N; y += 1)
    {
        for (int x = 0; x < N; x += 1)
        {
            xo_board->arr[y][x] = EMPTY;
        }
    }
}

int CountNum(const Board* xi_board, int color)
{
    int retVal = 0;

    for (int y = 0; y < N; y += 1)
    {
        for (int x = 0; x < N; x += 1)
        {
            if (xi_board->arr[y][x] == color)
            {
                retVal += 1;
            }
        }
    }

    return retVal;
}

void DoWing(Board* xio_board, const Move* xi_move, int dy, int dx)
{
    int moveColor = xi_move->color;
    int otherColor = (xi_move->color == WHITE) ? BLACK : WHITE;

    int y = xi_move->y;
    int x = xi_move->x;

    int sawOtherColor;
    int oob;

    sawOtherColor = 0;
    oob = 0;

    while (1)
    {
        y += dy;
        x += dx;

        if (!IS_BETWEEN(y, 0, N - 1))
        {
            oob = 1;

            break;
        }

        if (!IS_BETWEEN(x, 0, N - 1))
        {
            oob = 1;

            break;
        }

        if (xio_board->arr[y][x] == otherColor)
        {
            sawOtherColor = 1;
        }
        else
        {
            break;
        }
    }

    if ((oob == 0) && (sawOtherColor) && (xio_board->arr[y][x] == moveColor))
    {
        int changeY = xi_move->y + dy;
        int changeX = xi_move->x + dx;

        while (1)
        {
            xio_board->arr[changeY][changeX] = moveColor;

            changeY += dy;
            changeX += dx;

            if ((changeY == y) && (changeX == x))
            {
                break;
            }
        }
    }
}

void DoMove(Board* xio_board, const Move* xi_move)
{
    ASSERT(IS_BETWEEN(xi_move->x, 0, N - 1));
    ASSERT(IS_BETWEEN(xi_move->y, 0, N - 1));

    if (xi_move->color != EMPTY)
    {
        ASSERT(xio_board->arr[xi_move->y][xi_move->x] == EMPTY);
    }
    else
    {
        ASSERT(xio_board->arr[xi_move->y][xi_move->x] != EMPTY);
    }

    if (xi_move->color == EMPTY)
    {
        xio_board->arr[xi_move->y][xi_move->x] = EMPTY;
    }
    else
    {
        xio_board->arr[xi_move->y][xi_move->x] = xi_move->color;

        DoWing(xio_board, xi_move, -1, 0);
        DoWing(xio_board, xi_move, -1, 1);
        DoWing(xio_board, xi_move, 0, 1);
        DoWing(xio_board, xi_move, 1, 1);
        DoWing(xio_board, xi_move, 1, 0);
        DoWing(xio_board, xi_move, 1, -1);
        DoWing(xio_board, xi_move, 0, -1);
        DoWing(xio_board, xi_move, -1, -1);
    }
}

void UndoMove(Board* xio_board, const Move* xi_move)
{
    Move temp;

    temp.color = EMPTY;
    temp.y = xi_move->y;
    temp.x = xi_move->x;

    DoMove(xio_board, &temp);
}

Move FindBestMove(Board* xio_board, int color, int numMovesLeft)
{
    int numPossibleMoves = CountNum(xio_board, EMPTY);

    ASSERT(numPossibleMoves > 0);

    if (numPossibleMoves == 1)
    {
        Move move;

        for (int y = 0; y < N; y += 1)
        {
            for (int x = 0; x < N; x += 1)
            {
                if (xio_board->arr[y][x] == EMPTY)
                {
                    move.color = color;
                    move.y = y;
                    move.x = x;
                }
            }
        }

        return move;
    }
    else if (numMovesLeft == 1)
    {
        Move move;
        Move bestMove;
        int highestNumOfMyColor;

        highestNumOfMyColor = 0;

        move.color = color;
        bestMove.color = color;

        for (int y = 0; y < N; y += 1)
        {
            for (int x = 0; x < N; x += 1)
            {
                if (xio_board->arr[y][x] == EMPTY)
                {
                    Board tempBoard;

                    int numOfMyColor;

                    move.y = y;
                    move.x = x;

                    CopyBoard(&tempBoard, xio_board);

                    DoMove(&tempBoard, &move);

                    numOfMyColor = CountNum(&tempBoard, color);

                    if (numOfMyColor > highestNumOfMyColor)
                    {
                        highestNumOfMyColor = numOfMyColor;

                        bestMove.y = move.y;
                        bestMove.x = move.x;
                    }
                }
            }
        }

        return bestMove;
    }
    else
    {
        Move move;
        Move bestMove;
        int highestNumOfMyColor;

        highestNumOfMyColor = 0;

        move.color = color;
        bestMove.color = color;

        for (int y = 0; y < N; y += 1)
        {
            for (int x = 0; x < N; x += 1)
            {
                if (xio_board->arr[y][x] == EMPTY)
                {
                    Board tempBoard;

                    int numOfMyColor;

                    move.y = y;
                    move.x = x;

                    CopyBoard(&tempBoard, xio_board);

                    DoMove(&tempBoard, &move);

                    Move oppenentsBestMove = FindBestMove(&tempBoard, (color == WHITE) ? BLACK : WHITE, numMovesLeft - 1);

                    DoMove(&tempBoard, &oppenentsBestMove);

                    numOfMyColor = CountNum(&tempBoard, color);

                    if (numOfMyColor > highestNumOfMyColor)
                    {
                        highestNumOfMyColor = numOfMyColor;

                        bestMove.y = move.y;
                        bestMove.x = move.x;
                    }
                }
            }
        }

        return bestMove;
    }
}

int main(void)
{
    time_t t;
    int numberOfGames = 1;
    int numberTimesMachineWon = 0;
    int numberTimesPlayerWon = 0;
    int numberTimesTie = 0;
    Move playerMoves[30];
    int playerMoveIdx;

    t = 1685911528;

    //srand((unsigned)time(&t));
    srand((unsigned)1685911528);

    for (int game = 0; game < numberOfGames; game += 1)
    {
        Board mainBoard;

        InitBoard(&mainBoard);

        playerMoveIdx = 0;

        mainBoard.arr[3][3] = BLACK;
        mainBoard.arr[3][4] = WHITE;
        mainBoard.arr[4][3] = WHITE;
        mainBoard.arr[4][4] = BLACK;

        while (IsGameOver(&mainBoard) == 0)
        {
            int playerY;
            int playerX;

            PrintBoard(&mainBoard);

            printf("enter move(y,x): ");

#if 1
            do
            {
                scanf_s("%d %d", &playerY, &playerX);
            } while (mainBoard.arr[playerY][playerX] != EMPTY);
#else
            do
            {
                playerY = rand() % N;
                playerX = rand() % N;
            } while (mainBoard.arr[playerY][playerX] != EMPTY);
#endif

            Move playerMove;

            playerMove.color = WHITE;
            playerMove.y = playerY;
            playerMove.x = playerX;

            ASSERT(playerMoveIdx < 30);

            playerMoves[playerMoveIdx++] = playerMove;

            DoMove(&mainBoard, &playerMove);

            printf("\n");
            printf("After player's move (%d, %d):\n\n", playerY, playerX);

            PrintBoard(&mainBoard);

            Move machineBestMove = FindBestMove(&mainBoard, BLACK, 5);

            printf("computer playes (%d, %d)\n\n", machineBestMove.y, machineBestMove.x);
            printf("after computer's move (%d, %d):\n\n", machineBestMove.y, machineBestMove.x);

            DoMove(&mainBoard, &machineBestMove);
        }

        PrintBoard(&mainBoard);

        int numWhites = CountNum(&mainBoard, WHITE);
        int numBlacks = CountNum(&mainBoard, BLACK);

        ASSERT(numWhites + numBlacks == N * N);

        printf("Rand seed: %d, game=%d\n", t, game);
        printf("Score:\n");
        printf("Whites %d\n", numWhites);
        printf("Blacks %d\n", numBlacks);

        if (numWhites > numBlacks)
        {
            numberTimesPlayerWon += 1;

            printf("White wins!");
        }
        else if (numWhites < numBlacks)
        {
            numberTimesMachineWon += 1;

            printf("Black wins!");
        }
        else
        {
            numberTimesTie += 1;

            printf("Tie!");
        }
    }

    printf("Machine / Player / Ties: %d, %d, %d\n", numberTimesMachineWon, numberTimesPlayerWon, numberTimesTie);

    _getch();

    return 0;
}