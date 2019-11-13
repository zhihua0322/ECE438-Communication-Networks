/* 
 * File:   sender_main.c
 * Author: marvin
 *
 * Created on 
 */
//include and definition
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdbool.h>
//the maximum window size and rtt
#define maxcwnd 4096
#define mincwnd 1
#define headLength 50
#define msgLength 2422
#define bufsize 50
#define min(a, b) a < b ? a : b
#define max(a, b) a > b ? a : b
#define SLOW_GROWTH 16
#define SLOW_DECLINE 16

struct sockaddr_in;
//s is the udp socket
int s;
//definition of some variables
//msgBuffer is used to contain some information
int cwnd = 4;
int rtt = 20 * 1000;
FILE *fp;
double slowgrowth = 0.0;
struct addrinfo *p;
int bytesleft;
int fileRead[maxcwnd];
int frameCount = 0;
int frameIndex[maxcwnd];
int lastbytes = -1;
int lastindex = -1;
typedef struct {
    char segment[msgLength];
} Segment;

typedef struct {
    Segment data[maxcwnd];
    int front; //the front element
    int rear;
    int size;
} Queue;

Queue *CreateQueue() {
    Queue *q = (Queue *) malloc(sizeof(Queue));
    if (!q) {
        printf("no enough space\n");
        return NULL;
    }
    q->front = -1;
    q->rear = -1;
    q->size = 0;
    //memset(q->data, 0, sizeof(char) * msgLength * maxcwnd);
    return q;
}

int IsFullQ(Queue *q) {
    return (q->size == maxcwnd);
}

void AddQ(Queue *q, Segment item) {
    if (IsFullQ(q)) {
        printf("the space is full now\n");
        return;
    }
    if (q->size == cwnd) {
        return;
    }
    if (q->front == -1) {
        q->front = 0;
    }
    q->rear++;
    q->rear %= maxcwnd;
    q->size++;
    q->data[q->rear] = item;
}

int IsEmptyQ(Queue *q) {
    return (q->size == 0);
}

Segment DeleteQ(Queue *q) {
    if (IsEmptyQ(q)) {
        printf("empty queue to delete!\n");
        exit(1);
    }
    q->front++;
    q->front %= maxcwnd;
    q->size--;
//    memset(q->data + q->front, 0, sizeof(char) * 1024);
    return q->data[q->front];
}

void diep(char *s) {
    perror(s);
    exit(1);
}

int file_size2(char *filename) {
    struct stat statbuf;
    stat(filename, &statbuf);
    int size = statbuf.st_size;
    return size;
}

