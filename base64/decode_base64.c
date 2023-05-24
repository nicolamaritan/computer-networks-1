#include <stdio.h>
#include <string.h>

#define MAX_IN 10000
#define uc2 (unsigned char)2
#define uc4 (unsigned char)4
#define uc6 (unsigned char)6

// Same table but shifted by '+' (libb64 source code)
//static const unsigned char table[] = {62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};

// Reverse lookup table
char table[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
                

void quartet(unsigned char* in, unsigned char* out);

int main()
{
   unsigned char in[MAX_IN+1];
   unsigned char out[MAX_IN+1];  // In any case, out will be smaller than in because of the base64 encoding
   fgets(in, MAX_IN, stdin);
   
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
   printf("%s\n", out);

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
