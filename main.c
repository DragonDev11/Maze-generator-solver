#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <time.h>

#define SCALE 20

#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 800
#define MAZE_WIDTH 10
#define MAZE_HEIGHT 10

typedef struct Cell{
    int x, y, value;
    int visited;
    struct Cell* pointed_at_cell;
}Cell;

typedef struct Path{
    Cell* caster_p;
    Cell* target_p;
    int highlighted;
}Path;

typedef struct Maze{
    int origin_x, origin_y;
    int size;
    int num_paths;
    Cell* entrance;
    Cell* exit;
    Cell** cells;
    Path* paths;
}Maze;

Cell** get_neighbours(Maze* maze, Cell* cell, int* count){
    printf("getting neighbours...\n");
    int size = 0;
    Cell** neighbours = NULL;

    if (cell->y+1 < maze->size){
        if (maze->cells[cell->y+1][cell->x].pointed_at_cell == cell || cell->pointed_at_cell == &maze->cells[cell->y+1][cell->x]){
            neighbours = (Cell**)realloc(neighbours, (size+1)*sizeof(Cell*));
            if (neighbours == NULL){
                printf("Failed to re-allocate memory.\n");
                return NULL;
            }
            neighbours[size] = &maze->cells[cell->y+1][cell->x];
            size++;
        }
    }

    if (cell->y-1 >= 0){
        if (maze->cells[cell->y-1][cell->x].pointed_at_cell == cell || cell->pointed_at_cell == &maze->cells[cell->y-1][cell->x]){
            neighbours = (Cell**)realloc(neighbours, (size+1)*sizeof(Cell*));
            if (neighbours == NULL){
                printf("Failed to re-allocate memory.\n");
                return NULL;
            }
            neighbours[size] = &maze->cells[cell->y-1][cell->x];
            size++;
        }
    }

    if (cell->x+1 < maze->size){
        if (maze->cells[cell->y][cell->x+1].pointed_at_cell == cell || cell->pointed_at_cell == &maze->cells[cell->y][cell->x+1]){
            neighbours = (Cell**)realloc(neighbours, (size+1)*sizeof(Cell*));
            if (neighbours == NULL){
                printf("Failed to re-allocate memory.\n");
                return NULL;
            }
            neighbours[size] = &maze->cells[cell->y][cell->x+1];
            size++;
        }
    }

    if (cell->x-1 >= 0){
        if (maze->cells[cell->y][cell->x-1].pointed_at_cell == cell || cell->pointed_at_cell == &maze->cells[cell->y][cell->x-1]){
            neighbours = (Cell**)realloc(neighbours, (size+1)*sizeof(Cell*));
            if (neighbours == NULL){
                printf("Failed to re-allocate memory.\n");
                return NULL;
            }
            neighbours[size] = &maze->cells[cell->y][cell->x-1];
            size++;
        } 
    }

    *count = size;
    return neighbours;
}

int DFS(Maze* maze, Cell* cell, Cell*** correct_pathway, int* path_size){
    printf("DFS...\n");
    cell->visited = 1;
    (*correct_pathway)[*path_size] = cell;
    (*path_size)++;

    if (cell == maze->exit){
        return 1;
    }

    int neighbours_count;
    Cell** neighbours = get_neighbours(maze, cell, &neighbours_count);
    if (neighbours == NULL){
        printf("Failed to get the neighbours.\n");
        return 0;
    }

    for (int i=0; i<neighbours_count; i++){
        if (neighbours[i]->visited == 0){
            printf("Checking neighbour %d...\n", i);
            if (DFS(maze, neighbours[i], correct_pathway, path_size) == 1){
                printf("Found exit.\n");
                free(neighbours);
                return 1;
            }
        }
    }

    (*path_size)--;
    (*correct_pathway)[*path_size] = NULL;
    printf("Dead end.\n");
    free(neighbours);
    return 0;
}

