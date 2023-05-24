#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_H 100
#define MAX_RL 100
#define MAX_HBUF 5000
#define BACKLOG 50

char rl[MAX_RL+1];
char hbuf[MAX_HBUF+1];
struct header
{
   char* n;
   char* v;
} h[MAX_H];

int main()
{
   int s = socket(AF_INET, SOCK_STREAM, 0);
   if (s < 0)
   {
      perror("Socket creation failed.\n");
      printf("%d\n", errno);
      return 1;
   }

   int opt = 1;
   if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
   {
      perror("Setting socket option failed.\n");
      printf("%d\n", errno);
      return 1;
   }

   printf("Socket created.\n");

   struct sockaddr_in loc;
   loc.sin_family = AF_INET;
   loc.sin_port = htons(8081);

   if (bind(s, (struct sockaddr*)&loc, sizeof(loc)) < 0)
   {
      perror("Binding failed.\n");
      printf("%d\n", errno);
      return 1;
   }

   printf("Socket bounded.\n");

   if (listen(s, BACKLOG) < 0)
   {
      perror("Listen failed.\n");
      printf("%d\n", errno);
      return 1;
   }

   while (1)
   {
      struct sockaddr_in rem;
      int rem_size = sizeof(rem);
      int c = accept(s, (struct sockaddr*)&rem, &rem_size);

      if (c < 0)
      {
         printf("Connection failed.\n");
         printf("%d\n", errno);
         continue;
      }

      printf("Socket connected.\n");

      // RL parse
      int i;
      for (i=0; i<MAX_RL && read(c, rl+i, 1); i++)
      {
         if (rl[i] == '\n' && rl[i-1] == '\r')
         {
            rl[i-1] = 0;
            break;
         }
      }

      // Headers parse
      int j;
      h[0].n = hbuf;
      for (i=0, j=0; i<MAX_HBUF && j<MAX_H && read(c, hbuf+i, 1); i++)
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
         else if (hbuf[i] == ':' && !h[j].v)
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

      close(c);
      printf("Connection closed.\n-----------------------------------------\n\n\n");
   }


}
