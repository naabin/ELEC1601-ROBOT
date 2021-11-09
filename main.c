#include "stdio.h"
#include "stdlib.h"
#include "sdl.h"
#include "SDL2_gfxPrimitives.h"
#include "time.h"

#include "formulas.h"
#include "wall.h"
#include "robot.h"

int done = 0;


int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        return 1;
    }

    window = SDL_CreateWindow("Robot Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    window = SDL_CreateWindow("Robot Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, 0);

    struct Robot robot;
    struct Wall_collection *head = NULL;
    int front_left_sensor, front_right_sensor=0;
    clock_t start_time, end_time;
    int msec;

    // SETUP MAZE
    // You can create your own maze here. line of code is adding a wall.
    // You describe position of top left corner of wall (x, y), then width and height going down/to right
    // Relative positions are used (OVERALL_WINDOW_WIDTH and OVERALL_WINDOW_HEIGHT)
    // But you can use absolute positions. 10 is used as the width, but you can change this.
    insertAndSetFirstWall(&head, 1,  0, 0, 10, OVERALL_WINDOW_HEIGHT);//1
    insertAndSetFirstWall(&head, 2,  0, 0, OVERALL_WINDOW_WIDTH, 10);//2
    insertAndSetFirstWall(&head, 3,  70, OVERALL_WINDOW_HEIGHT-10, 570, 10);//3
    insertAndSetFirstWall(&head, 4,  OVERALL_WINDOW_WIDTH-10, 0, 10, 410);//4
    //insertAndSetFirstWall(&head, 5,  OVERALL_WINDOW_WIDTH-40, 410, 40, 10);//5
    insertAndSetFirstWall(&head, 6,  70, 0, 10, 70);//6
    insertAndSetFirstWall(&head, 7,  70, 140, 10, 340);//7
    insertAndSetFirstWall(&head, 8,  70, 70, 140, 10);//8
    insertAndSetFirstWall(&head, 9,  70, 140, 70, 10);//9
    insertAndSetFirstWall(&head, 10,  140, 140, 10, 70);//10
    insertAndSetFirstWall(&head, 11,  140, 280, 10, 200);//11
    insertAndSetFirstWall(&head, 12,  210, 70, 10, 270);//12
    insertAndSetFirstWall(&head, 13,  210, 410, 80, 10);//13
    insertAndSetFirstWall(&head, 14,  280, 0, 10, 410);//14
    insertAndSetFirstWall(&head, 15,  350, 70, 70, 10);//15
    insertAndSetFirstWall(&head, 16,  350, 70, 10, 410);//16
    insertAndSetFirstWall(&head, 17,  420, 140, 10, 270);//17
    insertAndSetFirstWall(&head, 18,  490, 70, 70, 10);//18
    insertAndSetFirstWall(&head, 19,  420, 140, 140, 10);//19
    insertAndSetFirstWall(&head, 20,  490, 210, 10, 270);//20
    insertAndSetFirstWall(&head, 21,  560, 70, 10, 80);//21
    insertAndSetFirstWall(&head, 22,  490, 210, 70, 10);//22
    insertAndSetFirstWall(&head, 23,  560, 210, 10, 270);//23

    setup_robot(&robot);
    updateAllWalls(head, renderer);

    SDL_Event event;
    while(!done){
        SDL_SetRenderDrawColor(renderer, 165, 141, 227, 255);
        SDL_RenderClear(renderer);

        //Move robot based on user input commands/auto commands
        if (robot.auto_mode == 1)
            robotAutoMotorMove(&robot, front_left_sensor, front_right_sensor);
        robotMotorMove(&robot);

        //Check if robot reaches endpoint. and check sensor values
        if (checkRobotReachedEnd(&robot, OVERALL_WINDOW_WIDTH-20, OVERALL_WINDOW_HEIGHT-35, 10, 100)){
            end_time = clock();
            msec = (end_time-start_time) * 1000 / CLOCKS_PER_SEC;
            robotSuccess(&robot, msec);
        }
        else if(checkRobotHitWalls(&robot, head))
            robotCrash(&robot);
        //Otherwise compute sensor information
        else {
            front_left_sensor = checkRobotSensorFrontLeftAllWalls(&robot, head);
            if (front_left_sensor>0)
                printf("Getting close on the left. Score = %d\n", front_left_sensor);

            front_right_sensor = checkRobotSensorFrontRightAllWalls(&robot, head);
            if (front_right_sensor>0)
                printf("Getting close on the right. Score = %d\n", front_right_sensor);


        }
        robotUpdate(renderer, &robot);
        updateAllWalls(head, renderer);

        // Check for user input
        SDL_RenderPresent(renderer);
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                done = 1;
            }
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            if(state[SDL_SCANCODE_UP] && robot.direction != DOWN){
                robot.direction = UP;
            }
            if(state[SDL_SCANCODE_DOWN] && robot.direction != UP){
                robot.direction = DOWN;
            }
            if(state[SDL_SCANCODE_LEFT] && robot.direction != RIGHT){
                robot.direction = LEFT;
            }
            if(state[SDL_SCANCODE_RIGHT] && robot.direction != LEFT){
                robot.direction = RIGHT;
            }
            if(state[SDL_SCANCODE_SPACE]){
                setup_robot(&robot);
            }
            if(state[SDL_SCANCODE_RETURN]){
                robot.auto_mode = 1;
                start_time = clock();
            }
        }

        SDL_Delay(120);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("DEAD\n");
}
