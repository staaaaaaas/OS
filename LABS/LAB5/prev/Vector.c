#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Vector.h"

void CreateVector (Vector *v){
    v->data = NULL;
    v->size = 0;
    v->count = 0;
}

int CountVector (Vector *v){
    return v->count;
}

void AddVector (Vector *v, char *e)
{
    if (v->size == 0) {
        v->size = 10;
        v->data = malloc(sizeof(char*) * v->size);
        memset(v->data, '\0', sizeof(char) * v->size);
    }

    // condition to increase v->data:
    // last slot exhausted
    if (v->size == v->count) {
        v->size *= 2;
        v->data = realloc(v->data, sizeof(char*) * v->size);
    }

    v->data[v->count] = e;
    v->count++;
}

/*void SetVector (Vector *v, int index, char *e)
{
    if (index >= v->count) {
        return;
    }

    v->data[index] = e;
}*/

char*  GetVector (Vector *v, int index){
    if (index >= v->count) {
        return 0;
    }
	char* app_element = ( char* ) malloc( sizeof( char ) * 255 );
	app_element = v->data[index];	
    return app_element;
}

void DeleteVector (Vector *v, int index){
    if (index >= v->count) {
        return;
    }

    v->data[index] = 0;

    int i, j;
    char **newarr = (char**)malloc(sizeof(char*) * v->count * 2);
    for (i = 0, j = 0; i < v->count; i++) {
        if (v->data[i] != 0) {
            newarr[j] = v->data[i];
            j++;
        }
    }

    free(v->data);

    v->data = newarr;
    v->count--;
}

void FreeVector(Vector *v){
    for ( int i = 0; i < v->count; ++i ) {
        v->data[i] = 0;
    }
    free(v->data);
}

	
