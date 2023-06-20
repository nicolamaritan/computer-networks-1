#include <sys/types.h>          /* See NOTES */ 
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <time.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <unistd.h>


#define ENTITY_SIZE 1000000
struct headers{
	char * n;
	char * v;
};

struct headers h[100];
char sl[1001];
char hbuf[5000];

//       int socket(int domain, int type, int protocol);

struct sockaddr_in remote_addr;
unsigned char * ip;
char * request = "GET / HTTP/1.1\r\nHost:www.example.com\r\n\r\n";
char* cacheFile = "cache";
FILE* cache;

char request2[100];
unsigned char entity[ENTITY_SIZE+1];

struct tm expiresTm;
struct tm dateTm;
time_t expiresTime;
time_t dateTime;

int main()
{
	int i,j,s,t;
	char ch;

	// Reads from cached file if caches exists and localExp >= time, then returns
	if (!access(cacheFile, F_OK))
	{
		printf("Cache found\n");
		cache = fopen(cacheFile, "r");
		char firstLine[100];
		for (i=0; (ch=fgetc(cache)) != EOF && ch != '\n'; i++)
		{
			firstLine[i] = ch;
		}
		firstLine[i] = 0;
		time_t localExpiration = atoi(firstLine);
		printf("localExpiration: %d\n", localExpiration);

		if (time(0) <= localExpiration)
		{
			while ((ch=fgetc(cache)) != EOF)
			{
				printf("%c", ch);
			}
			return 0;
		}

	}

	int length = -1;
	if (-1 ==(s = socket(AF_INET, SOCK_STREAM, 0))) {
		perror("Socket fallita");
		printf("%d\n",errno);
		return 1;
	}
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(80);
	ip = (unsigned char*)&remote_addr.sin_addr.s_addr; 
	//ip[0]=142; ip[1]=250;ip[2]=200;ip[3]=36;
	//147.162.235.155
	//ip[0]=88; ip[1]=80;ip[2]=187;ip[3]=84;
	remote_addr.sin_addr.s_addr = inet_addr("93.184.216.34");
	t = connect(s,(struct sockaddr *) &remote_addr,sizeof(struct sockaddr_in));
	if(t ==-1) {
		perror("Connect Fallita\n");
		return 1;
	}
	for(t=0;request[t];t++);
	write(s,request,t);
	for(i=0;i<1000 && read(s,sl+i,1) && (sl[i]!='\n' || sl[(i)?i-1:i]!='\r');i++){ }
	sl[i]=0;
	printf("Status Line ----> %s\n ", sl);

	h[0].n = & hbuf[0];
	for(j=0,i=0;i<5000 && read(s,hbuf+i,1);i++){
		if(hbuf[i]=='\n' && hbuf[(i)?i-1:i]=='\r'){
			hbuf[i-1]=0;
			if(h[j].n[0] == 0) break;
			h[++j].n=hbuf+i+1;
		}
		else if(hbuf[i]==':' && ! h[j].v ){
			hbuf[i]=0;
			h[j].v=hbuf+i+1;
		}		
	}
	for(i=0;h[i].n[0];i++){
		printf("h[%d].n ---> %s , h[%d].v ---> %s\n",i,h[i].n,i,h[i].v);
		if(!strcmp(h[i].n,"Content-Length"))
			length=atoi(h[i].v);
		if (!strcmp(h[i].n, "Date"))
		{
			memset(&expiresTm, 0, sizeof(struct tm));
			strptime(h[i].v+1, "%a, %d %b %Y %T %Z", &dateTm);
			dateTime = mktime(&dateTm);
		}
		if (!strcmp(h[i].n, "Expires"))
		{
			memset(&expiresTm, 0, sizeof(struct tm));
			strptime(h[i].v+1, "%a, %d %b %Y %T %Z", &expiresTm);
			expiresTime = mktime(&expiresTm);
		}
	}

	printf("dateTime: %d\nexpiresTime: %d\n", dateTime, expiresTime);
	// See professor's notes
	time_t localExpiration = time(0) + expiresTime - dateTime;
	char localExpirationStr[100];
	sprintf(localExpirationStr, "%d", localExpiration);


	if(length == -1) length = 5000;

	for(i=0;i<length && (t=read(s,entity+i,ENTITY_SIZE-i));i+=t);
	entity[i]=0;
	printf("%s",entity);

	// Write local expiration and entity in the cache
	cache = fopen(cacheFile, "w");
	if (cache != NULL)
	{
		for (i=0; i<strlen(localExpirationStr); i++)
		{
			fputc(localExpirationStr[i], cache);
		}
		fputc('\n', cache);
		for (i=0; i<strlen(entity); i++)
		{
			fputc(entity[i], cache);
		}
		fclose(cache);

	}
}

