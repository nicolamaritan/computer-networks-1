#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#define MAX 100000
#define MAX_IN 10000
#define TERN_BITS 24
#define SEXTET_BITS 6
#define TERN_BYTES 3
#define TERN_SEXTETS 4

char table[] = {
   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
   'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
   'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

int sextet(uint32_t ptr, int from);
void base64(void* in, int size, char* out);
int bit(uint32_t val, int pos);
int power(int a, int b);

int main()
{
   //char* in = "light work";
   char in[MAX_IN+1];
   char out[MAX+1];
   fgets(in, MAX_IN+1, stdin);
   in[strcspn(in, "\n")] = '\0';  // remove newline character by default
   base64((void*)in, strlen(in), out);
   printf("%s\n", out);
}

void base64(void* in, int size, char* out)
{
   int trailing = size%3;
   unsigned char* cin = (unsigned char*) in;
   int out_bytes = 0;
   int i = 0;
   
   for (i=0; i<size-trailing; i+=3)
   {
      uint32_t* ptr = (uint32_t*)(cin+i);

      /*
       * When dereferencing ptr the bit order inside the uint32_t
       * is set according to the architecture endiannes. Since
       * sextet assumes that the bytes in the bytes parameter are
       * in the same order as in the cin array (big endian), htonl is invoked.
       */
      uint32_t be_bytes = htonl(*ptr);

      out[out_bytes++] = table[sextet(be_bytes, 26)];
      out[out_bytes++] = table[sextet(be_bytes, 20)];
      out[out_bytes++] = table[sextet(be_bytes, 14)];
      out[out_bytes++] = table[sextet(be_bytes, 8)];
   }

   if (trailing > 0)
   {
      uint32_t* last = (uint32_t*)(cin+i);   // Last one or 2 bytes
      uint32_t be_last = htonl(*last);
      
      out[out_bytes++] = table[sextet(be_last, 26)];
      if (trailing == 2)
      {
         out[out_bytes++] = table[sextet(be_last, 20)];
         cin[i+2] = 0;
         out[out_bytes++] = table[sextet(be_last, 14)];
         out[out_bytes++] = '=';
      }
      else
      {
         cin[i+1] = 0;
         out[out_bytes++] = table[sextet(be_last, 20)];
         out[out_bytes++] = '=';
         out[out_bytes++] = '=';
      }
      
   }
}

int sextet(uint32_t bytes, int from)
{
   int res = 0;
   for (int i=0; i<SEXTET_BITS; i++)
   {
      res += (power(2, i) * bit(bytes, from+i));
   }
   return res;
}

int bit(uint32_t val, int pos)
{
   return(val >> pos) & 1;
}

int power(int a, int b) 
{
   int result = 1;
   for (int i = 0; i < b; i++) {
      result *= a;
   }
   return result;
}
