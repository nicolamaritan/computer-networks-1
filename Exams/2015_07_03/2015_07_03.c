#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

#define MAX_HBUF 5000
#define MAX_RL 100
#define MAX_H 100
#define MAX_RES 5000

char hbuf[MAX_HBUF+1];
char rl[MAX_RL+1];
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
      perror("socket failed\n");
      return 1;
   }

   int opt = 1;
   setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

   struct sockaddr_in loc;
   loc.sin_port = htons(8081);
   if (bind(s, (struct sockaddr*)&loc, sizeof(loc)) < 0)
   {
      perror("bind failed\n");
      return 1;
   }

   if (listen(s, 10) < 0)
   {
      perror("listen failed\n");
      return 1;
   }

   while (1)
   {
      struct sockaddr_in rem;
      int size = sizeof(rem);
      int s2 = accept(s, (struct sockaddr*)&rem, &size);

      if (s2 < 0)
      {
         perror("accept failed\n");
         continue;
      }

      int i;
      for (i=0; i<MAX_RL && read(s2, rl+i, 1) && rl[i] != '\n' && rl[i-1] != '\r'; i++);

      printf("\nRequest-Line:%s", rl);

      int j;
      h[0].n = hbuf;
      h[0].v = 0;
      for (i=0, j=0; i<MAX_HBUF && j<MAX_H && read(s2, hbuf+i, 1); i++)
      {
         if (hbuf[i] == '\n' && hbuf[i-1] == '\r')
         {
            hbuf[i-1] = 0;
            if (h[j].n[0] == 0)
               break;
            h[++j].n = hbuf+i+1;
         }
         else if (hbuf[i] == ':' && !h[j].v)
         {
            hbuf[i] = 0;
            h[j].v = hbuf+i+1;
         }
      } 

      char* port;
      char* ip;
      for (j=0; h[j].n[0]; j++)
      {
         printf("%s:%s\n", h[j].n, h[j].v);
         if (strcmp(h[j].n, "Host") == 0)
         {
            ip = h[j].v + 1; // jumps over the space
            for (i=0; h[j].v[i] != ':'; i++);
            port = h[j].v+i+1;
         }
      }

      char *method, *file, *ver;
      method = rl;
      for (i=0; rl[i] != ' '; i++);
      rl[i++] = 0;
      file = rl+i;
      for (i=0; rl[i] != ' '; i++);
      rl[i++] = 0;
      ver = rl+i;

      printf("Method:%s\nFile:%s\nVersion:%s\n", method, file, ver);

      if (strcmp(method, "GET") == 0 && strcmp(file, "/reflect") == 0)
      {
         // Reflect the message
         char res[MAX_RES+1];
         sprintf(res, "HTTP/1.1 200 OK\r\n\r\n");
         write(s2, res, strlen(res));

         sprintf(res, "%s %s %s", method, file, ver);
         write(s2, res, strlen(res));

         for (j=0; h[j].n[0]; j++)
         {
            sprintf(res, "%s:%s\n", h[j].n, h[j].v);
            write(s2, res, strlen(res));
         }

         char *crlf = "\r\n";
         write(s2, crlf, strlen(crlf));

         // Parse of the Host name; Not done before to print back all the headers easily
         for (i=0; ip[i] != ':'; i++);
         ip[i]=0;

         write(s2, ip, strlen(ip));
         write(s2, crlf, strlen(crlf));

         write(s2, port, strlen(port));

      }
      else
      {
         char res[MAX_RES+1];
         sprintf(res, "HTTP/1.1 501 Not Implemented\r\n\r\n");
      }

      close(s2);

   }
}
