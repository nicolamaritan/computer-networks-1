#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>
#include <assert.h>


int isLittleEndian()
{
	short one = 1;
	char* p = (char*) &one;
	if (*p==1)
		return 1;
	return 0;
}

int isBigEndian()
{
	return !isLittleEndian();
}

void swap(char* host, char* network, const int BYTES)
{
        for (int i = 0; i < BYTES; i++)
        {
                network[i] = host[BYTES-i-1];
        }
}

uint32_t my_htonl(uint32_t hostlong)
{
	if (isBigEndian())
		return hostlong;

	uint32_t networklong;
	swap((char*)&hostlong, (char*)&networklong, 4);
	return *(uint32_t*)&networklong;
}

uint16_t my_htons(uint16_t hostshort)
{

	if (isBigEndian())
		return hostshort;

	uint16_t networkshort;
	swap((char*)&hostshort, (char*)&networkshort, 2);
	return *(uint16_t*)&networkshort;
}

int main()
{
	const int TEST = 100;
	for (int i = 0; i < TEST; i++)
	{
		assert(htons(i)==my_htons(i));
		assert(htonl(i)==my_htonl(i));	
	}
	
	printf("All asserts were successful.\n");
	return 0;
}

