#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


#define MAZE_X 150
#define MAZE_Y 50

#define WALL_CHANCE 42


struct element 
{
    int x;
    int y;
    struct element *prev;
};

struct element *top = NULL;


struct element* pop()
{
    if(top != NULL)
    {
        struct element *el = top;
        if(top->prev != NULL)
        {
            top = top->prev;
        }
        else
        {
            top = NULL;
        }
        return el;
    }
    else
    {
        return NULL;
    }
    
}

void push(int y, int x) {
    struct element *el = (struct element*)malloc(sizeof(struct element));
    el->x = x;
    el->y = y;
    el->prev = top;
    top = el;
}


int check_bordering_end(int *x, int *y, int maze[MAZE_Y][MAZE_X])
{
    if(*y + 1 <= MAZE_Y - 1)
    {
        if(maze[*x][*y + 1] == 2)
        {
            *y += 1;
            return 1;
        }
    }
    else if(*x + 1 <= MAZE_X - 1)
    {
        if(maze[*x + 1][*y] == 2)
        {
            *x += 1;
            return 1;
        }
    }
    if(*y - 1 >= 0)
    {
        if(maze[*x][*y - 1] == 2)
        {
            *y -= 1;
            return 1;
        }
    }
    else if(*x - 1 >= 0)
    {
        if(maze[*x - 1][*y] == 2)
        {
            *x -= 1;
            return 1;
        }
    }
    return 0;
}


int traverse_tile(int *y, int *x, int maze[MAZE_Y][MAZE_X], int traversed[MAZE_Y][MAZE_X])
{
    //printf("position: %d, %d\n", *y, *x);
    //can it go down? (not out of bounds, not a wall, hasn't been visited?)
    if(*y + 1 <= MAZE_Y - 1)
    {
        if(maze[*y + 1][*x] && !traversed[*y + 1][*x])
        {
            *y += 1;
            return 0;
        }
    }
    //can it go right?
    if(*x + 1 <= MAZE_X - 1)
    {
        if(maze[*y][*x + 1] && !traversed[*y][*x + 1])
        {
            *x += 1;
            return 0;
        }
    }
    //can it go left?
    if(*x - 1 >= 0)
    {
        if(maze[*y][*x - 1] && !traversed[*y][*x - 1])
        {
            *x -= 1;
            return 0;
        }
    }
    //can it go up?
    if(*y - 1 >= 0)
    {
        if(maze[*y - 1][*x] && !traversed[*y - 1][*x])
        {
            *y -= 1;
            return 0;
        }
    }
    return 1;
}


void output_maze_and_path(int maze[MAZE_Y][MAZE_X], int endY, int endX, int startX, int startY, int traversed[MAZE_Y][MAZE_X], int headx, int heady)
{
    for(int i = 0; i < MAZE_Y; i++)
    {
        for(int j = 0; j < MAZE_X; j++)
        {
            if(i == heady && j == headx)
            {
                //head: magenta
                printf("\033[35;45mD");
            }
            else if(i == startY && j == startX)
            {
                //start: yellow
                printf("\033[30;40mS");
            }
            else if(i == endY && j == endX)
            {
                //goal: cyan
                printf("\033[36;46mO");
            }
            else if(maze[i][j] == 0)
            {
                //blocked path: red
                printf("\033[31;41mX");
            }
            else if(maze[i][j] == 1)
            {
                if(traversed[i][j])
                {
                    //traversed path: grey
                    printf("\033[33;43m-");
                }
                else
                {
                    //available path: grey
                    printf("\033[37;47m-");
                }
                
            }
            else if(maze[i][j] == 3)
            {
                //taken path: green
                printf("\033[32;42m=");
            }
        }
        printf("\n");
    }
    printf("\033[0m \n");
}


void generate_maze(int maze[MAZE_Y][MAZE_X])
{
    for(int i = 0; i < MAZE_Y; i++)
    {
        for(int j = 0; j < MAZE_X; j++)
        {
            if(rand() % 100 < WALL_CHANCE)
            {
                maze[i][j] = 0;
            }
            else
            {
                maze[i][j] = 1;
            }
        }
    }
    //maze[0][0] = 1;
    //maze[MAZE_Y - 1][MAZE_X - 1] = 2;
}