Path** solve(Maze* maze){
    printf("Solving...\n");
    Path** correct_path = NULL;
    Cell** correct_cells = (Cell**)malloc(sizeof(Cell*)*maze->size*maze->size);
    if (correct_cells == NULL){
        printf("Failed to allocate memory.\n");
        return NULL;
    }
    Cell* current_cell = maze->entrance;

    int size = 0;

    if (DFS(maze, current_cell, &correct_cells, &size) == 1){
        printf("Maze solved!\n");
        if (size > 1){
            correct_path = (Path**)malloc((size-1)*sizeof(Path*));
            if (correct_path == NULL){
                printf("Failed to allocate memory.\n");
                return NULL;
            }
            // no idea
        }

        for (int i=0; i<size; i++){
            if (correct_cells[i]->value == 0) correct_cells[i]->value = 5;
        }
    }

    printf("path size: %d\n", size);
    free(correct_path);
    free(correct_cells);
    return NULL;
}

Cell* get_origin_cell(Maze* maze){
    for (int i=0; i<maze->size; i++){
        for (int j=0; j<maze->size; j++){
            if (maze->cells[i][j].value == 1){
                return &maze->cells[i][j];
            }
        }
    }
}

void init_maze_paths(Maze* maze){
    maze->num_paths = 0;
    for (int i = 0; i < maze->size; i++){
        for (int j = 0; j < maze->size; j++){
            maze->paths[maze->num_paths].caster_p = &maze->cells[i][j];
            if (maze->cells[i][j].value != 1){
                if (maze->cells[i][j].pointed_at_cell != NULL){
                    maze->paths[maze->num_paths].target_p = maze->cells[i][j].pointed_at_cell;
                }else{
                    maze->paths[maze->num_paths].target_p = NULL;
                }
                //maze->paths[maze->num_paths].valid = 1;
            }
            maze->num_paths++;
        }
    }
}

void change_origin(Maze* maze){
    Cell* origin = get_origin_cell(maze);
    for (int i=0; i<maze->size; i++){
        for (int j=0; j<maze->size; j++){
            maze->cells[i][j].visited = 0;
            if (maze->cells[i][j].value != 1){
                maze->cells[i][j].value = 0;
            }
        }
    }
    //printf("Origin {(%d,%d) ; value: %d ; is_casting: %d}\n", origin->x, origin->y, origin->value, origin->is_casting);
    int direction = 0;
    while (direction == 0){
        direction = 1+rand()%4;
        switch (direction){
            case UP: //up
                if (origin->y > 0){
                    origin->value = 0;
                    origin->pointed_at_cell = &maze->cells[origin->y-1][origin->x];
                    maze->cells[origin->y-1][origin->x].value = 1;
                    maze->cells[origin->y-1][origin->x].pointed_at_cell = NULL;
                }else{
                    direction = 0;
                }
                break;
            case RIGHT: //right
                if (origin->x < maze->size - 1){
                    origin->value = 0;
                    origin->pointed_at_cell = &maze->cells[origin->y][origin->x+1];
                    maze->cells[origin->y][origin->x+1].value = 1;
                    maze->cells[origin->y][origin->x+1].pointed_at_cell = NULL;
                }else{
                    direction = 0;
                }
                break;
            case DOWN: //down
                if (origin->y < maze->size - 1){
                    origin->value = 0;
                    origin->pointed_at_cell = &maze->cells[origin->y+1][origin->x];
                    maze->cells[origin->y+1][origin->x].value = 1;
                    maze->cells[origin->y+1][origin->x].pointed_at_cell = NULL;
                }else{
                    direction = 0;
                }
                break;
            case LEFT: //left
                if (origin->x > 0){
                    origin->value = 0;
                    origin->pointed_at_cell = &maze->cells[origin->y][origin->x-1];
                    maze->cells[origin->y][origin->x-1].value = 1;
                    maze->cells[origin->y][origin->x-1].pointed_at_cell = NULL;
                }else{
                    direction = 0;
                }
                break;
            default:
                break;
        }
    }

    //printf("got the origin and picked a direction.\n");

    int updated_origin_path_1 = 0;
    int updated_origin_path_2 = 0;

    for (int i=0; i<(maze->size*maze->size); i++){
        if (maze->paths[i].caster_p == origin){
            maze->paths[i].target_p = origin->pointed_at_cell;
            updated_origin_path_1 = 1;
        }else{
            switch (direction){
                case UP: //up
                    if (maze->paths[i].caster_p == &maze->cells[origin->y-1][origin->x]){
                        maze->paths[i].target_p = maze->cells[origin->y-1][origin->x].pointed_at_cell;
                        updated_origin_path_2 = 1;
                    }
                    break;
                case RIGHT: //right
                    if (maze->paths[i].caster_p == &maze->cells[origin->y][origin->x+1]){
                        maze->paths[i].target_p = maze->cells[origin->y][origin->x+1].pointed_at_cell;
                        updated_origin_path_2 = 1;
                    }
                    break;
                case DOWN: //down
                    if (maze->paths[i].caster_p == &maze->cells[origin->y+1][origin->x]){
                        maze->paths[i].target_p = maze->cells[origin->y+1][origin->x].pointed_at_cell;
                        updated_origin_path_2 = 1;
                    }
                    break;
                case LEFT: //left
                    if (maze->paths[i].caster_p == &maze->cells[origin->y][origin->x-1]){
                        maze->paths[i].target_p = maze->cells[origin->y][origin->x-1].pointed_at_cell;
                        updated_origin_path_2 = 1;
                    }
                    break;
                default:
                    break;
            }
        }

        if (updated_origin_path_1 == 1 && updated_origin_path_2 == 1){
            break;
        }
    }

    if (updated_origin_path_1 != 1 || updated_origin_path_2 != 1){
        printf("Could not find the specified paths. %d %d\n", updated_origin_path_1, updated_origin_path_2);
        return;
    }

    //printf("right bottom corner cell:\n\tpointed_at->x: %d\n", maze->cells[maze->size-1][maze->size-1].pointed_at_cell->x);
}

