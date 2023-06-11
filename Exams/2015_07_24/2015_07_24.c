#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <string.h>

#define MAX_HBUF 5000
#define MAX_H 100
#define MAX_SL 100
#define MAX_REQ 5000
#define MAX 1000

struct header
{
	char* n;
	char* v;
} h[MAX_H];

char sl[MAX_SL+1];
char hbuf[MAX_HBUF+1];


int main()
{
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
	{
		perror("socket failed\n");
		return 1;
	}

	struct sockaddr_in rem;
	rem.sin_family = AF_INET;
	rem.sin_port = htons(80);
	char* ptr = (char*)&rem.sin_addr.s_addr;
	ptr[0] = 93;
	ptr[1] = 184;
	ptr[2] = 216;
	ptr[3] = 34;
 
	if (connect(s, (struct sockaddr*)&rem, sizeof(rem)) < 0)
	{
		perror("connect failed\n");
		return 1;
	}

	// Write request
	char req[MAX_REQ+1];
	sprintf(req, "GET / HTTP/1.1\r\nHost:www.example.com\r\n\r\n");
	write(s, req, strlen(req));

	int i;
	for (i=0; i<MAX_SL && read(s, sl+i, 1) && sl[i] != '\n' && sl[i-1] != '\r'; i++);
	sl[i-1] = 0;
	printf("Status-Line:%s\n", sl);

	int j;
	h[0].n = hbuf;
	h[0].v = 0;
	for (i=0, j=0; i<MAX_HBUF && j<MAX_H && read(s, hbuf+i, 1); i++)
	{
		if (hbuf[i] == '\n' && hbuf[i-1] == '\r')
		{
			hbuf[i-1] = 0;
			if (h[j].n[0] == 0)
				break;
			h[++j].n = hbuf+i+1;
			h[j].v = 0;
		}
		else if (hbuf[i] == ':' && !h[j].v)
		{
			hbuf[i] = 0;
			h[j].v = hbuf+i+1;
		}
	}

	// Parse req line to find path
	for (i=0; req[i] != ' '; i++);
	req[i++] = 0;
	char* path = req+i;
	printf("path:%s\n", path);
	for (; req[i] != ' '; i++)
	{
		if (req[i] == '/')
			req[i] = '_';
	}
	req[i++] = 0;

	// Filename in format example_sub1_sub2_
	char fileName[MAX];
	sprintf(fileName, "cache/example%s", path);
	printf("fileName: %s\n", fileName);

	FILE* fin;
	int reqAgain = 0;
	for (j=0; h[j].n[0]; j++)
	{
		printf("%s:%s\n", h[j].n, h[j].v);
		if (strcmp(h[j].n, "Last-Modified") == 0)
		{
			if (access(fileName, F_OK) == 0)
			{
				printf("Found cached file\n");
				fin = fopen(fileName, "r");
				char date[MAX];
				char body[MAX];
				char ch;
				i=0;
				while ( (ch=fgetc(fin)) != '\n')
				{
					date[i] = ch;
					i++;
				}
				i=0;
				while ( (ch=fgetc(fin)) != EOF)
				{
					body[i] = ch;
					i++;
				}

				printf("%s\n%s", date, body);

			}
			else
			{
				reqAgain = 1;
			}
		}
	}

}
