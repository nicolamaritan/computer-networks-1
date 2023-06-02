#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <stdlib.h>

#define MAX_RL 100
#define MAX_HBUF 5000
#define MAX_H 100
#define BACKLOG 10
#define MAX_ENT 1000

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
      perror("Socket creation failed\n");
      printf("%d\n", errno);
      return 1;
   }

   int opt = 1;
   if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
   {
      perror("Socket options failed\n");
      printf("%d\n", errno);
      return 1;
   }

   struct sockaddr_in loc;
   loc.sin_family = AF_INET;
   loc.sin_port = htons(8081);
   if (bind(s, (struct sockaddr*)&loc, sizeof(loc)) < 0)
   {
      perror("Binding failed\n");
      printf("%d\n", errno);
      return 1;
   }

   if (listen(s, BACKLOG) < 0)
   {
      perror("Listen failed\n");
      printf("%d\n", errno);
      return 1;
   }

   while (1)
   {
      struct sockaddr_in rem;
      int size = sizeof(rem);
      int c = accept(s, (struct sockaddr*)&rem, &size);

      if (c < 0)
      {
         perror("Accept failed\n");
         printf("%d\n", errno);
         continue;
      }

      int i;
      for (i=0; i<MAX_RL && read(c, rl+i, 1) && rl[i] != '\n' && rl[i-1] != '\r'; i++);
      rl[i]=0;

      printf("Request-Line: %s\n", rl);

      int j;
      h[0].n = hbuf;
      h[0].v = 0;
      for (i=0, j=0; i<MAX_HBUF+1 && j<MAX_H && read(c, hbuf+i, 1); i++)
      {
         if (hbuf[i]=='\n' && hbuf[i-1]=='\r')
         {
            hbuf[i-1]=0;
            if (h[j].n[0] == 0)
            {
               break;
            }
            h[++j].n = hbuf+i+1;
            h[j].v = 0;
         }
         else
         {
            if (hbuf[i]==':' && !h[j].v)
            {
               hbuf[i] = 0;
               h[j].v = hbuf+i+1;
            }
         }
      }
      hbuf[i] = 0;

      int cl = -1;
      for (j=0; h[j].n[0]; j++)
      {
         printf("%s:%s\n", h[j].n, h[j].v);
         if (strcmp("Content-Length", h[j].n) == 0)
         {
            cl = atoi(h[j].v+1); //skip space
         }
      }

      if (cl > 0)
      {
         char ent[MAX_ENT+1];
         for (i=0; i<MAX_ENT && i<cl && read(c, ent+i, 1); i++);
         printf("%s\n", ent);
      }


      char* res = "HTTP/1.1 200 OK\r\n\r\nHello World!";
      write(c, res, strlen(res));

      close(c);

   }



}
