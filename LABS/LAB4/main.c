#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/mman.h>

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
    	int i, id, size;
    	int element;
	int app_element;

    	char *area;
    	if ((area = mmap (0, sizeof(char) * 3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0))
        == MAP_FAILED) {
        	printf ("mmap error for input");
        	return 0;
    	}
	
	int sig;
    	pid_t parent_id;
    	sigset_t set;
    	sigemptyset(&set);
    	sigaddset(&set, SIGUSR1); //добавляем USR1 в набор
    	sigaddset(&set, SIGUSR2); //добавляем USR2 в набор
    	sigprocmask(SIG_BLOCK, &set, NULL);
   	parent_id = getpid(); //запомним id процесса - предка


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
			if ( command == EOF ) {
                		command = 'c';
                		write( my_pipe[1], &command, 1 );
                		kill(child_id, SIGUSR2);
                		break;
            		}
			area[0] = command;
			switch ( command ) {
                		case 'a':
					printf( "enter the element:\n" );	
					scanf("%d", &app_element);
					area[1] = app_element;
					break;
				case 'b':
					break;
				case 'c':
					break;
				case 'e':
                    			if ( close( my_pipe[1] ) < 0 ) {
                        			perror( "Failed to close pipe descriptors" );
                    			}
					kill(child_id, SIGUSR2);
                    			return 0;
				case'h':
                   		 	break;	
				default:
                    			printf( "wrong command\n" );
                    			break;
			}
		kill(child_id, SIGUSR2);
            	sigwait(&set, &sig);	
		}
				
	}
	else{ // child process
		Queue *q = CreateQueue();		
		if ( close( my_pipe[1] ) < 0 ) { // parent doesn't write
            		perror( "Failed to close pipe descriptors" );
        	}
		while ( true ) {
			sigwait(&set, &sig);
			if ( area[0] == EOF ) {
                		break;
            		}
			switch ( area[0] ) {
				case 'a':
                    			element = area[1];
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
		kill(parent_id, SIGUSR1);
		}
		if ( close( my_pipe[0] ) < 0 ) {
           		 perror( "Failed to close pipe descriptors" );
        	}
	}
	return 0;
}
