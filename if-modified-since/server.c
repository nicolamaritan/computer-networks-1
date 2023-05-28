#define _GNU_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#define MAX_RES 1000

int date_compare(const char* date1, const char* date2)
{
    struct tm tm1, tm2;
    time_t time1, time2;

    // Convert the first HTTP-Date string to time_t
    strptime(date1, "%a, %d %b %Y %H:%M:%S %Z", &tm1);
    time1 = mktime(&tm1);

    // Convert the second HTTP-Date string to time_t
    strptime(date2, "%a, %d %b %Y %H:%M:%S %Z", &tm2);
    time2 = mktime(&tm2);

    // Compare the time_t values
    if (time1 < time2)
        return -1;
    else if (time1 > time2)
        return 1;
    else
        return 0;
}

int main()
{
    int s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        perror("Socket creation failed\n");
        printf("%d\n", errno);
        return 1;
    }
    int opt = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
    {
        perror("Socket options failed\n");
        printf("%d\n", errno);
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8081);
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("Socket binding failed\n");
        printf("%d\n", errno);
        return 1;
    }

    const int BACKLOG = 10;
    if (listen(s, BACKLOG) < 0)

    {
        perror("Socket listen failed\n");
        printf("%d\n", errno);
        return 1;
    }

    while (1)
    {
        struct sockaddr_in remote_addr;
        int size = sizeof(remote_addr);
        int c = accept(s, (struct sockaddr*)&remote_addr, &size);

        if (c == -1)
        {
            perror("Remote connection failed\n");
            printf("%d\n", errno);
            continue;
        }

        printf("Connection accepted.\n");

        const int RL_SIZE = 100;
        const int HBUF_SIZE = 10000;
        char rl[RL_SIZE];
        char hbuf[HBUF_SIZE];

        int i;

        // Response line parsing
        for (i=0; i<RL_SIZE && read(c, rl+i, 1) && rl[i] != '\n' && rl[i-1] != '\r'; i++);
        rl[i] = 0;

        const int H_SIZE = 100;
        int j;
        struct header
        {
            char* n;
            char* v;
        } h[H_SIZE];

        h[0].n = hbuf;
        h[0].v = 0;
        // Header parsing
        for (i=0, j=0; i<HBUF_SIZE && j<H_SIZE && read(c, hbuf+i, 1); i++)
        {
            if (hbuf[i] == '\n' && hbuf[i-1] == '\r')
            {
                hbuf[i-1] = 0;
                if (h[j].n[0] == 0)
                {
                    break;
                }
                h[++j].n = hbuf+i+1;  
                h[j].v = 0;
            }
            else if (hbuf[i] == ':' && !h[j].v)
            {
                hbuf[i] = 0;
                h[j].v = hbuf+i+1;
            }
        }

        char response[MAX_RES+1];

        // Print request line and headers
        printf("%s\n", rl);
        for (j=0; h[j].n[0]; j++)
        {
            printf("%s : %s\n", h[j].n, h[j].v);
            if (strcmp(h[j].n, "If-Modified-Since") == 0)
            {
                char* last_modified = "Wed, 28 Apr 2023 20:31:05 GMT";
                int diff = date_compare(last_modified, h[j].v);

                if (diff > 0)
                {
                    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Lenght:12\r\nHello World!");
                }
                else
                {

                    sprintf(response, "HTTP/1.1 304 Not Modified\r\n\r\n");
                }
            }
        }

        while (send(c, response, strlen(response), 0) < 0)
        {
            perror("Error writing\n");
            printf("%d\n", errno);
        }

        printf("End of connection\n**********************************\n\n");
        close(c);
    }
}


