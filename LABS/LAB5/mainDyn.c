#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <string.h>

#include "Vector.h"


int main(){
	void (*CreateVector) (Vector* v);
	int (*CountVector) (Vector* v);
	void (*AddVector) (Vector* v, char* e);
	char* (* GetVector) (Vector* v, int index );
	void (*DeleteVector) (Vector* v, int index);
	void (*PrintVector) (Vector* v);
	void (*FreeVector) (Vector* v);
	char *err;

	void *libHandle;
    	libHandle = dlopen("libvec.so", RTLD_LAZY);
    	if (!libHandle) {
        	fprintf(stderr, "%s\n", dlerror());
        	exit(-1);
	}

	CreateVector = dlsym(libHandle, "CreateVector");
	CountVector = dlsym(libHandle, "CountVector");
	AddVector = dlsym(libHandle, "AddVector");
	GetVector = dlsym(libHandle, "GetVector");
	DeleteVector = dlsym(libHandle, "DeleteVector");
	PrintVector = dlsym(libHandle, "PrintVector");
	FreeVector = dlsym(libHandle, "FreeVector"); 

	if(err = dlerror()) {
        	fprintf(stderr, "%s\n", err);
       		exit(-1);
    	}
	
	int act = 0;
	Vector vec;
	(*CreateVector)(&vec);
	int id, sz, size,i;
    	printf("This is runtime linking\n\n");
    	printf("Choose an operation:\n");
   	printf("1) Add element\n");
   	printf("2) Delete element by index\n");
    	printf("3) Count of elems\n");
   	printf("4) Print vector\n");
    	printf("0) Exit\n");
    	while (scanf("%d", &act) && act) {
        	switch(act) {
		char* app_element;
		char masstr[255] = "";
		char* element;
			case 1:
				printf("Enter the elememt\n");
				app_element = ( char* ) malloc( sizeof( char ) * 255 );
				scanf("%s", masstr);
				strcpy(app_element, masstr);
				(*AddVector)(&vec, app_element);
				app_element = NULL;
                    		break;
			case 2:
				printf("Enter the index:\n");
				scanf( "%d", &id );	
				(*DeleteVector)(&vec, id);
				break;
			case 3:
				printf("Count:");
				sz = (*CountVector)(&vec);
				printf("%d\n",sz);
				break;
			case 4:
				printf( "vector:\n" );
                    		(*PrintVector)(&vec);
                    		break;
			default:
            			printf("Incorrect command\n");
            			break;
		}
	}
	printf("The programm has been finished\n");
	(*FreeVector)(&vec);
	dlclose(libHandle);
	return 0;
}	 
				
				

				
