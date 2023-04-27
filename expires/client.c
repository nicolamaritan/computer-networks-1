#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_H 40
#define MAX_SL 100
#define MAX_HBUF 5000
#define MAX_ENT 1000
#define MAX_DATE 29

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

   struct sockaddr_in addr;
   addr.sin_family = AF_INET;
   addr.sin_port = htons(8080);
   char* ptr = (char*)&addr.sin_addr.s_addr;
   ptr[0] = 127;
   ptr[1] = 0;
   ptr[2] = 0;
   ptr[3] = 1;

   if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
   {
      perror("Socket connection failed\n");
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

   char* request = "GET / HTTP/1.1\r\nHost:localhost\r\n\r\n";
   write(s, request, strlen(request));

   char hbuf[MAX_HBUF+1];
   char sl[MAX_SL+1];

   // Status Line and headers parsing
   int i, j;
   for (i=0; i<MAX_SL && read(s, sl+i, 1) && sl[i]!='\n' && sl[i-1]!='\r'; i++);
   sl[i] = 0;

   h[0].n = hbuf;
   for (i=0, j=0; i<MAX_HBUF && j<MAX_H && read(s, hbuf+i, 1); i++)
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
      else if(hbuf[i] == ':' && !h[j].v)
      {
         hbuf[i] = 0;
         h[j].v = hbuf+i+1;
      }
   }

   // Printing headers and setting header variables
   int length = -1;
   struct tm exp;
   printf("%s\n", sl);
   for (j=0; h[j].n[0]; j++)
   {
      printf("%s : %s\n", h[j].n, h[j].v);
      if (strcmp(h[j].n, "Content-Length") == 0)
      {
         length = atoi(h[j].v);
      }
      else if (strcmp(h[j].n, "Expires") == 0)
      {
         strptime(h[j].v, "%a, %d %b %Y %H:%M:%S %Z", &exp);
      }
   }

   // Time GMT
   time_t now_t = time(0);
   time_t exp_t = mktime(&exp);
   double diff = difftime(exp_t, now_t);

   // Resource output
   printf("-----------------\n");

   if (diff > 0)
   {
      printf("Resource NOT expired. Reading from cache.\n");
      // Should be read from a file or something
      printf("Hello World!\n");
   }
   else
   {
      printf("Resource expired. Requesting again.\n");
      // Entity Read
      char entity[MAX_ENT+1];
      int t;
      for (t=0, i=0; i<MAX_ENT && (t=read(s, entity, MAX_ENT-i)); i+=t);
      printf("%s\n", entity);
   }


   close(s);
}