int createSocket(char *hostname, unsigned short int hostport) {
    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    int rv;
    int sockfd;
    char portstring[10];
    sprintf(portstring, "%d", hostport);
    if ((rv = getaddrinfo(hostname, portstring, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("create socket error");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "failed to find the host\n");
        return 2;
    }
    return sockfd;
}

int sendSegments(int framePointer, Queue *q) {
    int bytesRead;
//    printf("enter sendSegment\n");
    int extra = 0;
//    int index = q->front;
//    int filetraversed = 0;
//    while (filetraversed <= q->size) {
//        if (fileRead[index] == 2) {
//            extra++;
//        }
//        filetraversed++;
//        index++;
//        index %= maxcwnd;
//    }

    while (q->size < cwnd + extra) {
        if (lastindex == -1) {
            Segment newadd;
            bytesRead = bytesleft >= msgLength ? msgLength : bytesleft;
            fread(newadd.segment, 1, bytesRead, fp);
            AddQ(q, newadd);
            frameIndex[q->rear] = frameCount++;
            fileRead[q->rear] = 1;
            bytesleft -= bytesRead;
            if (bytesleft == 0) {
                lastindex = q->rear;
                lastbytes = bytesRead;
                break;
            }
//            printf("q size is %d q front is %d q rear is %d\n",q->size,q->front,q->rear);
        } else {
            break;
        }
    }
    int filesent = 0;
    int count = 0;
    int i = q->front;
//    for (i = q->front; i <= q->rear; i++) {
//        printf("q size is %d q front is %d q rear is %d\n", q->size, q->front, q->rear);
//        if (fileRead[i] == 1) {
//            printf("send current index %d\n", i);
//            char packet[msgLength + headLength];
//            sprintf(packet, "frame%d;", frameIndex[i]);
//            int start = strlen(packet);
//            if (i == lastindex) {
//                memcpy(packet + start, q->data[i].segment, lastbytes);
//                sendto(s, packet, start + lastbytes, 0, p->ai_addr, p->ai_addrlen);
//                count++;
//            } else {
//                memcpy(packet + start, q->data[i].segment, msgLength);
//                sendto(s, packet, start + msgLength, 0, p->ai_addr, p->ai_addrlen);
//                count++;
//            }
//        }
//    }
    while (filesent <= q->size) {
//        printf("q size is %d q front is %d q rear is %d\n", q->size, q->front, q->rear);
        if (fileRead[i] == 1) {
//            printf("send current index %d\n", i);
            char packet[msgLength + headLength];
            sprintf(packet, "frame%d;", frameIndex[i]);
            int start = strlen(packet);
            if (i == lastindex) {
                memcpy(packet + start, q->data[i].segment, lastbytes);
                sendto(s, packet, start + lastbytes, 0, p->ai_addr, p->ai_addrlen);
                count++;
            } else {
                memcpy(packet + start, q->data[i].segment, msgLength);
                sendto(s, packet, start + msgLength, 0, p->ai_addr, p->ai_addrlen);
                count++;
            }
        }
        filesent++;
        i++;
        i %= maxcwnd;
    }
//    printf("expected is %d\n", count);
    return count;
}

int changewindowsize(bool positive) {
    if (positive) {
        if (cwnd < SLOW_GROWTH) {
            cwnd += 1;
        } else {
            slowgrowth += 1.0 / cwnd;
            if (slowgrowth >= 1) {
                cwnd++;
                slowgrowth = 0;
            }
        }
        cwnd = min(cwnd, maxcwnd);
    } else {
        if (cwnd > SLOW_DECLINE) {
            cwnd /= 2;
        } else {
            cwnd--;
        }
        cwnd = max(cwnd, mincwnd);
    }
}

void reliablyTransfer(char *hostname, unsigned short int hostUDPport, char *filename,
                      unsigned long long int bytesToTransfer) {
    //Open the file
//    FILE *fp;
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Could not open file to send.");
        exit(1);
    } else {
        printf("file opened!\n");
    }
    bytesleft = min(bytesToTransfer, file_size2(filename));

    /* Determine how many bytes to transfer */

    Queue *q = CreateQueue();
    // create a socket based on the hostname and port
    s = createSocket(hostname, hostUDPport);
    struct sockaddr_in ret_addr;
    socklen_t addrlen = sizeof(ret_addr);
    int framePointer = 0;
    struct timeval TimeSent, AckRecv, TimeOut;
    TimeOut.tv_sec = 0;
    TimeOut.tv_usec = rtt * 6 / 3;
    char buffer[bufsize];
    int counter;
    int expectedAck = 0;
    memset(fileRead, 0, maxcwnd * sizeof(int));
    memset(frameIndex, 0, maxcwnd * sizeof(int));
    bool retransmission = false;
    int expectedAcks;
    while (1) {
//        printf("send a bunch of segments\n");

//        if(!retransmission){
//            expectedAcks = sendSegments(framePointer, q);
//        }else{
//                char packet[msgLength + headLength];
//                sprintf(packet, "frame%d;", frameIndex[q->front]);
//                int start = strlen(packet);
//                memcpy(packet + start, q->data[q->front].segment, msgLength);
//                sendto(s, packet, start + msgLength, 0, p->ai_addr, p->ai_addrlen);
//               printf("enter");
//                retransmission = false;
//        }
//        TimeOut.tv_sec = 0;
//        TimeOut.tv_usec = 2*rtt;
        gettimeofday(&TimeSent, 0);
        expectedAcks = sendSegments(framePointer, q);
//        printf("the remaining bytes is %d\n", bytesleft);
        gettimeofday(&TimeSent, 0);
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &TimeOut, sizeof(TimeOut));
        if (expectedAcks == 0) {
            break;
        }
        for (int i = 0; i < expectedAcks; i++) {
            int recvnums = recvfrom(s, buffer, bufsize, 0, (struct sockaddr *) &ret_addr, &addrlen);
            gettimeofday(&AckRecv, 0);
            if (recvnums < 0) {
                changewindowsize(false);
                counter++;
                if (counter >= 5) {
                    TimeOut.tv_usec = AckRecv.tv_usec - TimeSent.tv_usec;
                    counter = 0;
                }
                break;
            } else {
//                printf("received %d bytes\n",recvnums);
//                TimeOut.tv_usec = AckRecv.tv_usec - TimeSent.tv_usec;
//                gettimeofday(&AckRecv, 0);
                int ackNum = 0;
                buffer[recvnums] = '\0';
                int idx;
                char header[headLength];
                for (idx = 0; idx < recvnums; idx++) {
                    header[idx] = buffer[idx];
                    if (buffer[idx] == ';') {
                        header[idx] = '\0';
                        idx++;
                        break;
                    }
                }
                sscanf(header, "ack%d;", &ackNum);
//                printf("received ack %d\n", ackNum);
                if (ackNum == expectedAck) {
//                    TimeOut.tv_usec = 6*(AckRecv.tv_usec - TimeSent.tv_usec)/5;
                    expectedAck++;
                    framePointer++;
//                    printf("delete a q front %d\n", q->front);
                    DeleteQ(q);
                    framePointer %= maxcwnd;
                    fileRead[ackNum % maxcwnd] = 0;
                    changewindowsize(true);
                    int temp = ((ackNum) % maxcwnd + 1) % maxcwnd;
                    while (fileRead[temp] == 2) {
                        DeleteQ(q);
                        expectedAck++;
                        fileRead[temp] = 0;
                        temp++;
                        temp %= maxcwnd;
                    }
                } else if (ackNum > expectedAck) {
                    fileRead[ackNum % maxcwnd] = 2;
                    changewindowsize(true);
                }
            }
        }
    }

    /* Send data and receive acknowledgements on s*/
//    sendto(s, "frame0;sdjlakjsdlkajsd", sizeof("frame0;sdjlakjsdlkajsd"), 0, p->ai_addr, p->ai_addrlen);
    int eot;
    for (eot = 0; eot < 10; eot++) {
        sendto(s, "EOT", sizeof("EOT"), 0, p->ai_addr, p->ai_addrlen);
    }
    printf("the remaining bytes is %d\n", bytesleft);
    printf("Closing the socket\n");
    close(s);
    fclose(fp);
    //using for end of transmission
}


/*
 * 
 */
int main(int argc, char **argv) {

    unsigned short int udpPort;
    unsigned long long int numBytes;

    if (argc != 5) {
        fprintf(stderr, "usage: %s receiver_hostname receiver_port filename_to_xfer bytes_to_xfer\n\n", argv[0]);
        exit(1);
    }
    udpPort = (unsigned short int) atoi(argv[2]);
    numBytes = atoll(argv[4]);
    clock_t start = time(NULL);
    reliablyTransfer(argv[1], udpPort, argv[3], numBytes);
    clock_t end = time(NULL);
    double duration = ((double) end - start) / CLOCKS_PER_SEC;
    printf("duration: %lf\n", difftime(end, start));
    return (EXIT_SUCCESS);
}

