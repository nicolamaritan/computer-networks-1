#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <signal.h>
#include <assert.h>


int pid;
struct sockaddr_in local, remote, server;
char request[10000];
char request2[10000];
char response[1000];
char response2[10000];

struct header {
	char * n;
	char * v;
} h[100];

struct hostent * he;

int main()
{
	char hbuffer[10000];
	char buffer[2000];
	char * reqline;
	char * method, *url, *ver, *scheme, *hostname, *port;
	char * filename;
	FILE * fin;
	int c;
	int n;
	int i,j,t, s,s2,s3;
	int yes = 1;
	int len;
	if (( s = socket(AF_INET, SOCK_STREAM, 0 )) == -1)
	{ printf("errno = %d\n",errno); perror("Socket Fallita"); return -1; }
	local.sin_family = AF_INET;
	local.sin_port = htons(20161);
	local.sin_addr.s_addr = 0;

	t= setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	if (t==-1){perror("setsockopt fallita"); return 1;}

	if ( -1 == bind(s, (struct sockaddr *)&local,sizeof(struct sockaddr_in)))
	{ perror("Bind Fallita"); return -1;}

	if ( -1 == listen(s,10)) { perror("Listen Fallita"); return -1;}
	remote.sin_family = AF_INET;
	remote.sin_port = htons(0);
	remote.sin_addr.s_addr = 0;
	len = sizeof(struct sockaddr_in);
	while ( 1 ){
		s2=accept(s,(struct sockaddr *)&remote,&len);
		printf("Remote address: %.8X\n",remote.sin_addr.s_addr);
		if (fork()) continue;
		if(s2 == -1){perror("Accept fallita"); exit(1);}
		bzero(hbuffer,10000);
		bzero(h,100*sizeof(struct header));
		reqline = h[0].n = hbuffer;
		for (i=0,j=0; read(s2,hbuffer+i,1); i++) {
			printf("%c",hbuffer[i]);
			if(hbuffer[i]=='\n' && hbuffer[i-1]=='\r'){
				hbuffer[i-1]=0; // Termino il token attuale
				if (!h[j].n[0]) break;
				h[++j].n=hbuffer+i+1;
			}
			if (hbuffer[i]==':' && !h[j].v && j>0){
				hbuffer[i]=0;
				h[j].v = hbuffer + i + 1;
			}
		}

		printf("Request line: %s\n",reqline);
		method = reqline;
		for(i=0;i<100 && reqline[i]!=' ';i++); reqline[i++]=0; 
		url=reqline+i;
		for(;i<100 && reqline[i]!=' ';i++); reqline[i++]=0; 
		ver=reqline+i;
		for(;i<100 && reqline[i]!='\r';i++); reqline[i++]=0; 
		if ( !strcmp(method,"GET")){
			scheme=url;
			// GET http://www.aaa.com/file/file 
			printf("url=%s\n",url);
			for(i=0;url[i]!=':' && url[i] ;i++);
			if(url[i]==':') url[i++]=0;
			else {printf("Parse error, expected ':'"); exit(1);}
			if(url[i]!='/' || url[i+1] !='/') 
			{printf("Parse error, expected '//'"); exit(1);}
			i=i+2; hostname=url+i;
			for(;url[i]!='/'&& url[i];i++);	
			if(url[i]=='/') url[i++]=0;
			else {printf("Parse error, expected '/'"); exit(1);}
			filename = url+i;
			printf("Schema: %s, hostname: %s, filename: %s\n",scheme,hostname,filename); 

			he = gethostbyname(hostname);
			printf("%d.%d.%d.%d\n",(unsigned char) he->h_addr[0],(unsigned char) he->h_addr[1],(unsigned char) he->h_addr[2],(unsigned char) he->h_addr[3]); 
			if (( s3 = socket(AF_INET, SOCK_STREAM, 0 )) == -1)
			{ printf("errno = %d\n",errno); perror("Socket Fallita"); exit(-1); }

			server.sin_family = AF_INET;
			server.sin_port =htons(80);
			server.sin_addr.s_addr = *(unsigned int *)(he->h_addr);

			if(-1 == connect(s3,(struct sockaddr *) &server, sizeof(struct sockaddr_in)))
			{perror("Connect Fallita"); exit(1);}	

			#define SIZE 100000
			int firstBytePos = 0;
			int lastBytePos = SIZE-1;
			char piece[SIZE];
			int p=0;


			int toExit = 0;

			char filenameStr[100];
			char hostnameStr[100];

			strcpy(filenameStr, filename);
			strcpy(hostnameStr, hostname);


			sprintf(request,"HEAD /%s HTTP/1.1\r\nHost:%s\r\n\r\n",filenameStr,hostnameStr);
			write(s3,request,strlen(request));

			printf("**************server header parsing\n");
			// Parsing headers
			bzero(hbuffer,10000);
			bzero(h,100*sizeof(struct header));
			reqline = h[0].n = hbuffer;
			for (i=0,j=0; read(s3,hbuffer+i,1); i++) {
				printf("%c",hbuffer[i]);
				if(hbuffer[i]=='\n' && hbuffer[i-1]=='\r'){
					hbuffer[i-1]=0; // Termino il token attuale
					if (!h[j].n[0]) break;
					h[++j].n=hbuffer+i+1;
				}
				if (hbuffer[i]==':' && !h[j].v && j>0){
					hbuffer[i]=0;
					h[j].v = hbuffer + i + 1;
				}
			}

			int fullCl;
			for (i=0; h[i].n[0]; i++)
			{
				//printf("%s:%s\n", h[i].n, h[i].v);
				if (strcmp("Content-Length", h[i].n) == 0)
				{
					fullCl = atoi(h[i].v+1);
				}
			}

			printf("fullImageLen: %d\n", fullCl);
			sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type:image/jpeg\r\nContent-Length:%d\r\n\r\n", fullCl);
			write(s2, response, strlen(response));


			int cl;
			toExit = 1;
			while (1)
			{
				sprintf(request,"GET /%s HTTP/1.1\r\nHost:%s\r\nRange:bytes=%d-%d\r\n\r\n",filenameStr,hostnameStr, firstBytePos, lastBytePos);

				printf("%s, from socket %d\n", request, s2);
				write(s3,request,strlen(request));

				printf("**************server header parsing\n");
				// Parsing headers
				bzero(hbuffer,10000);
				bzero(h,100*sizeof(struct header));
				reqline = h[0].n = hbuffer;
				for (i=0,j=0; read(s3,hbuffer+i,1); i++) {
					printf("%c",hbuffer[i]);
					if(hbuffer[i]=='\n' && hbuffer[i-1]=='\r'){
						hbuffer[i-1]=0; // Termino il token attuale
						if (!h[j].n[0]) break;
						h[++j].n=hbuffer+i+1;
					}
					if (hbuffer[i]==':' && !h[j].v && j>0){
						hbuffer[i]=0;
						h[j].v = hbuffer + i + 1;
					}
				}


				int cl;
				toExit = 1;
				for (i=0; h[i].n[0]; i++)
				{
					//printf("%s:%s\n", h[i].n, h[i].v);
					if (strcmp("Content-Length", h[i].n) == 0)
					{
						cl = atoi(h[i].v+1);
					}
					if (strcmp("Content-Range", h[i].n) == 0)
					{
						toExit = 0;
					}
				}

				for(i=0;i<cl && (t=read(s3,piece+i,SIZE-i));i+=t);
				for(i=0;i<cl && (t=write(s2,piece+i,cl-i));i+=t);

				printf("read bytes=%d-%d\n\n", firstBytePos, firstBytePos+cl);
				firstBytePos += SIZE;
				lastBytePos += SIZE;

				if (toExit)
					break;	
				p++;
			}
			printf("closing s3 and s2, p=%d\n", p);
			close(s2);
			close(s3);


		}
		else if(!strcmp("CONNECT",method)) { // it is a connect  host:port 
			hostname=url;
			for(i=0;url[i]!=':';i++); url[i]=0;
			port=url+i+1;
			printf("hostname:%s, port:%s\n",hostname,port);
			he = gethostbyname(hostname);
			if (he == NULL) { printf("Gethostbyname Fallita\n"); return 1;}
			printf("Connecting to address = %u.%u.%u.%u\n", (unsigned char ) he->h_addr[0],(unsigned char ) he->h_addr[1],(unsigned char ) he->h_addr[2],(unsigned char ) he->h_addr[3]); 			
			s3=socket(AF_INET,SOCK_STREAM,0);

			if(s3==-1){perror("Socket to server fallita"); return 1;}
			server.sin_family=AF_INET;
			server.sin_port=htons((unsigned short)atoi(port));
			server.sin_addr.s_addr=*(unsigned int*) he->h_addr;			
			t=connect(s3,(struct sockaddr *)&server,sizeof(struct sockaddr_in));		
			if(t==-1){perror("Connect to server fallita"); exit(0);}
			sprintf(response,"HTTP/1.1 200 Established\r\n\r\n");
			write(s2,response,strlen(response));
			// <==============
			if(!(pid=fork())){ //Child
				while(t=read(s2,request2,2000)){	
					write(s3,request2,t);
					//printf("CL >>>(%d)%s \n",t,hostname); //SOLO PER CHECK
				}	
				exit(0);
			}
			else { //Parent	
				while(t=read(s3,response2,2000)){	
					write(s2,response2,t);
					//printf("CL <<<(%d)%s \n",t,hostname);
				}	
				kill(pid,SIGTERM);
				close(s3);
			}	
		}	
		else {
			sprintf(response,"HTTP/1.1 501 Not Implemented\r\n\r\n");
			write(s2,response,strlen(response));
		}
		close(s2);
		exit(1);
	}
	close(s);
}

