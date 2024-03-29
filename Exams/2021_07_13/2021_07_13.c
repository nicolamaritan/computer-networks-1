// Thanks to Davide Molinaroli

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

#define LEN 10000

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

char entity[2000000];
int firstByte;

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


			char filename2[100];
			char hostname2[100];
			char contType[100];

			strcpy(filename2, filename);
			strcpy(hostname2, hostname);


			firstByte = 0;
			int len=0;
			for (int b=0; 1; b++)
			{
				if (( s3 = socket(AF_INET, SOCK_STREAM, 0 )) == -1)
				{ printf("errno = %d\n",errno); perror("Socket Fallita"); exit(-1); }

				server.sin_family = AF_INET;
				server.sin_port =htons(80);
				server.sin_addr.s_addr = *(unsigned int *)(he->h_addr);

				if(-1 == connect(s3,(struct sockaddr *) &server, sizeof(struct sockaddr_in)))
				{perror("Connect Fallita"); exit(1);}	

				sprintf(request,"GET /%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\nRange:bytes=%d-%d\r\n\r\n",filename2,hostname2, firstByte, firstByte+LEN-1);
				printf("%s\n",request);
				write(s3,request,strlen(request));

				// Header parsing
				bzero(hbuffer,10000);
				bzero(h,100*sizeof(struct header));
				reqline = h[0].n = hbuffer;
				for (i=0,j=0; read(s3,hbuffer+i,1); i++) {
					//printf("%c",hbuffer[i]);
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

				for (i=0; h[i].n[0]; i++)
				{
					if (b==0)
					{
						if (strcmp(h[i].n, "Content-Range") == 0)
						{
							len = atoi(strstr(h[i].v, "/")+1);
							printf("len: %d\n", len);
						}
						else if (strcmp(h[i].n, "Content-Type") == 0)
						{
							strcpy(contType, h[i].v+1);
							printf("contType: %s\n", contType);
						}

					}
				}


				/*while ( t=read(s3,buffer,2000))
				  write(s2,buffer,t);*/

				for (t=0, i=0; (t=read(s3, entity+i+firstByte, LEN-i)); i+=t);

				close(s3);

				firstByte += LEN;
				if (firstByte >= len)
					break;
			}

			sprintf(response, "HTTP/1.1 200 OK\r\nContent-Length:%d\r\nConnection:close\r\nContent-Type:%s\r\n\r\n", len, contType);
			write(s2, response, strlen(response));
			for (t=0, i=0; i<len && (t=write(s2, entity+i, len-i)); i+=t);
			close(s2);


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

