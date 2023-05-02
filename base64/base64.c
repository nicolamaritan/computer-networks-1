#include <stdio.h>
#include <string.h>
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

void base64(void* in, int size, char* out);
int bit(char val, int pos);

// Calculate the decimal of val byte given the power of the
// most left bit and the bits to scan from the "from" bit of the bit.
int calculate(char val, int max_pow, int bits, int from);
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
   // Calculate the trailing octets
   int trailing = size%3;

   unsigned char* cin = (unsigned char*) in;

   int out_bytes = 0;
   int i = 0;
   // For each tern
   for (i=0; i<size-trailing; i+=3)
   {
      // Init tern bytes
      char tb[TERN_BYTES]; // tern_byte
      for (int j = 0; j<TERN_BYTES; j++)
      {
         tb[j] = cin[i+j];
      }
      
      // First sextet
      out[out_bytes] = table[calculate(tb[0], 5, 6, 2)];  // takes the first byte and calculate the value only for the 6 left-most bits

      // Second sextet
      // Takes the last 2 (from left) bits of the first byte and the 4 first (from left) bits of the second byte
      out[out_bytes+1] = table[calculate(tb[0], 5, 2, 0) + calculate(tb[1], 3, 4, 4)]; 

      // Third sextet
      out[out_bytes+2] = table[calculate(tb[1], 5, 4, 0) + calculate(tb[2], 1, 2, 6)];

      // Fourth sextet
      out[out_bytes+3] = table[calculate(tb[2], 5, 6, 0)];

      out_bytes += 4;
   }

   // Checks for trailing
   if (trailing > 0)
   {
      // First sextet
      out[out_bytes] = table[calculate(cin[i], 5, 6, 2)];
      
      if (trailing == 1)
      {
         // Second sextet
         // Takes the last 2 (from left) bits of the first byte: the second added is not considered as it only contains zeroes. Same below.
         out[out_bytes+1] = table[calculate(cin[i], 5, 2, 0)];
         out[out_bytes+2] = '=';
         out[out_bytes+3] = '=';
      }
      else
      {
         // Second sextet
         // Takes the last 2 (from left) bits of the first byte and the 4 first (from left) bits of the second byte
         out[out_bytes+1] = table[calculate(cin[i], 5, 2, 0) + calculate(cin[i+1], 3, 4, 4)]; 
         // Third sextet
         out[out_bytes+2] = table[calculate(cin[i+1], 5, 4, 0)];
         out[out_bytes+3] = '=';
      }
   }

   out_bytes += 4;
   out[out_bytes] = 0;
}

int bit(char val, int pos)
{
   return (val >> pos) & 1;
}

int calculate(char val, int max_pow, int bits, int from)
{
   int res=0;
   for (int i=0; i<bits; i++)
   {
      int esp = max_pow - (bits-i-1);
      int ith_bit = bit(val, i+from);
      res += (ith_bit * power(2, esp));
   }
   return res;
}

int power(int a, int b) 
{
    int result = 1;
    for (int i = 0; i < b; i++) {
        result *= a;
    }
    return result;
}
