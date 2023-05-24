#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_IN 1000
#define MAX_OUT ((MAX_IN/3)*4)+4

// unsigned chars used for shifting
#define uc2 (unsigned char)2
#define uc4 (unsigned char)4
#define uc6 (unsigned char)6

void triplet(unsigned char* in, unsigned char* out);
char encode(unsigned char c);

char* table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* 
   Tested with: $ echo <in> | base64
   Both fgets and echo without -n flag adds a \n at
   the end of the input. 
 */

int main()
{
   unsigned char in[MAX_IN];
   unsigned char out[MAX_OUT];

   fgets(in, sizeof(in), stdin);

   int tri = strlen(in) / 3;
   int tail = strlen(in) % 3;

   int i, j;
   for (i=0, j=0; i < (tri*3); i+= 3, j+=4)
   {
      triplet(in+i, out+j);
   }

   if (tail == 1)
   {
      out[j++] = table[((in[i] & 0xFC) >> uc2)];
      out[j++] = table[(in[i] & 0x03) << uc4];
      out[j++] = '=';
      out[j++] = '=';
   } 
   else if (tail == 2)
   {
      out[j++] = table[((in[i] & 0xFC) >> uc2)];
      out[j++] = table[((in[i] & 0x03) << uc4) + ((in[i+1] & 0xF0) >> uc4)];
      out[j++] = table[((in[i+1] & 0x0F) << uc2)];
      out[j++] = '=';
   }

   out[j] = 0;
   printf("%s", out);

}

void triplet(unsigned char* in, unsigned char* out)
{
   out[0] = table[((in[0] & 0xFC) >> uc2)];
   out[1] = table[((in[0] & 0x03) << uc4) + ((in[1] & 0xF0) >> uc4)];
   out[2] = table[((in[1] & 0x0F) << uc2) + ((in[2] & 0xC0) >> uc6)];
   out[3] = table[in[2] & 0x3F];
}

