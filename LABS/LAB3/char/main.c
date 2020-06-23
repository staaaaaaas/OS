
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>


int ThreadsCount;

int Run;

int n;

typedef struct{
	int left;
	int mid;
	int right;
	int pidThread;
} Range;

//Initial array
char *arr; 

int min(int a, int b){
	return a<=b?a:b;
}

//Insertoin Sort sorts from left to right (<= Run)
void *InsertionSort (void *arg){
	Range *tmp = (Range*) arg;
	int left = tmp->left;
	int right = tmp->right;
	for (int i = left; i < right; i++){
		char* x = ( char* ) malloc( sizeof( char ) * 255); 
		x = strcpy(x, arr[i]);
		int j;
		for (j = i - 1; j >= left && arr[j] > x; j--){
			arr[j+1] = arr[j];
		}
		arr[j+1] = x;
	}
}


void *Merge (void *arg){
	// Divide array on 2 parts
	Range *tmp = (Range*) arg;
	int len1 = tmp->mid - tmp->left;
	int len2 = tmp->right - tmp->mid;
	// Create 2 arrays
	char *left = (char*)malloc(sizeof(char)*len1);
	char *right = (char*)malloc(sizeof(char)*len2);
	// Fill arrays
	for (int i = 0; i < len1; i++){
		left[i] = arr[tmp->left + i];
	}
	for (int i = 0; i < len2; i++){
		right[i] = arr[tmp->mid +i];
	}
	int i = 0, j = 0;
	int k = tmp->left;
	// Compare and merge this 2 arrays
	while (i < len1 && j < len2){
		if (left[i] <= right[j]){
			arr[k] = left[i];
			i++;
		}
		else{
			arr[k] = right[j];
			j++;
		}
		k++;
	}
	// Copy remaining elems if any stay
	while (i < len1){
		arr[k] = left[i];
		i++;
		k++;
	}
	while (j < len2){
		arr[k] = right[j];
		j++;
		k++;
	}
	free(left);
	free(right);
}	
	

int GetRun (int n){
	int r = 0;
   	while (n >= 64) {
        	n >>= 1;
        	r |= n & 1;
    	}
    	return n + r;
}


void TimSort(){
	Run = GetRun(n);
	// Sort individual subarrays of size Run
	pthread_t *thread = (pthread_t *)malloc(ThreadsCount*sizeof(pthread_t *));
	Range *k = (Range *)malloc(sizeof(Range)*ThreadsCount);
	int count = 0;
	int shift = 0;
	for (int i = 0; i < n; i += Run){
		if (count == ThreadsCount){
			count = 0;
			shift++;
		}
		if (shift > 0){
			pthread_join(thread[count], NULL);
		}
		k[count].pidThread = count;
		k[count].left = i;
		k[count].right = min(i + Run, n);
		pthread_create(&thread[count], NULL, InsertionSort, &k[count]);
		count++;
	}
	// Wait all treads
	if (shift > 0){
		count = ThreadsCount;
	}
	for (int i = 0; i < count; i++) {
		pthread_join(thread[i], NULL);
	}
	//Merging subarrays
	for (int size = Run; size < n; size = 2*size){
		shift = 0;
		count = 0;
		for (int left = 0; left < n; left += 2*size){
			if (count == ThreadsCount){
				count = 0;
				shift++;
			}
			if (shift > 0) {
				pthread_join(thread[count], NULL);
			}
			k[count].pidThread = count;
			k[count].right = min((left + 2*size), n);
			k[count].left = left;
			k[count].mid = left+size;
			pthread_create(&thread[count], NULL, Merge, &k[count]);
			count++;
		}
		if (shift > 0){
                	count = ThreadsCount;
       	 	}
		for (int i = 0; i < count; i++) {
			pthread_join(thread[i], NULL);
		}
	}
	free(k);
	free(thread);
}


void PrintArray(char arr[], int n){
    	for (int i = 0; i < n; i++){
        	printf("%s\t", arr[i]);
	}
	printf("\n");
}


int main (int argc, char **argv){
	if (argc != 2) {
		printf("Usage: ThreadsNumber\n");
		exit(EXIT_FAILURE);
	}
	ThreadsCount = atoi(argv[1]);
	if (ThreadsCount <= 0) {
		printf("ERROR: usage ThreadNumber > 0\n");
		exit(EXIT_FAILURE);
	}
	printf("Count of Threads:  ");
	printf("%d\n", ThreadsCount);
	printf("Enter the numder of elems\n");
	scanf("%d", &n);
	arr = (char *)malloc(sizeof(char)*n);
	printf("Enter the elems\n");
	for (int i = 0; i < n; i++) {
		scanf("%s", arr[i]);
	}
	TimSort();
	PrintArray(arr, n);
	/*for (int i = 0; i < n-1; i++) {
		if (arr[i] > arr[i+1]) std::cout <<"WRONG ANSWER " << arr[i] << "\t" << arr[i+1] << std::endl;
	}*/
	free(arr);
    	return 0;
}
	

	

