#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_H 100
#define MAX_HBUF 5000
#define MAX_RL 100
#define MAX_QS 1000
#define MAX_RES 10000
#define MAX_OUT 1000
#define MAX_MESSAGE 1000

int min(int a, int b) 
{
   return a < b ? a : b;
}

struct header
{
   char* n;
   char* v;
} h[MAX_H];

char hbuf[MAX_HBUF+1];
char rl[MAX_RL+1];
char* qs[MAX_QS+1];

int main(int argc, char* argv[])
{
   int s = socket(AF_INET, SOCK_STREAM, 0);
   if (s<0)
   {
      perror("Socket creation failed\n");
      return 1;
   }

   int opt = 1;
   if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
   {
      perror("Setting socket options failed\n");
      return 1;
   }

   struct sockaddr_in loc;
   loc.sin_family = AF_INET;
   loc.sin_port = htons(8081);

   if (bind(s, (struct sockaddr*)&loc, sizeof(loc)) < 0)
   {
      perror("Socket binding failed\n");
      return 1;
   }

   const int BACKLOG = 10;
   if (listen(s, BACKLOG) < 0)
   {
      perror("Listen failed\n");
      return 1;
   }

   while (1)
   {
      printf("Searching for connection...\n");

      struct sockaddr_in rem;
      int size = sizeof(rem);
      int c = accept(s, (struct sockaddr*)&rem, &size);
      if (c < 0)
      {
         perror("Accept failed\n");
         continue;
      }

      printf("--------------------------------------------\n\n");
      int i;
      for (i=0; i<MAX_RL && read(c, rl+i, 1) && rl[i]!='\n' && rl[i-1]!='\r'; i++);
      rl[i] = 0;

      // Header parse
      int j;
      h[0].n = hbuf;
      for (i=0,j=0; i<MAX_HBUF && j<MAX_H && read(c, hbuf+i, 1); i++)
      {
         if (hbuf[i] == '\n' && hbuf[i-1] == '\r')
         {
            hbuf[i-1] = 0;
            if (h[j].n[0] == 0)
            {
               break;
            }
            h[++j].n = hbuf+i+1;
         }
         else if (hbuf[i] == ':')
         {
            hbuf[i] = 0;
            h[j].v = hbuf+i+1;
         }
      }


      int content_length = 0;
      printf("Request-Line: %s\n", rl);
      for (j=0; h[j].n[0]; j++)
      {
         printf("%s: %s\n", h[j].n, h[j].v);
         if (strcmp(h[j].n, "Content-Length") == 0)
         {
            content_length = atoi(h[j].v);
         }
      }

      // Listen for the message body
      char message[MAX_MESSAGE+1];
      int t;
      for (i=0, t=0; i<MAX_MESSAGE && i<content_length  && (t=read(c, message+i, MAX_MESSAGE-i)); i+=t);
      printf("message-body: %s\n", message);

      // Start request line parse
      char *method, *file, *version;
      method = rl;
      int rl_length = strlen(rl);
      for (i=0; i<rl_length; i++)
      {
         if (rl[i]==' ')
         {
            rl[i] = 0;
            if (!file)
            {
               file = rl+i+1;
            }
            else
            {
               version = rl+i+1;
            }
         }
         else if (rl[i] == '?')
         {
            rl[i++] = 0;
            break;
         }
      }

      // Query string parse - Starts scanning from the end of the previous parse
      qs[0] = rl+i;
      for (j=0; i<rl_length && j<MAX_QS; i++)
      {
         // Separates query of type name=value
         if (rl[i] == '&')
         {
            rl[i] = 0;
            qs[++j] = rl+i+1;
         }
         else if (rl[i] == ' ')  // End of query string reached
         {
            rl[i] = 0;
            break;
         }
      }
      // Last enviroment variable is the null pointer
      qs[++j] = 0;

      printf("Query Strings:\n");
      for (i=0; qs[i]; i++)
      {
         printf("%s\n", qs[i]);
      }

      printf("Method: %s; File: %s\n", method, file);

      if (strcmp(method, "GET") == 0 || strcmp(method, "POST") == 0)
      {
         // Checks if the file name is cgi
         if(strncmp("cgi/", file+1, 4) == 0)
         {
            char response[MAX_RES+1];
            char out[MAX_OUT+1];
            char* program = file+5;
            printf("Executing server program: %s\n", program);

            // Pipe for child-parent communication
            int pipefd[2];
            pipe(pipefd);

            if (fork() == 0)
            {
               // Child process

               close(pipefd[0]); // Child does not read

               dup2(pipefd[1], 1); // Send stdout to the pipe
               dup2(pipefd[1], 2); // Send stderr

               close(pipefd[1]);

               char* exec_argv[3];
               exec_argv[0] = program; // First argument should be program name
               exec_argv[1] = message; 
               exec_argv[2] = 0;       // Last argument should be NULL

               // Start CGI program
               if(execve(program, exec_argv, qs) < 0)
               {
                  perror("Execve failed\n");
                  exit(1);
               }
            }
            else
            {
               // Parent process
               close(pipefd[1]); // Parent cannot write

               int* status;

               // Wait for child process to end before reading from the pipe
               wait(status);
               while (read(pipefd[0], out, MAX_OUT));

               printf("%s's output is: %s\n", program, out);

               sprintf(response, "HTTP/1.1 200 OK\r\n\r\n%s", out);
            }

            write(c, response, strlen(response));
         }
      }

      printf("\n--------------------------------------------\n");
      printf("Connection closed\n\n");
      close(c);
   }


}
