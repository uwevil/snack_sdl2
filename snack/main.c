/*
 SNACK SDL2
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "SDL2/SDL.h"


const enum DIR {UP, DOWN, LEFT, RIGHT, NONE};

const long AUTO_TIME = 0.01; // 0.01 seconde

int width, height, rectSize;

int black, white, red, blue;

int backgroundColor;

typedef struct p {
    SDL_Rect *rect;
    enum DIR dir;
    struct p *next;
    struct p *prev;
    struct p *tail;
} Pile;


Pile * create_pile(SDL_Rect *rect, enum DIR dir) {
    Pile *head = malloc(sizeof(Pile));
    head->rect = malloc(sizeof(SDL_Rect));
    head->rect->x = rect->x;
    head->rect->y = rect->y;
    head->rect->w = rect->w;
    head->rect->h = rect->h;
    head->dir = dir;
    head->prev = NULL;
    head->next = NULL;
    head->tail = head;
    return head;
}


Pile * add_head(Pile *p, SDL_Rect *rect, enum DIR dir) {
    Pile *head = malloc(sizeof(Pile));
    head->rect = malloc(sizeof(SDL_Rect));
    head->rect->x = rect->x;
    head->rect->y = rect->y;
    head->rect->w = rect->w;
    head->rect->h = rect->h;
    head->dir  = dir;
    head->prev = NULL;
    head->next = p;
    head->tail = p->tail;

    p->prev = head;
    p->tail = NULL;

    return head;
}

Pile * add_tail(Pile *p, int rectSize) {
    Pile *tail = malloc(sizeof(Pile));

    if (p->tail == NULL){
        p->tail = tail;
        p->next = tail;
        tail->prev = p;
        tail->next = NULL;
        tail->dir  = p->dir;
    }else{
        tail->prev = p->tail;
        tail->next = NULL;
        tail->dir  = p->tail->dir;
        p->tail->next = tail;
        p->tail = tail;
    }

    SDL_Rect *r_tmp = tail->prev->rect;

    switch (tail->dir) {
        case UP:
            tail->rect = malloc(sizeof(SDL_Rect));
            tail->rect->x = r_tmp->x;
            tail->rect->y = r_tmp->y+rectSize;
            tail->rect->w = r_tmp->w;
            tail->rect->h = r_tmp->h;
            break;
        case DOWN:
            tail->rect = malloc(sizeof(SDL_Rect));
            tail->rect->x = r_tmp->x;
            tail->rect->y = r_tmp->y-rectSize;
            tail->rect->w = r_tmp->w;
            tail->rect->h = r_tmp->h;
            break;
        case LEFT:
            tail->rect = malloc(sizeof(SDL_Rect));
            tail->rect->x = r_tmp->x+rectSize;
            tail->rect->y = r_tmp->y;
            tail->rect->w = r_tmp->w;
            tail->rect->h = r_tmp->h;
            break;
        case RIGHT:
            tail->rect = malloc(sizeof(SDL_Rect));
            tail->rect->x = r_tmp->x-rectSize;
            tail->rect->y = r_tmp->y;
            tail->rect->w = r_tmp->w;
            tail->rect->h = r_tmp->h;
            break;
        default:
            printf("Erreur add_tail\n");
            exit(1);
    }

    return p;
}

Pile * remove_tail(Pile *p) {
    Pile *tail = p->tail;

    p->tail = tail->prev;
    p->tail->next = NULL;

    return tail;
}


SDL_Window * init(char *title, int width, int height)
{
	/* Initialiser de SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("Problème de l'initialisation de SDL: %s\n", SDL_GetError());
		exit(1);
	}

	/* Créer une fenêtre SDL */
    SDL_Window *screen = SDL_CreateWindow(title,
                          SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED,
                          width, height,
                          0);

	if (screen == NULL){
		printf("Problème de la création d'une fenêtre SDL: %s\n", SDL_GetError());
		exit(1);
	}

    return screen;
}

