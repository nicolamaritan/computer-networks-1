#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int countOccurrences(char* str, char* word)
{
    int count = 0;
    char* pos = strstr(str, word);
    
    while (pos != NULL) {
        count++;
        pos = strstr(pos + 1, word);
    }
    
    return count;
}

int main(int argc, char* argv[], char* env[])
{
   char* word = getenv("word");
   char* text = argv[1];
   int occ = countOccurrences(text, word);

   printf("<!DOCTYPE html><html><body><h1>Occurencies finder ;)</h1><p>In the text you submitted:</p><p>%s</p><p>The word %s was found %d times.</p></body></html>", text, word, occ);
   
}
