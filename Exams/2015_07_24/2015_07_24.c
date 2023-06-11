#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_HBUF 5000
#define MAX_H 100
#define MAX_SL 100
#define MAX_REQ 5000
#define MAX 1000
#define MAX_ENT 5000

struct header
{
	char* n;
	char* v;
} h[MAX_H];

char sl[MAX_SL+1];
char hbuf[MAX_HBUF+1];
char ent[MAX_ENT+1];

int month(const char* s)
{
	if (strcmp(s, "Jan") == 0)
		return 0;
	if (strcmp(s, "Feb") == 0)
		return 1;
	if (strcmp(s, "Mar") == 0)
		return 2;
	if (strcmp(s, "Apr") == 0)
		return 3;
	if (strcmp(s, "May") == 0)
		return 4;
	if (strcmp(s, "Jun") == 0)
		return 5;
	if (strcmp(s, "Jul") == 0)
		return 6;
	if (strcmp(s, "Aug") == 0)
		return 7;
	if (strcmp(s, "Sep") == 0)
		return 8;
	if (strcmp(s, "Oct") == 0)
		return 9;
	if (strcmp(s, "Nov") == 0)
		return 10;
	if (strcmp(s, "Dec") == 0)
		return 11;
	assert(0);
}

time_t toTime(char* val)
{
	struct tm date;
	// Lazy parsing
	val[3] = 0;
	val[7] = 0;
	val[11] = 0;
	val[16] = 0;
	val[19] = 0;
	val[22] = 0;
	val[25] = 0;
	date.tm_sec = atoi(val+23);
	date.tm_min = atoi(val+20);
	date.tm_hour = atoi(val+17);
	date.tm_year = atoi(val+12)-1900;
	date.tm_mon = month(val+8);
	date.tm_mday = atoi(val+4);

	return  mktime(&date);
}

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
	//printf("path:%s\n", path);
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
	char fileDate[MAX];
	char fileBody[MAX];
	time_t valTime;
	int len = -1;
	for (j=0; h[j].n[0]; j++)
	{
		printf("%s:%s\n", h[j].n, h[j].v);
		if (strcmp(h[j].n, "Last-Modified") == 0)
		{
			char* val = h[j].v+1;
			valTime = toTime(val);
			//time_t valTime = mktime(gmtime(val));
			printf("valTime:%d\n", valTime);

			if (access(fileName, F_OK) == 0)
			{
				printf("Found cached file\n");
				fin = fopen(fileName, "r");

				char ch;
				i=0;
				while ( (ch=fgetc(fin)) != '\n')
				{
					fileDate[i] = ch;
					i++;
				}
				fileDate[i] = 0;
				printf("fileDate from file:%s--\n", fileDate);
				unsigned int fileDateInt = atoi(fileDate);
				
				i=0;
				while ( (ch=fgetc(fin)) != EOF)
				{
					fileBody[i] = ch;
					i++;
				}
				fileBody[i] = 0;
				fclose(fin);
			
				//printf("%s\n%s", fileDate, fileBody);
				printf("valTime: %d\nfileDateInt: %d\n", valTime, fileDateInt);

				if (valTime > fileDateInt)
				{
					reqAgain = 1;
				}

			}
			else
			{
				reqAgain = 1;
			}
		}
		if (strcmp(h[j].n, "Content-Length") == 0)
		{
			len = atoi(h[j].v+1);
		}
	}

	int t=0;
	i=0;
	if (reqAgain > 0)
	{
		printf("Reading again\n");
		for(i=0; i<len && (t=read(s, ent+i, MAX_ENT-i)); i+=t);
		ent[i] = 0;
		printf("Entity:%s\n", ent);

		char valTimeStr[MAX];
		sprintf(valTimeStr, "%d", valTime);

		// Save in cache
		fin = fopen(fileName, "w");
		for (i=0; i<strlen(valTimeStr); i++)
		{
			fputc(valTimeStr[i], fin);
		}
		fputc('\n', fin);
		for (i=0; i<len; i++)
		{
			fputc(ent[i], fin);
		}
		fclose(fin);
	}
	else
	{
		printf("Reading from cache\n");
		printf(fileBody);
	}

}
