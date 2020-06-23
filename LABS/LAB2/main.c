#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "Queue.h"

char read_command() {
    	char command;
    	do {
        	command = getchar();
    	} 
	while ( command == '\n' || command == ' ' );
    		return command;
}

void help_function() {
    	printf( "command:\n" );
   	printf( "<h>\thelp\n" );
    	printf( "<e>\texit\n" );
    	printf( "<a>\tpush element\n" );
    	printf( "<b>\t pop element\n" );
	printf( "<c>\t print queue\n" );
}

int main( int argc, char* argv[] ) {
    	int i, id;
    	int app_element,  element;

    	// create a pipe
    	int my_pipe[2];
    	if ( pipe( my_pipe ) == -1 ) {
        	perror( "Error creating pipe\n" );
   	}

    	// fork
    	pid_t child_id;
    	child_id = fork();
    	if ( child_id == -1 ) {
       		perror( "Fork error\n" );
    	}
	
	if ( child_id > 0 ){ // parent process
        	if ( close( my_pipe[0] ) < 0 ) { // child doesn't read
           		perror( "Failed to close pipe descriptors" );
        	}
		
		char command;
        	printf( "Write a command(<h> for help):\n" );
        	while ( true ) {
            		command = read_command();

           		if ( command == EOF) {
                		write( my_pipe[1], &command, 1 );
                		break;
           		 }

            	switch ( command ) {
			case 'a':
				printf( "enter the element:\n" );	
				scanf("%d", &app_element);
				write( my_pipe[1], &command, 1 );
                    		write( my_pipe[1], &app_element, sizeof( int ));
                   		break;
			case 'b':
				write( my_pipe[1], &command, 1 );
				break;
			case 'c':
				write( my_pipe[1], &command, 1 );
				break;
			case 'e':
				write( my_pipe[1], &command, 1 );
                    		if ( close( my_pipe[1] ) < 0 ) {
                        		perror( "Failed to close pipe descriptors" );
                    		}
                    		return 0;
			case'h':
				 write( my_pipe[1], &command, 1 );
                   		 break;	
			default:
                    		printf( "wrong command\n" );
                    		break;
			}	
		}
				
	}
	else{ // child process
		Queue *q = CreateQueue();		
		if ( close( my_pipe[1] ) < 0 ) { // parent doesn't write
            		perror( "Failed to close pipe descriptors" );
        	}
		while ( true ) {
           		char reading_buf;
			do {
                		read( my_pipe[0], &reading_buf, 1 );
            		} while ( reading_buf == '\n' || reading_buf == ' ' );
            		if ( reading_buf == EOF) {
                		break;
            		}
			switch ( reading_buf ) {
				case 'a':
                    			read( my_pipe[0], &element, sizeof( int ));
                   			QueuePush(q, element);
                    			break;
				case 'b':
					QueuePop(q);
					break;
				case 'c':
					PrintQueue(q);
					printf("\n");
					break;
				case 'e':
					DestroyQueue(q);
					if ( close( my_pipe[0] ) < 0 ) {
                        			perror( "Failed to close pipe descriptors" );
                    			}
                   			return 0;
				case 'h':
                   			help_function();
                    			break;
                		default:
                    			printf( "wrong command\n" );
                    			break;

            		}
		}
		if ( close( my_pipe[0] ) < 0 ) {
            		perror( "Failed to close pipe descriptors" );
        	}
	}
	return 0;
}



