void drawLargePoint(SDL_Renderer* renderer, float x, float y, float size) {
    SDL_FRect rect = {
        .x = x - size / 2.0f,
        .y = y - size / 2.0f,
        .w = size,
        .h = size
    };
    SDL_RenderFillRect(renderer, &rect);
}


Maze* generate_maze(int size){
    Maze* maze = (Maze*)malloc(sizeof(Maze));
    if (maze == NULL){
        printf("Failed to allocate memory\n");
        return NULL;
    }
    maze->size = size;
    maze->origin_x = (SCREEN_WIDTH/2) - (maze->size*SCALE)/2;
    maze->origin_y = (SCREEN_HEIGHT/2) - (maze->size*SCALE)/2;
    maze->entrance = NULL;
    maze->exit = NULL;
    //printf("maze origin point: (%d,%d)\n", maze->origin_x, maze->origin_y);

    maze->cells = (Cell**)malloc(maze->size*sizeof(Cell*));
    if (maze->cells == NULL){
        printf("Failed to allocate memory\n");
        return NULL;
    }
    for (int i=0; i<maze->size; i++){
        maze->cells[i] = (Cell*)malloc(maze->size*sizeof(Cell));
        if (maze->cells[i] == NULL){
            printf("Failed to allocate memory\n");
            return NULL;
        }
    }

    for (int i=0; i<maze->size; i++){
        for (int j=0; j<maze->size; j++){
            maze->cells[i][j].x = j;
            maze->cells[i][j].y = i;
            maze->cells[i][j].value = 0;
            maze->cells[i][j].pointed_at_cell = NULL;
            if (j<maze->size-1){
                maze->cells[i][j].pointed_at_cell = &maze->cells[i][j+1];
            }else if (i<maze->size-1){
                maze->cells[i][j].pointed_at_cell = &maze->cells[i+1][j];
            }else if ((j<maze->size) && (i<maze->size)){
                maze->cells[i][j].value = 1;
            }
        }
        printf("\n");
    }

    maze->paths = (Path*)malloc(((size*size))*sizeof(Path));
    if (maze->paths == NULL){
        printf("Failed to allocate memory\n");
        return NULL;
    }

    
    maze->num_paths = 0;

    init_maze_paths(maze);
    return maze;
}