/*Création d'une surface*/
SDL_Surface * createSurface (int width, int height){
    SDL_Surface *surface;
    surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        SDL_Log("Problème de la création d'une surface SDL: %s", SDL_GetError());
        exit(1);
    }

    return surface;
}

/*Fermer SDL*/
void cleanup(){
	SDL_Quit();
}

/*Récupération des entrées au clavier*/
enum DIR getInput()
{
	SDL_Event event;
    enum DIR direction = NONE;

	while (SDL_PollEvent(&event)){
		switch (event.type){
			case SDL_QUIT:
				exit(0);
			    break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE:
						exit(0);
					    break;
                    case SDLK_UP:
                        direction = UP;
                        break;
                    case SDLK_DOWN:
                        direction = DOWN;
                        break;
                    case SDLK_LEFT:
                        direction = LEFT;
                        break;
                    case SDLK_RIGHT:
                        direction = RIGHT;
                        break;
					default:
					    break;
				}
			break;
		}
	}

    return direction;
}

/*Gérer la tête du serpent*/
SDL_Rect * updateRect(SDL_Window *screen,
                      SDL_Surface *surface,
                      SDL_Rect *rect,
                      enum DIR dir,
                      int width,
                      int height,
                      int colorOn,
                      int colorOff){
    switch(dir) {
        case UP:
            rect->y -= rectSize;
            if (rect->y < 0) {
                rect->y = height-rectSize;
            }
            break;
        case DOWN:
            rect->y += rectSize;
            if (rect->y > height-rectSize) {
                rect->y = 0;
            }
            break;
        case LEFT:
            rect->x -= rectSize;
            if (rect->x < 0) {
                rect->x = width-rectSize;
            }
            break;
        case RIGHT:
            rect->x += rectSize;
            if (rect->x > width-rectSize) {
                rect->x = 0;
            }
            break;
        default:
            break;
    }

    return rect;
}

/*Remplir d'un rectangle*/
void set_rect(SDL_Surface *surface, SDL_Rect *rect, int color){
    SDL_FillRect(surface, rect, color);
}

/*Générer aléatoirement d'un rectangle*/
SDL_Rect gen_rect(int width, int height, int rectSize) {
    SDL_Rect rect;
    rect.x = -1;
    rect.y = -1;
    rect.w = rectSize;
    rect.h = rectSize;
    while(rect.x < 0 || rect.x > width-rectSize){
        rect.x = rand()%(width-rectSize+1);
    }
    while(rect.y < 0 || rect.y > height-rectSize){
        rect.y = rand()%(height-rectSize+1);
    }
    return rect;
}

/*Retourner 1 si 2 rectangles sont touchés, 0 sinon*/
int check_intersection(SDL_Rect rect1, SDL_Rect rect2, int rectSize){
    int is_in = (rect1.x >= rect2.x && rect1.x <= rect2.x+rectSize) ||
        (rect1.x+rectSize >= rect2.x && rect1.x+rectSize <= rect2.x+rectSize);
    is_in = is_in && ((rect1.y >= rect2.y && rect1.y <= rect2.y+rectSize) ||
        (rect1.y+rectSize >= rect2.y && rect1.y+rectSize <= rect2.y+rectSize));

    return is_in;
}

/*Retourner 1 si 2 rectangles sont touchés, 0 sinon*/
int eat_yourself(SDL_Rect rect1, SDL_Rect rect2, int rectSize){
    return rect1.x == rect2.x && rect1.y == rect2.y;
}

/*Mettre à jour le serpent*/
int update_pile(Pile *p, SDL_Surface *surface, int headColor, int color, rectSize){
    Pile *tmp  = p;
    Pile *head = p;

    int count  = 0;
    SDL_FillRect(surface, p->rect, headColor);

    while(tmp->next != NULL){
        SDL_Rect *r_tmp = tmp->next->rect;
        if (count > 3 && eat_yourself(*(head->rect), *r_tmp, rectSize)){
            return 1;
        }

        SDL_FillRect(surface, r_tmp, color);
        tmp = tmp->next;
        count++;
    }

    return 0;
}

