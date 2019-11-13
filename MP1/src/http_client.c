/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>


//#define PORT "80" // the port client will be connecting to

#define MAXDATASIZE 2056 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    long int bytessum = 0;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    FILE *fp = NULL;
    FILE *fpt = NULL;

    int succ_parsing = 0; // Whether the parsing has been
    char ip[100]; // IP field of the HTTP uri
    int port;  // Port field of the HTTP uri if found
    char dir[200]; // Dir field of the uri if found
    memset(ip, 0, 100);
    memset(dir, 0, 200);
    port = 80;

    if (argc != 2) {
        fprintf(stderr, "usage: URL\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (sscanf(argv[1], "http://%99[^:]:%i/%199[^\n]", ip, &port, dir) == 3) { succ_parsing = 1;}
    else if (sscanf(argv[1], "http://%99[^/]/%199[^\n]", ip, dir) == 2) { succ_parsing = 2;}
    else if (sscanf(argv[1], "http://%99[^:]:%i[^\n]", ip, &port) == 2) { succ_parsing = 3;}
    else if (sscanf(argv[1], "http://%99[^\n]", ip) == 1) { succ_parsing = 4;}

    char portstring[12];
    sprintf(portstring, "%d", port);
    printf("ip: %s\n", ip);
    printf("port: %s\n", portstring);
    printf("dir: %s\n", dir);
    if ((rv = getaddrinfo(ip, portstring, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);

    char str1[99] = "GET /";
    char str2[12] = " HTTP/1.1";

    strcat(str1, dir);
    strcat(str1, str2);
    strcat(str1, "\r\n");
    strcat(str1, "Host:");
    if (succ_parsing == 1 || succ_parsing == 3) {
        strcat(str1, ip);
        strcat(str1, ":");
        strcat(str1, portstring);
    } else {
        strcat(str1, ip);
    }

    strcat(str1, "\r\n");
    strcat(str1, "Connection: Keep-Alive");

    strcat(str1, "\r\n\r\n");
    printf("request: %s\n", str1);
    char *header = str1;
    if (send(sockfd, header, strlen(header), 0) == -1) {
        perror("client: send");
        exit(1);
    }
    printf("GET Sent...\n");
    fp = fopen("output", "w");
    //all right ! now that we're connected, we can receive some data!
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = 0;
    char response[100];
    char newbuff[MAXDATASIZE];
    int i = 0;
    int j = 0;
    for(; buf[i] != '\r'; ++i)
    {
        response[j] = buf[i];
        j++;
    }
    i = i + 4;
    for(j = 0; buf[i] != '\0';)
    {
        newbuff[j] = buf[i];
        i++;
        j++;
    }
    newbuff[j] = '\0';
    printf("response: %s\n", response);
    fwrite(newbuff, strlen(newbuff), 1, fp);
    bytessum += strlen(newbuff);
    while((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) > 0) {
        fwrite(buf, numbytes, 1, fp);
        bytessum += numbytes;
    }

    //printf("recv()'d %d bytes of data in buf\n", bytessum);
    printf("client: received '%ld'\n", bytessum);

    fclose(fp);

    freeaddrinfo(servinfo); // all done with this structure

    close(sockfd);

    return 0;
}
