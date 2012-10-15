#include <stdio.h>
#include <SDL/SDL.h>
#include <algorithm>

#include "serial.h"

//#undef main

#define WIDTH 640
#define HEIGHT 480
#define BPP 4
#define DEPTH 32

enum KEYSTATE {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  
  NUM_KEYS
};

int keyboard_state[NUM_KEYS] = {0};

struct SteeringState {
	float forward_speed;
	float angle;

	float speed_left;
	float speed_right;
};

//typedef struct SteeringState SteeringState;

void draw(SteeringState* state) {
//    SDL_

    printf("Forward Speed: %f\tAngle: %f\n", state->forward_speed, state->angle);
}

void fill_command(SteeringState* state, char command[4]) {

    command[0] = state->speed_left;
    command[1] = state->speed_right;
    
    command[2] = 255; // Maximum Command Timeout
    command[3] = '\n';
    
}

void process_input(SteeringState* state, float dt) {
    const float MAX_SPEED = 127.0;
    const float MIN_SPEED = -127.0;
    
    printf("dt: %f\n", dt);

    state->speed_left = 0;
    state->speed_right = 0;

    bool turning = !keyboard_state[UP] && !keyboard_state[DOWN];
    
    if (keyboard_state[UP]) {
    	state->speed_left = 127;
    	state->speed_right = 127;
    } else if (keyboard_state[DOWN]) {
        state->speed_left = -127;
        state->speed_right = -127;
    }
    
    if (keyboard_state[LEFT]) {
	if (turning) {
             state->speed_left = -127;
             state->speed_right = 127;
        } else {
             state->speed_left /= 4;
        }
    }
    
    if (keyboard_state[RIGHT]) {
        if (turning) {
            state->speed_left = 127;
            state->speed_right = -127;
        } else {
            state->speed_right /= 4;
        }
    }
    
}

void serial_sendsteering(SteeringState* state) {
    char command[4];
    fill_command(state, command);
    
    serial_write(command, 4);
}


int main(int argc, char* argv[])
{
    if (argc < 2) {
        puts("Usage: rccontrol TTYPORT");
        exit(1);
    }
    
    serial_init(argv[1]);
    
    SDL_Surface *screen;
    SDL_Event event;
  
    int running = 1;
    int h=0; 
    
    SteeringState steeringState;
    
    steeringState.forward_speed = 0;
    steeringState.angle = 0;
  
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;
   
    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_SWSURFACE | SDL_DOUBLEBUF)))
    {
        SDL_Quit();
        return 1;
    }
    
    int lastTime = SDL_GetTicks();
  
    while(running) 
    {

         while(SDL_PollEvent(&event)) 
         {      
              switch (event.type) 
              {
                  case SDL_QUIT:
	                running = 0;
	                break;
                  case SDL_KEYDOWN:
                      switch (event.key.keysym.sym) {
                          case SDLK_q:                    
		                    running = 0;
                            break;
                            /*
                        case SDLK_UP:
                            keyboard_state[UP] = 1;
                            break;
                        case SDLK_DOWN:
                            keyboard_state[DOWN] = 1;
                            break;
                        case SDLK_LEFT:
                            keyboard_state[LEFT] = 1;
                            break;
                        case SDLK_RIGHT:
                            keyboard_state[RIGHT] = 1;
                            break;
                            */
                        }
                }
                
                Uint8* keystate = SDL_GetKeyState(0);
                keyboard_state[UP] = keystate[SDLK_UP];
                keyboard_state[DOWN] = keystate[SDLK_DOWN];
                keyboard_state[LEFT] = keystate[SDLK_LEFT];
                keyboard_state[RIGHT] = keystate[SDLK_RIGHT];
         }
         
         
         int curTime = SDL_GetTicks();
         
         float dt = float(curTime - lastTime) / 1000.0;
         lastTime = curTime;
         process_input(&steeringState, dt);
	     draw(&steeringState);
         serial_sendsteering(&steeringState);
        SDL_Flip(screen);
        
        SDL_Delay(15);


	char buf[1024];
	int length = 0;
    int c;
	while (length < 1024 && (c = serial_selectread()) != 0) {
	    buf[length++] = c;
	}

	buf[length] = 0;
	length = 0;
	puts(buf);
        
    }
    SDL_Quit();
  
    return 0;
}




