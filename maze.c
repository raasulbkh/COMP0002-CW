#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "graphics.c"
#include <string.h>

#define WindowSize 800
#define Margin 50
#define GridSize 20
#define CellSize ((WindowSize - 2 * Margin) / GridSize)
#define MarkerCount 100
#define BlockCount 60
#define SleepTime 10

typedef enum { NORTH, EAST, SOUTH, WEST } Direction;
typedef enum { EMPTY, HOME, BLOCK, MARKER, PICKED_UP } Type;

typedef struct node 
{
    int x, y;
    struct node* next;
} Node;

typedef struct
{
    Node* head;
    Node* tail;
} Queue;

typedef struct 
{
    Type getType[GridSize][GridSize];
    int robotX, robotY;
    Direction dir;
    int homeSquareX, homeSquareY;
    bool carryMarker;
} State;

// Initialize the queue
void initQueue(Queue* q) 
{
    q->head = NULL;
    q->tail = NULL;
}

// Add node to the queue
void push(Queue* q, int x, int y) 
{
    Node* newNode = malloc(sizeof(Node));
    newNode->next = NULL;
    newNode->x = x;
    newNode->y = y;
    if (q->head == NULL) 
    {
        q->head = newNode;
        q->tail = newNode;
        return;
    }
    q->tail->next = newNode;
    q->tail = newNode;
}

// Remove node from the queue
void pop(Queue* q) 
{
    if (q->head == q->tail) 
    {
        free(q->head);
        q->head = NULL;
        q->tail = NULL;
        return;
    }
    Node* currentDirection = q->head;
    q->head = q->head->next;
    free(currentDirection);
}

// Get x-coordinate of front node
int getturnTopX(Queue* q) 
{
    return q->head->x;
}

// Get y-coordinate of front node
int getturnTopY(Queue* q) 
{
    return q->head->y;
}

// Check if queue is empty
bool empty(Queue* q) 
{
    return q->head == NULL;
}

// Convert grid coordinate to pixel coordinate
int getPx(int x) 
{
    return Margin + x * CellSize;
}

// Initialize graphics window
void initializeWindow()
{
    setWindowSize(WindowSize, WindowSize);
}

// Initialize game state
void initState(State* state) 
{
    state->homeSquareX = 0;
    state->homeSquareY = 0;
    state->robotX = 0;
    state->robotY = 0;
    state->dir = EAST;
    state->carryMarker = false;
    for (int i = 0; i < GridSize; i++) 
    {
        for (int j = 0; j < GridSize; j++) 
        {
            state->getType[i][j] = EMPTY;
        }
    }
}

// Draw grid lines
void drawGrid()
{
    for (int i = Margin; i <= WindowSize - Margin; i += CellSize)
    {
        drawLine(Margin, i, WindowSize - Margin, i);
        drawLine(i, Margin, i, WindowSize - Margin);
    }
}

// Draw the robot
void drawRobot(State* state)
{
    int x = getPx(state->robotX);
    int y = getPx(state->robotY);
    setColour(green);
    int triangleX[3], triangleY[3];
    switch (state->dir)
    {
    case NORTH:
        triangleX[0] = x + CellSize / 2;
        triangleY[0] = y;
        triangleX[1] = x;
        triangleY[1] = y + CellSize;
        triangleX[2] = x + CellSize;
        triangleY[2] = y + CellSize;
        break;
    case EAST:
        triangleX[0] = x + CellSize;
        triangleY[0] = y + CellSize / 2;
        triangleX[1] = x;
        triangleY[1] = y;
        triangleX[2] = x;
        triangleY[2] = y + CellSize;
        break;
    case SOUTH:
        triangleX[0] = x + CellSize / 2;
        triangleY[0] = y + CellSize;
        triangleX[1] = x;
        triangleY[1] = y;
        triangleX[2] = x + CellSize;
        triangleY[2] = y;
        break;
    case WEST:
        triangleX[0] = x;
        triangleY[0] = y + CellSize / 2;
        triangleX[1] = x + CellSize;
        triangleY[1] = y;
        triangleX[2] = x + CellSize;
        triangleY[2] = y + CellSize;
        break;
    }
    fillPolygon(3, triangleX, triangleY);
    if (state->carryMarker)
    {
        int markerSizeWhileBeingCarriedX = CellSize / 4;
        int markerSizeWhileBeingCarriedY = CellSize / 4;
        setColour(blue);
        fillRect(x + markerSizeWhileBeingCarriedX, y + markerSizeWhileBeingCarriedY, CellSize / 2, CellSize / 2);
    }
}

// Draw home square
void drawHomeSquare(State* state)
{
    int x = getPx(state->homeSquareX);
    int y = getPx(state->homeSquareY);
    setColour(gray);
    int squareX[4] = { x, x + CellSize, x + CellSize, x };
    int squareY[4] = { y, y, y + CellSize, y + CellSize };
    fillPolygon(4, squareX, squareY);
}

