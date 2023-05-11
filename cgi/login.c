#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAX_OUT 100000

int main(int argc, char* argv[], char* env[])
{
   char* name;
   char* surname;

   int i;
   for (i=0; env[i]; i++)
   {
      if (strncmp(env[i], "name", 4) == 0)
      {
         name = env[i]+5;
      }
      else if (strncmp(env[i], "surname", 7) == 0)
      {
         surname = env[i]+8;
      }
   }

   if (!name || !surname)
   {
      perror("Error in input\n");
      return 1;
   }

   char out[MAX_OUT+1];
   sprintf(out, 
         "<!DOCTYPE html><html><body><h1>Hello %s %s :)</h1><p>How are you today?</p></body></html>",
         name,
         surname);

   printf("%s", out);

}
