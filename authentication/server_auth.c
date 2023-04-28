#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BACKLOG 10
#define MAX_H 100
#define MAX_HBUF 5000
#define MAX_RL 100
#define MAX_RES 1000

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
      printf("Socket creation failed\n");
      return 1;
   }

   int opt = 1;
   if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
   {
      printf("Socket option error\n");
      return 1;
   }

   struct sockaddr_in local;
   local.sin_port = htons(8080);
   local.sin_family = AF_INET;

   if (bind(s, (struct sockaddr*) &local, sizeof(local)) < 0)
   {
      printf("Binding failed\n");
      return 1;
   }

   if (listen(s, BACKLOG) < 0)
   {
      printf("Listen failed\n");
      return 1;
   }

   while (1)
   {
      struct sockaddr_in remote;
      int size = sizeof(remote);
      int c = accept(s, (struct sockaddr*) &remote, &size);

      if (c < 0)
      {
         printf("*** Connection error.\n");
         continue;
      }
      printf("*** Connection successful.\n");

      int i;
      char rl[MAX_RL+1];
      for (i=0; i<MAX_RL && read(c, rl+i, 1) && rl[i] != '\n' && rl[i-1] != '\r'; i++);

      char hbuf[MAX_HBUF+1];
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
         else if (hbuf[i] == ':' && !h[j].v )
         {
            hbuf[i] = 0;
            h[j].v = hbuf+i+1;
         }
      }

      int authorized = 0;
      char* user = "user";
      char* psw = "qwerty";

      // Printing
      printf("%s\n", rl);
      for (i=0; h[i].n[0]; i++)
      {
         printf("%s : %s\n", h[i].n, h[i].v);
      }

      char response[MAX_RES];

      if (!authorized)
      {
         strcpy(response, "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"WallyWorld\"\r\n\r\n");
      }
      else
      {
         strcpy(response, "HTTP/1.1 200 OK\r\n\r\nAccess granted.");
      }

      write(c, response, strlen(response));
      printf("*** Closing connection.\n\n");
      close(c);
   }
}