int main(int argc, char* argv[]){

    SDL_Window *window;
    int done = 0;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Maze", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

    if (window == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    if (renderer == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
        return 1;
    }

    Maze* maze = generate_maze(30);

    if (maze == NULL){
        printf("Failed to generate maze\n");
        return EXIT_FAILURE;
    }

    int auto_ = 0;

    while (!done){
        if (auto_ == 1){
            change_origin(maze);
        }
        SDL_Event event;

        while (SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_EVENT_QUIT:
                    done = 1;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_SPACE){
                        //for (int i=0; i<(maze->size*maze->size*10); i++){
                            auto_ = !auto_;
                        //}
                        //for (int i=0; i<maze->num_paths; i++){
                            //printf("Path %d: c1 (%d,%d) c2 (%d,%d)\n", i, maze->paths[i].c1.x, maze->paths[i].c1.y, maze->paths[i].c2.x, maze->paths[i].c2.y);
                        //}
                        //printf("New origin: {(%d, %d) ; %d}\nOld origin: {(%d, %d) ; %d}\n", maze->origin_cell.x, maze->origin_cell.y, maze->origin_cell.value, maze->previous_origin_cell.x, maze->previous_origin_cell.y, maze->previous_origin_cell.value);
                    }
                    if (event.key.key == SDLK_0){
                        for (int i=0; i<maze->size; i++){
                            for (int j=0; j<maze->size; j++){
                                maze->cells[i][j].visited = 0;
                                if (maze->cells[i][j].value != 1){
                                    maze->cells[i][j].value = 0;
                                }
                            }
                        }
                        if (maze->entrance != NULL && maze->exit != NULL){
                                maze->entrance->value = 0;
                                maze->exit->value = 0;
                        }else{
                            maze->entrance = (Cell*)malloc(sizeof(Cell));
                            maze->exit = (Cell*)malloc(sizeof(Cell));

                            if (maze->exit == NULL || maze->entrance == NULL){
                                printf("Failed to allocate memory\n");
                                return EXIT_FAILURE;
                            }
                        }

                        int random_x=0, random_y=0;

                        do{
                            random_x = rand()%maze->size;
                            random_y = rand()%maze->size;
                        }while(maze->cells[random_y][random_x].value == 1);

                        maze->entrance = &maze->cells[random_y][random_x];
                        maze->entrance->value = 2;

                        do{
                            random_x = rand()%maze->size;
                            random_y = rand()%maze->size;
                        }while(maze->cells[random_y][random_x].value == 1);

                        maze->exit = &maze->cells[random_y][random_x];
                        maze->exit->value = 3;
                    }
                    if (event.key.key == SDLK_9){
                        if (maze->entrance != NULL && maze->exit != NULL) solve(maze);
                    }
                    if (event.key.key == SDLK_ESCAPE){
                        done = 1;
                    }

                default:
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for (int i=0; i<maze->size; i++){
            for (int j=0; j<maze->size; j++){
                if (maze->cells[i][j].x == j && maze->cells[i][j].y == i){
                    switch (maze->cells[i][j].value){
                        case 1:
                            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                            break;
                        case 2:
                            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                            break;
                        case 3:
                            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                            break;
                        case 5:
                            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                            break;
                        default:
                            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                            break;
                    }
                }
                drawLargePoint(renderer, (float)maze->origin_x+(j*SCALE), (float)maze->origin_y+(i*SCALE), 4.0f);
                
            }
        }
        
        for (int i = 0; i < maze->num_paths; i++) {
            if (maze->paths[i].target_p != NULL) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderLine(
                    renderer,
                    maze->origin_x + (maze->paths[i].caster_p->x * SCALE),
                    maze->origin_y + (maze->paths[i].caster_p->y * SCALE),
                    maze->origin_x + (maze->paths[i].target_p->x * SCALE),
                    maze->origin_y + (maze->paths[i].target_p->y * SCALE)
                );
            }
        }
        // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        // SDL_RenderLine(renderer, SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT);
        SDL_RenderPresent(renderer);
    }

    for (int i=0; i<maze->size; i++){
        free(maze->cells[i]);
    }
    free(maze->cells);
    free(maze->paths);
    free(maze->entrance);
    free(maze->exit);
    free(maze);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}