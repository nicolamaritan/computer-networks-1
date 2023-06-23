#include <sys/types.h>          /* See NOTES */ 
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>       
#define _XOPEN_SOURCE       /*  See  feature_test_macros(7)*/
#include <time.h>


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
char * request = "GET / HTTP/1.1\r\nHost:www.google.com\r\n\r\n";
const char*  requestIfMod = "GET / HTTP/1.1\r\nIf-Modified-Since:%s\r\nHost:88.80.187.84\r\n\r\n";

char request2[100];
unsigned char entity[ENTITY_SIZE+1];

//
char* lastModified;
struct tm lmTm;
time_t lmTime;
FILE* f;
char cachedLastModified[100];

time_t toTime(char* str)
{
	printf("str: %s\n", str);
	struct tm temp;
	bzero(&temp, sizeof(temp));
	strptime(str, "%a, %d %b %Y %T %Z", &temp);
	return mktime(&temp);
}

int main()
{
	int i,j,s,t;
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
	ip[0]=88; ip[1]=80;ip[2]=187;ip[3]=84;
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

	lastModified = 0;
	lmTime=0;

	for(i=0;h[i].n[0];i++){
		printf("h[%d].n ---> %s , h[%d].v ---> %s\n",i,h[i].n,i,h[i].v);
		if(!strcmp(h[i].n,"Content-Length"))
			length=atoi(h[i].v);

		if (!strcmp(h[i].n, "Last-Modified"))
		{
			lastModified=h[i].v+1;
		}

	}

	if (lastModified)
	{
		lmTime = toTime(lastModified);
		printf("lmTime: %d\n", lmTime);
	}

	if(length == -1) length = 5000;

	for(i=0;i<length && (t=read(s,entity+i,ENTITY_SIZE-i));i+=t);
	entity[i]=0;
	printf("%s",entity);

	if ((f=fopen("cache", "w")) == NULL)
	{
		exit(1);
	}

	fwrite(lastModified, 1, strlen(lastModified), f);
	fwrite(entity, 1, strlen(entity), f);
	fclose(f);


	// Second server request
	if ((f = fopen("cache", "r")) == NULL)
	{
		exit(1);
	}

	char ch;
	int cnt =0;
	while ((ch=fgetc(f)) != EOF && ch != '\n')
	{
		cachedLastModified[cnt] = ch;
		cnt++;
	}
	cachedLastModified[cnt] = 0;
	printf("cachedLastModified: %s\n", cachedLastModified);

	sprintf(request2, requestIfMod, cachedLastModified);
	printf("req2:********%s************\n", request2);
	
	write(s, request2, strlen(request2));

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

	}

	if (strcmp(sl, "HTTP/1.1 200 OK") == 0)
	{
		for(i=0;i<length && (t=read(s,entity+i,ENTITY_SIZE-i));i+=t);
		entity[i]=0;
		printf("%s",entity);
	}
	else
	{
		fseek(f, cnt+1, SEEK_SET);
		while ((ch=fgetc(f)) != EOF)
		{
			printf("%c", ch);
		}

	}

}

