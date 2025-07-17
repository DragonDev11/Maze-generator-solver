#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>

#define SIZE 10

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

typedef struct Maze{
    int origin_x, origin_y;
    int size;
    int** cells;
}Maze;

int main(int argc, char* argv[]){
    Maze* maze = (Maze*)malloc(sizeof(Maze));
    if (maze == NULL){
        printf("Failed to allocate memory\n");
        return EXIT_FAILURE;
    }
    printf("Maze size: ");
    int size;
    scanf("%d", size);
    maze->origin_x = 0;
    maze->origin_y = 0;
    maze->size = size;
    int** matrix = (int**)malloc(size*sizeof(int*));
    if (matrix == NULL){
        printf("Failed to allocate memory\n");
        return EXIT_FAILURE;
    }
    for (int i=0; i<size; i++){
        matrix[i] = (int*)malloc(size*sizeof(int));
        if (matrix[i] == NULL){
            printf("Failed to allocate memory\n");
            return EXIT_FAILURE;
        }
        for (int j=0; j<size; j++){
            matrix[i][j] = 1;
            printf("%d", matrix[i][j]);
        }
        printf("\n");
    }
    for (int i=0; i<size; i++){
        free(matrix[i]);
    }
    free(matrix);
    free(maze);

    return 0;
}