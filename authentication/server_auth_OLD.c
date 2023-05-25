#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define BACKLOG 10
#define MAX_H 100
#define MAX_HBUF 5000
#define MAX_RL 100
#define MAX_RES 1000
#define MAX_DECODE 1000

struct header
{
   char* n;
   char* v;
} h[MAX_H];

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};
void build_decoding_table(); 
void base64_cleanup();
unsigned char *base64_decode(const char *data,
                             size_t input_length,
                             size_t *output_length);


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
            h[j].v = 0; // Override previous connection value
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
         
         if (strcmp(h[i].n, "Authorization") == 0)
         {
            printf("OOOOOOOOOOOOOOOOOOOOOOOOOO");
            char* encoded = h[i].v + 7;   // skip Basic
            size_t length = 0;
            char* out = base64_decode(encoded, strlen(encoded), &length);
            out[length] = 0;

            char* out_user = out;
            for (j=0; out[j]!=':'; j++);  // Parse decoded text
            out[j] = 0;
            char* out_psw = out+j+1;

            // Checks for correct user and password
            if (strcmp(user, out_user) == 0 && strcmp(psw, out_psw) == 0)
            {
               authorized = 1;
            }
         }
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

// Code from https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
void build_decoding_table() {

    decoding_table = malloc(256);

    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}


void base64_cleanup() {
    free(decoding_table);
}

unsigned char *base64_decode(const char *data,
                             size_t input_length,
                             size_t *output_length) {

    if (decoding_table == NULL) build_decoding_table();

    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
}
