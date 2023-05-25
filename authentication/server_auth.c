#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

#define MAX_H 100
#define MAX_RL 100
#define MAX_HBUF 5000
#define BACKLOG 50
#define MAX_RES 5000

char rl[MAX_RL+1];
char hbuf[MAX_HBUF+1];
struct header
{
   char* n;
   char* v;
} h[MAX_H];

// Implementation from ../base64/decode_base64.c

#define MAX_IN 10000
#define uc2 (unsigned char)2
#define uc4 (unsigned char)4
#define uc6 (unsigned char)6

void decode(char* in, char* out);
void quartet(unsigned char* in, unsigned char* out);
// Reverse lookup table
char table[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};


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
            h[j].v = 0;
         }
         else if (hbuf[i] == ':' && !h[j].v)
         {
            hbuf[i] = 0;
            h[j].v = hbuf+i+1;
         }
      }

      char response[MAX_RES+1 ];
      int authenticated = 0;
      int h_authorization = 0;
      char* secretUser = "user";
      char* secretPassword = "password";

      sprintf(response, "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"WallyWorld\"");

      printf("Request-Line: %s\n", rl);
      for (j=0; h[j].n[0]; j++)
      {
         printf("%s: %s\n", h[j].n, h[j].v);
        
         // Authorization header 
         if (strcmp(h[j].n, "Authorization") == 0)
         {
            h[j].v[6] = 0; // Puts terminator expecting <SP>; should parsethe string instead
            if (strcmp(h[j].v+1, "Basic") != 0)
            {
               continue;
            }
           
            // Decode base64 encoded "userid:password" 
            char out[MAX_IN+1];
            decode(h[j].v+7, out);

            // Search for : and check the words
            for(i=0; i<strlen(out); i++)
            {
               if (out[i] == ':')
               {
                  out[i] = 0;
                  if (strcmp(out, secretUser) == 0 && strcmp(out+i+1, secretPassword) == 0)
                  {
                     sprintf(response, "HTTP/1.1 200 OK\r\n\r\nAccess granted ;)");
                  }
               }
            }
         }
      }


      write(c, response, strlen(response));

      close(c);
      printf("Connection closed.\n-----------------------------------------\n\n\n");
   }


}


void decode(char* in, char* out)
{
   int len = strlen(in);

   // Ignore chars in non-quartet. Each encoded string has a multiple of 4 chars
   int tail = len % 4;
   int iter = len - tail;

   // Decode each quartet
   int i, j;
   for (i=0, j=0; i<iter; i+= 4, j+= 3)
   {
      quartet(in+i, out+j);
   }

   out[j] = 0;

}

void quartet(unsigned char* in, unsigned char* out)
{
   // Reverse translation of each char
   char it0 = table[in[0]];
   char it1 = table[in[1]];
   char it2 = table[in[2]];
   char it3 = table[in[3]];

   // First byte is always set
   out[0] = ((it0 << uc2) & 0xFC) + ((it1 >> uc4) & 0x03) ;

   // Ugly code as the compiler complained about guard clauses :/

   // If the third byte was NOT '='
   if (it2 != -2)
   {
      out[1] = ((it1 & 0x0F) << uc4) + ((it2 & 0x3C) >> uc2);

      // If the fourth byte was NOT '='
      if (it3 != -2)
      {
         out[2] = ((it2 & 0x03) << uc6) + (it3 & 0x3F);
      }
      else
      {
         out[2] = 0;
      }
   }
   else
   {
      out[1] = 0;
   }
}

