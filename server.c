#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#define FIFO_NAME "a_fifo write"
#define FIFO_NAME2 "a_fifo read"
#define STR_LEN 100

pthread_t th[1]; // Array to keep the thread
char strW[STR_LEN], strR[STR_LEN]; // Global variables to keep string read and write
int fdw, fdr; // Global int variables to keep file indexes

// Void function, recevies a string and reverse it
void revstr(char *str1)  //function for reverse the string
{  
    int i, len, temp;  
    len = strlen(str1); // Keep string length 
       
    for (i = 0; i < len/2; i++)  // Iterate over the string.
    {  
        temp = str1[i];  // Keep str1[i].
        str1[i] = str1[len - i - 1];   // Replace str1[i] with str1[len-i-1].
        str1[len - i - 1] = temp;  // Replace str1[len-i-1] with temp.
    }  
} 

//Function checks if client sent exit message
int check()
{
	if(strcmp(strR, "exit") == 0) // Check if strR equals "exit".
	{
		strcpy(strW, "Done"); // If so create the server response "Done".
		fdw = open(FIFO_NAME2, O_WRONLY); // Open FIFO_NAME2 for writing
		write(fdw, strW, strlen(strW)+1); // Write strW to FIFO_NAME2
		close(fdw); // close FIFO_NAME2
		printf("Server shut down.\n");
		
		if (unlink(FIFO_NAME) == -1 || unlink(FIFO_NAME2) == -1) // unlink FIFO_NAME and FIFO_NAME2
			printf("cannot Close fifo"); // If failed print a message
		return 1;
	}
	return 0;
	
}

// Thread Function, receives void* client message
void *add_to_value(void *arg) 
{
	char* str = (char*)(arg); // Convert to string
	printf("Got: %s\n", str);
	revstr(str); // Call revstr to reverse the message
	fdw = open(FIFO_NAME2, O_WRONLY); // Open FIFO_NAME2 to write message
    write(fdw, str, strlen(str)+1); // Write message
    close(fdw); // Close FIFO_NAME2 
	printf("Response sent...\n");
	return ((void*)0);
}


int main()//server
{
	int retcode; // int to keep result from thread create
	char s[STR_LEN]; // String to keep message

	printf("server is waiting for clients.\n");
	// Create fifo
	if (mkfifo(FIFO_NAME, S_IFIFO | 0644) == -1 &&
		 errno != EEXIST) {
		 perror("cannot create fifo file");
		 exit(EXIT_FAILURE); // If failed print error and exit
	 }

	// Create fifo
	if (mkfifo(FIFO_NAME2, S_IFIFO | 0644) == -1 &&
		 errno != EEXIST) {
		 perror("cannot create fifo file") ;
		 exit(EXIT_FAILURE) ; // If failed print error and exit
	 }
	 
	 fdr = open(FIFO_NAME, O_RDONLY); // Open FIFO_NAME for reading
	 printf("Server is ready.\n");
	 if(read(fdr, strR, STR_LEN) <= 0) // Check if there is something to read
	 {
		close(fdr); // If not we should close server, first close FIFO_NAME
		if (unlink(FIFO_NAME) == -1 || unlink(FIFO_NAME2) == -1) // unlink FIFO_NAME and FIFO_NAME2
			printf("cannot Close fifo"); // If failed print a message
		return EXIT_SUCCESS;
	 }
	 else if(check() == 1) // If check is 1 it means we received exit message from client
		return EXIT_SUCCESS;
	close(fdr); // Finshed reading currently, close FIFO_NAME for reading
	retcode = pthread_create(&th[0], NULL, add_to_value, (void *)strR); // Create thread
	if (retcode != 0) // Check if creation was unscusseful
	{
		printf("Create thread failed with error %d\n", retcode);
		exit(1); // If creation was unscusseful print message and exit 
	}

	 
	 
	 while(1) // infinite loop.
	 {
		 fdr = open(FIFO_NAME, O_RDONLY); // Open FIFO_NAME for reading
		 if (read(fdr, strR, STR_LEN) <= 0) // Check if there is something to read
		 {
			 close(fdr); // If not we should close server, first close FIFO_NAME
			 if (unlink(FIFO_NAME) == -1 || unlink(FIFO_NAME2) == -1) // unlink FIFO_NAME and FIFO_NAME2
				 printf("cannot Close fifo"); // If failed print a message
			 return EXIT_SUCCESS;
		 } 
		 else if (check() == 1) // If check is 1 it means we received exit message from client
			 return EXIT_SUCCESS;

		 close(fdr); // Close FIFO_NAME for reading
		 retcode = pthread_create(&th[0], NULL, add_to_value, (void *)strR); // Create thread
		 if (retcode != 0) // Check if creation was unscusseful
		 {
			 printf("Create thread failed with error %d\n", retcode);
			 exit(1); // If creation was unscusseful print message and exit 
		 }
	 }	 
 return EXIT_SUCCESS ;
w} 