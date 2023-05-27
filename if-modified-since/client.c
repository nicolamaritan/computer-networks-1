#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define MAX_H 100
#define MAX_HBUF 5000
#define MAX_RL 100

char hbuf[MAX_HBUF+1];
char rl[MAX_RL+1];
struct header
{
   char* n;
   char* v;
}h[MAX_H];

int main()
{
   int s;
   s = socket(AF_INET, SOCK_STREAM, 0);

   if (s<0)
   {
      perror("Socket creation failed.\n");
      printf("%d\n", errno);
      return 1;
   }

   struct sockaddr_in add;
   add.sin_family = AF_INET;
   add.sin_port = htons(8081);
   char* ptr = (char*)&add.sin_addr.s_addr;
   ptr[0]=127;
   ptr[1]=0;
   ptr[2]=0;
   ptr[3]=1;

   if (connect(s, (struct sockaddr*)&add, sizeof(add)) < 0)
   {
      perror("Socket connection failed.\n");
      printf("%d\n", errno);
      return 1;
   }

   char* request = "GET / HTTP/1.1\r\nIf-Modified-Since: Sat, 29 Oct 1994 19:43:31 GMT\r\n\r\n";

   write(s, request, strlen(request));

   // Request-Line parse   
   int i;
   for(i=0; i<MAX_RL && read(s, rl+i, 1); i++)
   {
      if (rl[i]=='\n' && rl[i-1] == '\r')
      {
         rl[i-1] = 0;
         break;
      }
   }

   // Headers parse
   int j;
   h[0].n = hbuf;
   for (i=0, j=0; i<MAX_HBUF && j<MAX_H && read(s, hbuf+i, 1); i++)
   {
      if (hbuf[i]=='\n' && hbuf[i-1] == '\r')
      {
         hbuf[i-1]=0;
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

   printf("Status-Line: %s\n", rl);

   for (j=0; h[j].n[0]; j++)
   {
      printf("%s: %s\n", h[j].n, h[j].v);
   }





}