// Draw marker at specified coordinates
void drawMarker(int x, int y)
{
    x = getPx(x);
    y = getPx(y);
    setColour(blue);
    fillRect(x, y, CellSize, CellSize);
}

// Draw block at specified coordinates
void drawBlock(int x, int y)
{
    x = getPx(x);
    y = getPx(y);
    setColour(black);
    fillRect(x, y, CellSize, CellSize);
}

// Generate random markers in the maze
void initializeRandomMarkers(State* state)
{
    for (int i = 0; i < MarkerCount; i++)
    {
        int x, y;
        do
        {
            x = rand() % GridSize;
            y = rand() % GridSize;
        } 
        while (state->getType[x][y] != EMPTY);
        state->getType[x][y] = MARKER;
    }
}

// Generate random blocks in the maze
void initializeRandomBlocks(State* state)
{
    for (int i = 0; i < BlockCount; i++)
    {
        int x, y;
        do
        {
            x = rand() % GridSize;
            y = rand() % GridSize;
        } 
        while (state->getType[x][y] != EMPTY);
        state->getType[x][y] = BLOCK;
    }
}

// Convert string direction to enum
Direction getDir(const char* dir) 
{
    if (strcmp(dir, "east") == 0) 
    {
        return EAST;
    }
    if (strcmp(dir, "west") == 0) 
    {
        return WEST;
    }
    if (strcmp(dir, "north") == 0) 
    {
        return NORTH;
    }
    if (strcmp(dir, "south") == 0) 
    {
        return SOUTH;
    }
    printf("ERROR IN ARGUMENTS");
    return 1;
}

// Check if robot is at home
bool atHome(State* state)
{
    return (state->robotX == state->homeSquareX && state->robotY == state->homeSquareY);
}

// Clear robot from its current position
void clearRobot(State* state)
{
    int x = getPx(state->robotX);
    int y = getPx(state->robotY);
    setColour(white);
    fillRect(x, y, CellSize, CellSize);
    setColour(black);
    drawLine(x, y, x + CellSize, y);
    drawLine(x, y, x, y + CellSize);
    if (x + CellSize == WindowSize - Margin) 
    {
        drawLine(x + CellSize, y, x + CellSize, y + CellSize);
    }
    if (y + CellSize == WindowSize - Margin) 
    {
        drawLine(x, y + CellSize, x + CellSize, y + CellSize);
    }
    if (atHome(state)) 
    {
        drawHomeSquare(state);
    }
}

// Pick up marker if robot is on it
void pickUpMarker(State* state)
{
    if (state->getType[state->robotX][state->robotY] == MARKER) {
        state->carryMarker = true;
        state->getType[state->robotX][state->robotY] = PICKED_UP;
        drawRobot(state);
    }
}

// Move robot one step forward
void forward(State* state)
{
    sleep(SleepTime);
    clearRobot(state);
    switch (state->dir)
    {
    case NORTH:
        state->robotY -= 1;
        break;
    case EAST:
        state->robotX += 1;
        break;
    case SOUTH:
        state->robotY += 1;
        break;
    case WEST:
        state->robotX -= 1;
        break;
    }
    pickUpMarker(state);
    drawRobot(state);
}

// Rotate robot to face specified direction
void turnturnTo(State* state, Direction intendedDirection) 
{
    int currentDirection = state->dir;
    int turnTo = intendedDirection;
    if (currentDirection == turnTo) 
        return;
    int turnToRight;
    int turnToLeft;
    if (currentDirection < turnTo) 
    {
        turnToRight = turnTo - currentDirection;
        turnToLeft = 4 - turnToRight;
    }
    if (currentDirection > turnTo) 
    {
        turnToLeft = currentDirection - turnTo;
        turnToRight = 4 - turnToLeft;
    }
    int dir;
    if (turnToLeft <= turnToRight) 
    {
        dir = 3; //left
    }
    else dir = 1; //right
    while (state->dir != intendedDirection) 
    {
        sleep(SleepTime);
        clearRobot(state);
        state->dir = (state->dir + dir) % 4;
        drawRobot(state);
    }
}

// Direct robot to turn to intended direction and move forward
void goturnTo(State* state, Direction intendedDirection) 
{
    turnturnTo(state, intendedDirection);
    forward(state);
}

// Calculate the opposite direction
Direction opposite(Direction d) 
{
    return (d + 2) % 4;
}

// Initialize BFS search
void initializeBFS(State* state, Direction cameFrom[GridSize][GridSize], bool vis[GridSize][GridSize], Queue* q) {
    memset(vis, false, sizeof(bool) * GridSize * GridSize);
    initQueue(q);
    push(q, state->homeSquareX, state->homeSquareY);
    vis[state->homeSquareX][state->homeSquareY] = true;
}

