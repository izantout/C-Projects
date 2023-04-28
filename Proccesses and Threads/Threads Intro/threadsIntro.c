#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

static void *child(void *ignored){
   // While loop to keep outputting the message until enter is pressed
   while(1){
      // Sleep for 3 seconds 
      sleep(3);
      // Output the message
      printf("Child is done sleeping 3 seconds.\n");
   }
}

int main(int argc, char *argv[]){
   // ----- Variable Decleration -----
   int code;
   char input;
   pthread_t child_thread;
   int keyPressed;
   // --------------------------------

   // Create the child thread
   code = pthread_create(&child_thread, NULL, child, NULL);

   // Print the instruction that the enter key would terminate the thread
   printf("Please press the 'Enter' key to cancel the child thread\n");
   
   // Check for keyboard input
   keyPressed = scanf("%c", &input);

   // If its pressed
   if(keyPressed){
      // Print that the enter key is pressed
      printf("The 'Enter' key is pressed\n");
   }
   // If the variable code returns 1, this means that there was an error creating the thread
   if(code){
      // print the error message with the error code
      fprintf(stderr, "pthread_create failed with code %d\n", code);
   }
   
   // Cancel the child thread
   code = pthread_cancel(child_thread);
   
   // If the variable code returns 1, this means that there was an error creating the thread
   if(code){
      // print the error message with the error code
      fprintf(stderr, "pthread_cancel failed with code %d\n", code);
   }
   
   // Print that the child is canceled
   printf("Child is canceled\n");

   // Sleep for 5 seconds
   sleep(5);

   // Print the end message
   printf("Parent is done sleeping 5 seconds.\n");

   // End program
   return 0;
}