int main() 
{
    system("clear");
    srand(time(NULL));

    //maze:
    //0 = wall
    //1 = traversable
    //2 = cheese
    int maze[MAZE_Y][MAZE_X] = {{0}};

    //identifies which tiles have been traversed and which haven't
    //0 = not traversed
    //1 = traversed
    int traversed[MAZE_Y][MAZE_X] = {{0}};

    int foundEnd = 0;

    int validMaze = 0;

    int startX = 0;
    int startY = 0;

    int endY = MAZE_Y - 1;
    int endX = MAZE_X - 1;

    int count = 0;

    //generate a valid maze to then solve
    while(!validMaze)
    {
        count++;
        printf("attempts: %d\n", count);
        //generate maze
        generate_maze(maze);

        maze[startY][startX] = 1;
        maze[endY][endX] = 2;

        //start position
        push(startY, startX);

        //system("clear");
        //output_maze_and_path(maze, endY, endX, startY, startX);
        //getchar();

        foundEnd = 0;

        while(!foundEnd)
        {
            int deadEnd = 1;

            //needs to pop current position
            struct element *el = pop();

            if(el != NULL)
            {
                int x = el->x;
                int y = el->y;

                //is a neighbouring tile the end?
                if(!check_bordering_end(&y, &x, maze))
                {
                    deadEnd = traverse_tile(&y, &x, maze, traversed);
                    //if there is a new tile that can be traversed to, do so  
                    if(!deadEnd)
                    {
                        //to traverse, repush the currently held element
                        push(el->y, el->x);
                        //set traversed to true on the current tile
                        traversed[el->y][el->x] = 1;
                        //then also push the next tile to visit
                        push(y, x);
                        //free memory
                        free(el);
                    }
                    //otherwise, dead end has been reached, so no need to repush the current tile. 
                    else
                    {
                        //set traversed to true on the current tile
                        traversed[el->y][el->x] = 1;
                        //printf("dead end at %d %d\n", el->y, el->x);
                        //free memory
                        free(el);
                    }
                }
                else
                {
                    //goal found, push tiles back on to result in final stack
                    push(el->y, el->x);
                    push(x, y);
                    foundEnd = 1;
                }        
            }
            else
            {
                foundEnd = 2;
            }
        }

        

        //full unpop stack to reveal path
        if(foundEnd == 1)
        {
            validMaze = 1;
            //printf("valid maze!\n");
        }
        else
        {
            //printf("invalid\n");
        }
        

        struct element *el = (struct element*)malloc(sizeof(struct element));
        el->prev = top;

        while(el->prev != NULL)
        {
            el = pop();
        }

        el = pop();

        //output_maze_and_path(traversed, endY, endX, startY, startX);
        //getchar();

        for(int i = 0; i < MAZE_Y; i++)
        {
            for(int j = 0; j < MAZE_X; j++)
            {
                traversed[i][j] = 0;
            }
        }
    }

    system("clear");

    printf("total invalid attempts: %d\n", count);

    int headx = -1;
    int heady = -1;

    //output starting maze
    output_maze_and_path(maze, endY, endX, startY, startX, traversed, headx, heady);

    getchar();

    //start position
    push(startY, startX);
    maze[endY][endX] = 2;

    foundEnd = 0;

    count = 0;

    //traverse solvable maze
    while(!foundEnd)
    {
        count++;
        system("clear");
        int deadEnd = 1;

        //needs to pop current position
        struct element *el = pop();

        if(el != NULL)
        {
            int x = el->x;
            int y = el->y;

            headx = x;
            heady = y;

            //is a neighbouring tile the end?
            if(!check_bordering_end(&y, &x, maze))
            {
                deadEnd = traverse_tile(&y, &x, maze, traversed);
                //if there is a new tile that can be traversed to, do so  
                if(!deadEnd)
                {
                    //to traverse, repush the currently held element
                    push(el->y, el->x);
                    //set traversed to true on the current tile
                    traversed[el->y][el->x] = 1;
                    //then also push the next tile to visit
                    push(y, x);
                    //free memory
                    free(el);
                }
                //otherwise, dead end has been reached, so no need to repush the current tile. 
                else
                {
                    //set traversed to true on the current tile
                    traversed[el->y][el->x] = 1;
                    //printf("dead end at %d %d\n", el->y, el->x);
                    //free memory
                    free(el);
                }
            }
            else
            {
                //goal found, push tiles back on to result in final stack
                push(el->y, el->x);
                push(x, y);
                foundEnd = 1;
            }        
        }

        output_maze_and_path(maze, endY, endX, startY, startX, traversed, headx, heady);
        usleep(50000);
    }

    system("clear");


    //fully unpop stack to see final result
    printf("RESULT: (%d iterations)\n\n", count);
    
    

    struct element *el = (struct element*)malloc(sizeof(struct element));
    el->prev = top;

    while(el->prev != NULL)
    {
        el = pop();
        //printf("%d, %d\n", el->y, el->x);
        maze[el->y][el->x] = 3;
    }

    //el = pop();

    headx = -1;
    heady = -1;

    output_maze_and_path(maze, endY, endX, startY, startX, traversed, headx, heady);
}
