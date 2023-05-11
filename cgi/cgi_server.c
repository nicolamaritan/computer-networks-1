#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define MAX_H 100
#define MAX_HBUF 5000
#define MAX_RL 100

struct header
{
   char* n;
   char* v;
} h[MAX_H];

char hbuf[MAX_HBUF+1];
char rl[MAX_RL+1];

int main()
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

      printf("Request-Line: %s\n", rl);
      for (j=0; h[j].n[0]; j++)
      {
         printf("%s: %s\n", h[j].n, h[j].v);
      }


      printf("\n--------------------------------------------\n");
      printf("Connection closed\n\n");
      close(c);
   }


}
