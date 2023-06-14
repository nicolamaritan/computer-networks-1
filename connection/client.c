#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>

#define MAX_HBUF 10000
#define MAX_H 100
#define MAX_SL 100
#define MAX_ENT 100000

char sl[MAX_SL+1];
char hbuf[MAX_HBUF+1];
struct header
{
		char* n;
		char* v;
} h[MAX_H];
char ent[MAX_ENT+1];

int main()
{
		int s = socket(AF_INET, SOCK_STREAM, 0);
		if (s<0)
		{
				perror("socket failed\n");
				return 1;
		}

		struct sockaddr_in rem;
		rem.sin_family = AF_INET;
		rem.sin_port = htons(80);
		char* ptr = (char*)&rem.sin_addr.s_addr;
		//142.250.184.110
		ptr[0] = 142;
		ptr[1] = 250;
		ptr[2] = 184;
		ptr[3] = 110;

		if (connect(s, (struct sockaddr*)&rem, sizeof(rem)) < 0)
		{
				perror("connect failed\n");
				return 1;
		}

		// Request triggers 404 file not found
		char* req = "GET /ppp HTTP/1.1\r\nConnection:close\r\nHost:www.google.com\r\n\r\n";

		write(s, req, strlen(req));

		int i;
		for(i=0; i<MAX_SL && read(s, sl+i, 1) && sl[i] != '\n' && sl[i-1] != '\r'; i++);
		sl[i] = 0;

		printf("Status-Line:%s\n", sl);

		int j;
		h[0].n = hbuf;
		for (i=0, j=0; i<MAX_HBUF && j<MAX_H && read(s, hbuf+i, 1); i++)
		{
				if (hbuf[i] == '\n' && hbuf[i-1] == '\r')
				{
						hbuf[i-1] = 0;
						if (h[j].n[0] == 0)
								break;
						h[++j].n = hbuf+i+1;
				}
				else if (hbuf[i] == ':' && !h[j].v)
				{
						hbuf[i] = 0;
						h[j].v = hbuf+i+1;
				}
		}


		int len = -1;
		for(j=0; h[j].n[0]; j++)
		{
				printf("%s:%s\n", h[j].n, h[j].v);
				if (strcmp(h[j].n, "Content-Length") == 0)
				{
						len = atoi(h[j].v+1);
				}
		}

		if (len > 0)
		{
				int t;
				for (i=0, t=0; i<len && i<MAX_ENT && (t = read(s, ent+i, MAX_ENT-i)); i += t){printf("reading i=%d\n", i);};
				printf("%s\n", ent);
		}

		/*
		 * If Connection:close in the request, this read returns 0 and the client exits the program.
		 * If Connection:keep-alive in the request, this read does not return 0 (at least immediatly)
		 * and the program is stuck for some time, as the connection is not closed and the
		 * server does not send anything, even if is has nothing more to send. The client
		 * could send other requests.
		 * */
		while(read(s, ent+i, MAX_ENT-i));
}
