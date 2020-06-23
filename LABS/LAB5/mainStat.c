#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "Vector.h"

int main(){
	int act = 0;
	Vector vec;
	CreateVector(&vec);
	int id, sz, size,i;
    	printf("This is compile-time  linking\n\n");
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
				AddVector(&vec, app_element);
				app_element = NULL;
                    		break;
			case 2:
				printf("Enter the index:\n");
				scanf( "%d", &id );	
				DeleteVector(&vec, id);
				break;
			case 3:
				printf("Count:");
				sz = CountVector(&vec);
				printf("%d\n",sz);
				break;
			case 4:
				printf( "vector:\n" );
                    		PrintVector(&vec);
                    		break;
			default:
            			printf("Incorrect command\n");
            			break;
		}
	}
	printf("The programm has been finished\n");
	FreeVector(&vec);
	return 0;
}
