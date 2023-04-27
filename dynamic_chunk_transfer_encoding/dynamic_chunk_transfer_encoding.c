/*
 Receives from a client an undefined number of
 chunks when Transfer-Encoding is chunked.
 A linked list of chunk element is used.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_SL 1000
#define MAX_HEADERS 20
#define MAX_HBUF 5000
#define MAX_ENTITY 1000000
#define MAX_LEN 10
#define INVALID -1
#define CHUNKED -2
int main()
{
   int s;
   struct sockaddr_in addr;
   int i, j, t;

   s = socket(AF_INET, SOCK_STREAM, 0);
   if (s == -1)
   {
      perror("Socket creation failed\n");
      printf("%d\n", errno);
      return 1;
   }

   char* ptr = (char*)&addr.sin_addr.s_addr;
   ptr[0] = 142;
   ptr[1] = 251;
   ptr[2] = 209;
   ptr[3] = 46;
   addr.sin_family = AF_INET;
   addr.sin_port = htons(80);

   if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == -1)
   {
      perror("Connection failed\n");
      printf("%d\n", errno);
      return 1;
   }

   char* request = "GET / HTTP/1.1\r\nHost:www.google.com\r\n\r\n";
   write(s, request, strlen(request));

   // Status line
   char sl[MAX_SL+1];
   for (i = 0; i<MAX_SL && read(s, sl+i, 1) && sl[i] != '\n' && sl[i-1] != '\r'; i++);
   printf("%s\n", sl);

   // Header parsing
   struct header
   {
      char* n;
      char* v;
   };

   struct header h[MAX_HEADERS];
   char hbuf[MAX_HBUF+1];
   h[0].n = hbuf;
   for (i=0, j=0; i<MAX_HBUF && j<MAX_HEADERS && read(s, hbuf+i, 1); i++)
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
   hbuf[i] = 0;


   int length = INVALID;
   // Printing headers
   for (i=0; h[i].n[0]; i++)
   {
      printf("%s : %s\n", h[i].n, h[i].v);
      if (strcmp(h[i].n, "Content-Length") == 0)
      {
         length = atoi(h[i].v);
      }
      else if (strcmp(h[i].n, "Transfer-Encoding") == 0 && strcmp(h[i].v, "chunked"))
      {
         length = CHUNKED;
      }
   }


   // Producing entity
   if (length == INVALID)
   {
      perror("Invalid response.\n");
      return 1;
   }   

   // Chunk as linked list element
   struct chunk
   {
      int length;
      char* content;
      struct chunk* prev;
      struct chunk* next;
   };

   if (length > 0)
   {
      char ent[MAX_ENTITY +1];
      for (i=0; i<MAX_ENTITY && i<length && (t = read(s, ent+i, MAX_ENTITY-i)); i+=t);
      ent[i] = 0;
      printf("%s\n", ent);
   }
   else if (length = CHUNKED)
   {
      // Memorize previous node in the chunk
      struct chunk* prev = malloc(sizeof(struct chunk));
      struct chunk* head = prev;
      head->prev = NULL;
      head->length = 0;
      head->content = "";

      while (1)
      {
         struct chunk* curr = malloc(sizeof(struct chunk));

         // Links nodes
         curr->prev = prev;
         prev->next = curr;

         char s_arr[MAX_LEN+1];

         // Read curr chunk length
         for (i=0; i<MAX_LEN && read(s, s_arr+i, 1) && s_arr[i] != '\n' && s_arr[i-1] != '\r'; i++);
         s_arr[i] = 0;

         curr->length = strtol(s_arr, NULL, 16);
         if (curr->length == 0)
         {
            // The chunk before the last one has no next
            prev->next = NULL;
            break;
         }

         // Allocate memory for chunk and read directly into it
         curr->content = malloc(sizeof(char)*(curr->length));
         for (i=0; i < curr->length && read(s, curr->content+i,1); i++);
         curr->content[i] = 0;

         // Save for next iteration
         prev = curr;

         // Flush pending CRLF
         char pending[3];
         read(s, pending, 2);
         pending[3] = 0;

         assert(strcmp(pending, "\r\n") == 0);
      }

      // Printing all content
      struct chunk* curr = head;
      while (curr->next)
      {
         curr = curr->next;
         printf("%s", curr->content);
      }


   }
   else
   {
      perror("Error while processing response\n");
      return 1;
   }

   close(s);


}
