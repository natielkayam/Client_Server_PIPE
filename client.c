#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define FIFO_NAME "a_fifo write"
#define FIFO_NAME2 "a_fifo read"
#define STR_LEN 100

char strW[STR_LEN], strR[STR_LEN]; // Global variables to store strings.
int fdr, fdw; // Global int to keep index.

// Function to catch signal and exit.
void catchSigInt(int signum)
{
	printf("Recived signal, end of communication");
	exit(0);  // If received C+ctrl exit
}

int main()//client.
{
	signal(SIGINT, catchSigInt); // Catch SIGINT(C+ctrl)
	system("clear"); // Clear ubunutu screen like in the add file
	while(1) // infinite loop.
	{
		fdw = open(FIFO_NAME, O_WRONLY); // Open writing file(Fifo_Name).
		printf("Enter for the server: "); 
		fgets(strW, STR_LEN, stdin); // Get string from user.
		fflush(stdin); // Flush to clear buffer.
		strtok(strW, "\n"); // Design the string to look better.
		write(fdw, strW, strlen(strW) + 1); // Write to file.
		close(fdw); // Close writing file.
		 
		fdr = open(FIFO_NAME2, O_RDONLY); // Open reading file(Fifo_Name2).
		read(fdr, strR, sizeof(strR)); // read message received from server.
		printf("Received from server: %s\n", strR); 
		close(fdr); // Close reading file
		if(strcmp(strR, "Done") == 0) // Check if got "Done" from server which means that the client and server both disconnect.
		{
			if (unlink(FIFO_NAME) == -1 || unlink(FIFO_NAME2) == -1) // unlink FIFO_NAME and FIFO_NAME2
				printf("cannot Close fifo"); // If failed print a message
			return EXIT_SUCCESS;
		 }
	 }
	 return EXIT_SUCCESS; 
} 