/*Vérifier si la direction choisie par l'utilisateur est valide*/
int check_dir(enum DIR current, enum DIR new){
    switch(new){
        case UP:
            if (current == DOWN) {
                return 0;
            }
            break;
        case DOWN:
            if (current == UP) {
                return 0;
            }
            break;
        case LEFT:
            if (current == RIGHT) {
                return 0;
            }
            break;
        case RIGHT:
            if (current == LEFT) {
                return 0;
            }
            break;
        default:
            return 0;
    }
    return 1;
}


int main(int argc, char *argv[]) {
    time_t now, last=0;

    now = time(NULL);
    srand(now);

    width  = 640;
    height = 480;
    rectSize = width / 30;

	SDL_Window *screen   = init("Serpent", width, height);
    SDL_Surface *surface = SDL_GetWindowSurface(screen);

    SDL_Rect rect;
    rect.x = (width/2)+rectSize;
    rect.y = (height/2)+rectSize;
    rect.w = rectSize;
    rect.h = rectSize;

    backgroundColor = SDL_MapRGBA(surface->format, 130, 197, 158, 200);
    SDL_FillRect(surface, NULL, backgroundColor);

    white = SDL_MapRGBA(surface->format, 255, 255, 255, 200);
    black = SDL_MapRGBA(surface->format, 0, 0, 0, 0);
    red   = SDL_MapRGBA(surface->format, 255, 0, 0, 200);
    blue  = SDL_MapRGBA(surface->format, 0, 0, 255, 200);

	/* Lorsque le programme s'arrête, on appelle la fonction "cleanup" */
	atexit(cleanup);

    enum DIR dir         = UP;
    enum DIR current_dir = UP;

    SDL_Rect food = gen_rect(width, height, rectSize);
    int is_eat=0;

    SDL_FillRect(surface, &food, red);

    Pile *head = create_pile(&rect, current_dir);
    Pile *tail;

    SDL_UpdateWindowSurface(screen);

	while (1){
		dir  = getInput();
        last = now;
        now  = time(NULL);

        if (check_dir(current_dir, dir)) {
            current_dir = dir;
            rect = *(updateRect(screen, surface, &rect, current_dir,
                width, height, blue, backgroundColor));
            head = add_head(head, &rect, current_dir);
            tail = remove_tail(head);
            set_rect(surface, tail->rect, backgroundColor);
            free(tail);
            if (update_pile(head, surface, blue, white, rectSize)){
                printf("PERDUUUU\n");
                exit(0);
            }
            SDL_UpdateWindowSurface(screen);
        }

        if (now - last > AUTO_TIME) {
            rect = *(updateRect(screen, surface, &rect, current_dir,
                width, height, blue, backgroundColor));
            head = add_head(head, &rect, current_dir);
            tail = remove_tail(head);
            set_rect(surface, tail->rect, backgroundColor);
            free(tail);
            if (update_pile(head, surface, blue, white, rectSize)){
                printf("PERDUUUU\n");
                exit(0);
            }
            SDL_UpdateWindowSurface(screen);
        }

        is_eat = check_intersection(rect, food, rectSize);

        if (is_eat) {
            SDL_FillRect(surface, &food, backgroundColor);
            SDL_FillRect(surface, head->rect, blue);
            SDL_UpdateWindowSurface(screen);

            head = add_tail(head, rectSize);
            food = gen_rect(width, height, rectSize);

            SDL_FillRect(surface, &food, red);
        }

        if (update_pile(head, surface, blue, white, rectSize)){
            printf("PERDUUUU\n");
            exit(0);
        }

		/* Pause 16 milisecondes pour ne pas prendre trop de CPU */
		SDL_Delay(16);
	}

	return 0;
}
