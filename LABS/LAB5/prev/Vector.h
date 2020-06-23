#ifndef VECTOR_H__
#define VECTOR_H__



typedef struct vector_ {
    char** data;
    int size;
    int count;
} Vector;


extern void CreateVector (Vector* v);
extern int CountVector (Vector* v);
extern void AddVector (Vector* v, char* e);
extern void SetVector (Vector* v, int, char* e);
extern char* GetVector (Vector* v, int);
extern void DeleteVector (Vector* v, int);
extern void FreeVector (Vector* v);
extern void PrintVector (Vector* v);


#endif
