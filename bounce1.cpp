#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
using namespace std;
int sgn(int dx)
{
	if(dx>0)
		dx=1;
	else if(dx<0)
		dx=-1;
	return dx;
}
extern "C" {
#include "SDL.h"
}
Uint32 background, red;
#define NUM_SPRITES     5
#define MAX_SPEED       1
//////////////////////////////////////////////////
class sprite
{
	SDL_Rect sprite_rect;
	SDL_Rect position;
	SDL_Rect velocitie;
	SDL_Surface *photo;
private:
	sprite()
	{
	}
public:
sprite(const char *file)
{
	SDL_Surface *temp;

        /* Load the sprite image */
        photo = SDL_LoadBMP(file);
        if ( photo == NULL ) {
                fprintf(stderr, "Couldn't load %s: %s", file, SDL_GetError());
                throw new int(-1);
        }

        /* Set transparent pixel as the pixel at (0,0) */
        if ( photo->format->palette ) {
                SDL_SetColorKey(photo, (SDL_SRCCOLORKEY|SDL_RLEACCEL),
                                                *(Uint8 *)photo->pixels);
        }

        /* Convert sprite to video format */
        temp = SDL_DisplayFormat(photo);
        SDL_FreeSurface(photo);
        if ( temp == NULL ) {
                fprintf(stderr, "Couldn't convert background: %s\n",
                                                        SDL_GetError());
                throw new int(-1);
        }
        photo = temp;
	position.w=photo->w;
	position.h=photo->h;
	position.x=rand()%900;
	position.y=rand()%800;
}

SDL_Rect *getPosition()
{
	return &position;
}

void blit(SDL_Surface *screen)
{
	SDL_BlitSurface(photo, NULL, screen, &position);
}

void moveto(int x, int y)
{
	position.x= x; position.y=y;	
}

void moveby(SDL_Surface *screen, int dx,int dy)
{
	SDL_FillRect(screen, this->getPosition(), red);
	position.x+=dx;
	position.y+=dy;
	this->blit(screen);
}

~sprite()
{
        SDL_FreeSurface(photo);
}

void chase(SDL_Surface *screen, sprite *target)
{
        SDL_Rect *targetPosition=target->getPosition();
        SDL_Rect *myPosition=this->getPosition();
	int dx = targetPosition->x-myPosition->x;
	int dy = targetPosition->y-myPosition->y;
        dx=(sgn(dx));
        dy=(sgn(dy)); 
	this->moveby(screen, dx,dy);
	collision();
}

bool collision()
{
  
	for(int i=0;i<5;i++)
	{
		for(int j=i+1;j<5;j++)
		{
  			if( (villain[i].x >= villain[j].x && villain[i].x <= (villain[j].x + villain[j].w)) ||
      				((villain[i].x + villain[i].w) >= villain[j].x && (villain[i].x + 
					villain[i].w) <= (villain[j].x + villain[j].w)) )
        		{
   				 if( (villain[i].y >=villain[j].y && villain[i].y <= (villain[j].y + villain[j].h)) ||
        				((villain[i].y + villain[i].h) >= villain[j].y && (villain[i].y + 
						villain[i].h) <= (villain[j].y + villain[j].h)) )
				 {
     
      					return true;
        			 }
        		}
        	}
        }
 	 return false;
}
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class sarang:public sprite
{
public:
	sarang():sprite("sarang.bmp")
	{

	}	
	~sarang()
	{

	}
};
//////////////////////////////////////////////////////////////////////////
class virus : public sprite
{
public:
	virus():sprite("virus.bmp")
	{
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	
	sprite *hero, *villain[5];
	SDL_Surface *screen;
	struct timeval now, prev={0}, diff;
	SDL_Event event;
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		return(1);
	}

	screen = SDL_SetVideoMode(1024, 768, 8, SDL_SWSURFACE|SDL_ANYFORMAT);
	if ( ! screen ) {
		fprintf(stderr, "Couldn't set 1024x768 video mode: %s\n",
				SDL_GetError());
		return -1;
	}

	background = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	SDL_Rect dst={0};
	dst.w=screen->w;
	dst.h=screen->h;
	SDL_FillRect(screen, &dst, red);

	hero=new sarang();
	for(int i=0;i<5;i++)
	{
        	villain[i]=new virus();
        	villain[i]-> blit(screen);
	}
	hero-> moveto(512,768/2);
	hero-> blit(screen);
	int done = 0;
	SDL_EnableKeyRepeat(100,100);
	while ( !done ) {
		if ( (screen->flags & SDL_DOUBLEBUF) == SDL_DOUBLEBUF ) {
			SDL_Flip(screen);
			cout<<"flip"<<endl;
		
		} 
		else {
			SDL_UpdateRects(screen, 1, &dst);
		}

		gettimeofday(&now, NULL);
		timersub(&now, &prev, &diff);
		for(int i=0;i<5;i++)
		{
		//collision();
			if(diff.tv_usec>100000)
			{
				villain[i]->chase(screen, hero);
				villain[i]-> blit(screen);
				prev=now;
			}	
		}
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
				case SDL_MOUSEBUTTONDOWN:
					SDL_WarpMouse(screen->w/2, screen->h/2);
					break;
				case SDL_KEYDOWN:
					cout << event.key.keysym.sym << endl;
					switch(event.key.keysym.sym)
					{
					case SDLK_UP:
						hero->moveby(screen,0,-10);
						break;
					case SDLK_DOWN:
						hero->moveby(screen,0,10);
						break;
					case SDLK_LEFT:
						hero->moveby(screen,-10,0);
						break;
					case SDLK_RIGHT:
						hero->moveby(screen, 10,0);
						break;
					default:
						cout << "Unknown key"<<endl;
					}
					break;
				case SDL_QUIT:
					done = 1;
					break;
				default:
					break;
			}
		}
	}
	delete hero;
	SDL_Quit();
}