// Perform BFS to find the closest marker
int performBFS(State* state, Direction cameFrom[GridSize][GridSize], bool vis[GridSize][GridSize], Queue* q, int* markerX, int* markerY) {
    int diffX[] = { 0, +1, 0, -1 };
    int diffY[] = { -1, 0, +1, 0 };

    while (!empty(q)) 
    {
        int currentDirectionX = getturnTopX(q);
        int currentDirectionY = getturnTopY(q);
        pop(q);
        if (state->getType[currentDirectionX][currentDirectionY] == MARKER) 
        {
            *markerX = currentDirectionX;
            *markerY = currentDirectionY;
            return 1;
        }
        for (int i = 0; i < 4; i++) 
        {
            int nextX = currentDirectionX + diffX[i];
            int nextY = currentDirectionY + diffY[i];
            Direction d = i;
            if (nextX >= 0 && nextX < GridSize && nextY >= 0 && nextY < GridSize && !vis[nextX][nextY] && state->getType[nextX][nextY] != BLOCK) 
            {
                vis[nextX][nextY] = true;
                cameFrom[nextX][nextY] = opposite(d);
                push(q, nextX, nextY);
            }
        }
    }
    return -1;
}

// Trace back path from marker to home square
int traceBackPath(State* state, Direction* directions, Direction cameFrom[GridSize][GridSize], int markerX, int markerY) {
    int turnTotalDist = 0;
    int currentDirectionX = markerX;
    int currentDirectionY = markerY;

    while (currentDirectionX != state->homeSquareX || currentDirectionY != state->homeSquareY) 
    {
        directions[turnTotalDist++] = opposite(cameFrom[currentDirectionX][currentDirectionY]);
        switch (cameFrom[currentDirectionX][currentDirectionY]) 
        {
        case NORTH:
            currentDirectionY -= 1;
            break;
        case EAST:
            currentDirectionX += 1;
            break;
        case SOUTH:
            currentDirectionY += 1;
            break;
        case WEST:
            currentDirectionX -= 1;
            break;
        }
    }

    for (int i = 0; i < turnTotalDist / 2; i++) 
    {
        Direction tmp = directions[i];
        directions[i] = directions[turnTotalDist - i - 1];
        directions[turnTotalDist - i - 1] = tmp;
    }

    return turnTotalDist;
}

// Find the closest marker using BFS and trace back the path
int closestMarker(State* state, Direction* directions) 
{
    Direction cameFrom[GridSize][GridSize];
    bool vis[GridSize][GridSize];
    Queue q;
    int markerX = -1;
    int markerY = -1;

    initializeBFS(state, cameFrom, vis, &q);

    if (performBFS(state, cameFrom, vis, &q, &markerX, &markerY) == -1) 
    {
        return -1;
    }

    return traceBackPath(state, directions, cameFrom, markerX, markerY);
}

// Continuously find and pick up all reachable markers
void pickUpAllMarkers(State* state) 
{
    int turnTotalDist;
    do 
    {
        Direction directions[GridSize * GridSize * GridSize];
        turnTotalDist = closestMarker(state, directions);
        if (turnTotalDist == -1) 
        {
            return;
        }
        for (int i = 0; i < turnTotalDist; i++) 
        {
            goturnTo(state, directions[i]);
        }
        for (int i = turnTotalDist - 1; i >= 0; i--) 
        {
            goturnTo(state, opposite(directions[i]));
        }
        drawHomeSquare(state);
        state->carryMarker = false;
        drawRobot(state);
    } 
    while (turnTotalDist != -1);
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    if (MarkerCount + BlockCount >= GridSize * GridSize - 1)
    {
        printf("Error: Number of markers and blocks exceeds the limit\n");
        return 1;
    }
    initializeWindow();
    drawGrid();
    State state;
    initState(&state);
    if (argc > 1) 
    {
        state.homeSquareX = atoi(argv[1]);
        state.homeSquareY = atoi(argv[2]);
        state.dir = getDir(argv[3]);
    }
    state.getType[state.homeSquareX][state.homeSquareY] = HOME;
    state.robotX = state.homeSquareX;
    state.robotY = state.homeSquareY;
    initializeRandomMarkers(&state);
    initializeRandomBlocks(&state);
    for (int i = 0; i < GridSize; i++) 
    {
        for (int j = 0; j < GridSize; j++) 
        {
            if (state.getType[i][j] == MARKER) 
            {
                drawMarker(i, j);
            }
            else if (state.getType[i][j] == BLOCK) 
            {
                drawBlock(i, j);
            }
        }
    }
    drawHomeSquare(&state);
    drawRobot(&state);
    pickUpAllMarkers(&state);
    printf("All reachable markers have been collected");
    return 0;
}
