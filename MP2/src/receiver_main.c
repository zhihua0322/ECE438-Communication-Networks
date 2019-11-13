/*
 * File:   receiver_main.c
 * Author: 
 *
 * Created on
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define BUFSIZE 2500
#define HEADER_SIZE 50
#define DATA_SIZE 2422

struct sockaddr_in si_me, si_other;
int s, slen;

void diep(char *s) {
    perror(s);
    exit(1);
}

typedef struct Node {
    char *data;
    struct Node *next, *prev;
} Node;

typedef struct LinkedList {
    struct Node *root, *tail;
    size_t _size;
} LinkedList;

// Constructor for the Node.
Node *constructNode(char *data) {
    struct Node *temp = malloc(sizeof(struct Node));
    char tempData[DATA_SIZE];
//    char *tempData = (char *) malloc(DATA_SIZE);
    memcpy(tempData, data, DATA_SIZE);
    temp->data = tempData;
    temp->prev = NULL;
    temp->next = NULL;
    return temp;
}

// Constructor for the LinkedList.
LinkedList *constructList() {
    struct LinkedList *temp = malloc(sizeof(struct LinkedList));
    temp->root = NULL;
    temp->tail = NULL;
    temp->_size = 0;
    return temp;
}

// Locates a Node within the LinkedList based on the index.
Node *find(struct LinkedList *list, unsigned int index) {
    if (index > list->_size)
        fprintf(stderr, "%s%d%s%zu%c\n",
                "Error: Index Out of Bounds. Index was '",
                index, "\' but size is ", list->_size, '.');
    else if (list->_size > 0)
        if (index == 0)
            return list->root;
        else if (index == list->_size - 1)
            return list->tail;
        else {
            struct Node *temp;

            if ((double) index / list->_size > 0.5) {
                // Seek from the tail.
                temp = list->tail;
                for (unsigned int i = list->_size - 1; i > index; i--)
                    temp = temp->prev;
            } else {
                // Seek from the root.
                temp = list->root;
                for (unsigned int i = 0; i < index; i++)
                    temp = temp->next;
            }

            return temp;
        }

    return NULL;
}

// Inserts a Node at the front of the LinkedList.
void add(struct LinkedList *list, char *data) {
    struct Node *inserted = constructNode(data);

    if (list->_size == 0) {
        list->root = inserted;
        list->root->next = NULL;
        list->root->prev = NULL;
        list->tail = list->root;
    } else {
        inserted->next = list->root;
        list->root->prev = inserted;
        list->root = inserted;
    }

    list->_size++;
}

// Inserts a Node at the end of the LinkedList.
// O(1) complexity.
void addLast(struct LinkedList *list, char *data) {
    if (list->_size == 0)
        add(list, data);
    else {
        struct Node *added = constructNode(data);
        added->prev = list->tail;
        list->tail->next = added;
        list->tail = added;
        list->_size++;
    }
}

// Clear the LinkedList of Nodes.
// O(1) complexity.
void clear(struct LinkedList *list) {
//    struct Node *curr = list->root;
//    struct Node *next = curr;
//    while (curr != NULL) {
//        char *tempData = curr->data;
//        next = curr->next;
//        free(tempData);
//        free(curr);
//        curr = next;
//    }
//    free(list);
    list->root = NULL;
    list->tail = NULL;
    list->_size = 0;
}

// Remove's the first Node in the LinkedList.
// O(1) complexity.
void removeFirst(struct LinkedList *list) {
    if (list->_size > 0) {
        if (list->_size == 1)
            clear(list);
        else {
//            struct Node *first = list->root;
//            char* tempData = list->root->data;
            list->root = list->root->next;
            list->root->prev = NULL;
            list->_size--;
//            free(tempData);
//            free(first);
        }
    }
}

// Remove's the last Node in the LinkedList.
// O(1) complexity.
//void removeLast(struct LinkedList *list) {
//    if (list->_size > 0) {
//        if (list->_size == 1)
//            clear(list);
//        else {
//            list->tail = list->tail->prev;
//            list->tail->next = NULL;
//            list->_size--;
//        }
//    }
//}

// Inserts a Node at a given index inside the LinkedList.
int addAt(struct LinkedList *list, unsigned int index, char *data) {
    if (index > list->_size) {
        fprintf(stderr, "%s%d%s%zu%c\n",
                "Error: Index Out of Bounds. Index was '",
                index, "\' but size is ", list->_size, '.');
        return 1;
    } else if (index == 0)
        add(list, data);
    else if (index == list->_size)
        addLast(list, data);
    else {
        struct Node *temp = find(list, index),
                *added = constructNode(data);
        added->prev = temp->prev;
        added->next = temp;
        temp->prev->next = added;
        temp->prev = added;
        list->_size++;
    }

    return 0;
}

// Remove's a Node at a given index.
//int removeAt(struct LinkedList *list, unsigned int index) {
//    if (index >= list->_size) {
//        fprintf(stderr, "%s%d%s%d%c\n",
//                "Error: Index Out of Bounds. Index was '",
//                index, "\' but size is ", list->_size, '.');
//        return 1;
//    } else if (index == 0)
//        removeFirst(list);
//    else if (index == list->_size - 1)
//        removeLast(list);
//    else {
//        struct Node *temp = find(list, index);
//        temp->prev->next = temp->next;
//        temp->next->prev = temp->prev;
//        list->_size--;
//    }
//
//    return 0;
//}

// Remove's the first Node whose data matches the parameter, if it exists.
//int removeFirstMatch(struct LinkedList *list, void *data) {
//    struct Node *temp = list->root;
//    while (temp != NULL) {
//        if (temp->data == data) {
//            if (list->_size == 1)
//                clear(list);
//            else if (temp->prev == NULL)
//                removeFirst(list);
//            else if (temp->next == NULL)
//                removeLast(list);
//            else {
//                temp->prev->next = temp->next;
//                temp->next->prev = temp->prev;
//                list->_size--;
//            }
//
//            return 0;
//        }
//        temp = temp->next;
//    }
//
//    return 1;
//}

// Prints out the LinkedList to the terminal window.
// O(n) complexity.
//void print(struct LinkedList *list) {
//    printf("%c", '{');
//
//    struct Node *temp = list->root;
//    while (temp != NULL) {
//        printf(" %s", temp->data);
//        if (temp->next != NULL)
//            printf("%c", ',');
//        temp = temp->next;
//    }
//
//    printf(" }\n");
//}

// Copies a given LinkedList and returns the cloned version.
// O(n) complexity.
//LinkedList *clone(struct LinkedList *list) {
//    if (list == NULL)
//        return NULL;
//
//    struct LinkedList *copy = constructList();
//    struct Node *root = list->root;
//
//    while (root != NULL) {
//        addLast(copy, root->data);
//        root = root->next;
//    }
//
//    return copy;
//}

int searchIndex(int temp[], int length, int value) {
//    if (value < temp[0]) return 0;
//    for (int i = 0; i < length - 1; i++) {
//        if (value > temp[i] && value < temp[i + 1]) return i + 1;
//    }
//    return length;
    if (value == temp[0] || value == temp[length - 1]) return -1;
    if (value < temp[0]) return 0;
    int low = 0;
    int high = length - 1;
    int mid;
    while (low <= high) {
        mid = (high + low) / 2;
        if (value == temp[mid]) {
            return -1;
        } else if (value <= temp[mid]) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return low;
}


int comp(const void *a, const void *b) {
    return *(int *) a - *(int *) b;
}

void printArray(int a[], int length) {
    int i;
    for (i = 0; i < length; i++) {
        if (i == 1 && a[i] == 0) break;
        printf("%d  ", a[i]);
    }

    printf("\n");
}

void reliablyReceive(unsigned short int myUDPport, char *destinationFile) {

    slen = sizeof(si_other);


    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        diep("socket");

    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(myUDPport);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("Now binding\n");
    if (bind(s, (struct sockaddr *) &si_me, sizeof(si_me)) == -1)
        diep("bind");


    /* Now receive data and send acknowledgements */
//    if (remove(destinationFile) == 0)
//        printf("Removed %s.", destinationFile);
//    else
//        perror("remove");
//    FILE *fp = NULL;
    remove(destinationFile);
    FILE *fp = NULL;
    fp = fopen(destinationFile, "wb");
    if (fp == NULL) {
        fprintf(stderr, "FILE NOT OPEN\n");
        return;
    }
    //single message's buf
    char buf[BUFSIZE];
    //whole buffer of frames out of order, the maximum is 300 frames
    LinkedList *SegNotInOrder = constructList();
    //the index of frames out of order which is extracted from messages
    int indexOutOfOrder[300];
    int sizeOutOfOrder[300];
    memset(indexOutOfOrder, 0, 300 * sizeof(int));
    memset(sizeOutOfOrder, 0, 300 * sizeof(int));

    struct sockaddr_in sender_addr; //the addr to send back ack
    socklen_t addrlen = sizeof(sender_addr);
    int lastAckSent = -1;

    int recvbytes = 0;
    while (1) {
        //start receiving message
        recvbytes = recvfrom(s, buf, BUFSIZE, 0, (struct sockaddr *) &sender_addr, &addrlen);
//        printf("%s\n",buf);
        if (recvbytes < 0) {
            fprintf(stderr, "Connection closed\n");
            break;
        }
        //put \0 at the end
        buf[recvbytes] = '\0';
        //compare buf with "EOF", if the first 3 characters are the same, it means the transmission ends
        if (!strncmp(buf, "EOT", 3)) {
            fprintf(stderr, "end of transmission\n");
            break;
        } else {
            char header[HEADER_SIZE]; //put the header in it as "ack n"
            int k;
            char *curr;
            //extract header from buf
            for (curr = buf, k = 0; *curr != ';' && k < BUFSIZE; curr++, k++) {
                header[k] = *curr;
            }

            curr++;     //pointing to first bit of real message
            header[k] = '\0';
            int frameIndex;
            //extract frame index from header
            sscanf(header, "frame%d", &frameIndex);
            //the first situation happens when the received frameIndex equals lastAckSent + 1
            if (frameIndex == lastAckSent + 1) {
//                printf("success getting current one\n");
                //update lastAckSent
                lastAckSent++;
                //write real message into file
//                printf("frameIndex%d\n", frameIndex);
                fwrite(curr, 1, recvbytes - k - 1, fp);
                //send ACK
                char ack[HEADER_SIZE];
                sprintf(ack, "ack%d;", lastAckSent); //+1 stored has been used
//                printf("%s\n", ack);
                sendto(s, ack, strlen(ack), 0, (struct sockaddr *) &sender_addr, sizeof(sender_addr));
                while (indexOutOfOrder[0] == lastAckSent + 1) {
//                    printf("getting one from buffer\n");
                    lastAckSent++;
//                    printf("frameIndex%d\n", indexOutOfOrder[0] % 300);
//                    printf("%d\n", sizeOutOfOrder[indexOutOfOrder[0] % 300]);
                    fwrite(SegNotInOrder->root->data, 1, sizeOutOfOrder[indexOutOfOrder[0] % 300], fp);
                    sizeOutOfOrder[indexOutOfOrder[0] % 300] = 0;
                    removeFirst(SegNotInOrder);
//                    printArray(indexOutOfOrder, 300);
                    for (int k = 1; k < SegNotInOrder->_size + 2; ++k) {
                        indexOutOfOrder[k - 1] = indexOutOfOrder[k];
                    }
//                    printArray(indexOutOfOrder, 300);
                }
            } else if (frameIndex <= lastAckSent) {
//                printf("getting an old ack\n");
                char ack[HEADER_SIZE];
                //resend duplicate ACK
                sprintf(ack, "ack%d;", frameIndex); //frameIndex
//                printf("%s\n", ack);
                sendto(s, ack, strlen(ack), 0, (struct sockaddr *) &sender_addr, sizeof(sender_addr));
            } else {
                //receive out of order segment
//                printf("future new ack stored\n");
                //resend ACK
                char ack[HEADER_SIZE];
                sprintf(ack, "ack%d;", frameIndex);
//                printf("%s\n", ack);
                sendto(s, ack, strlen(ack), 0, (struct sockaddr *) &sender_addr, sizeof(sender_addr));
                if (searchIndex(indexOutOfOrder, SegNotInOrder->_size, frameIndex) == -1) continue;
                //if it is out of order then store it
                //beginOutOfOrder[frameNumber] = sequenceNumber;
//                int i;
                char applicationData[DATA_SIZE];
                memcpy(applicationData, curr, recvbytes - k - 1);
                //put out of order segment into buffer
//                for (i = 0; i < recvbytes - k; ++i) {
//                    applicationData[i] = *curr;
//                    curr++;
//                    //sequenceNumber++;
//                }
//                applicationData[recvbytes - k - 1] = '\0';
                sizeOutOfOrder[frameIndex % 300] = recvbytes - k - 1;
                addAt(SegNotInOrder, searchIndex(indexOutOfOrder, SegNotInOrder->_size, frameIndex), applicationData);
                indexOutOfOrder[SegNotInOrder->_size - 1] = frameIndex;
                qsort(indexOutOfOrder, SegNotInOrder->_size, sizeof(int), comp);
                //endOutOfOrder[frameNumber] = sequenceNumber;
                //frameNumber++;
            }
            memset(buf, 0, BUFSIZE);//buf reads in a packet every time
        }
    }

    close(s);
    fclose(fp);
    printf("%s received.", destinationFile);
    return;
}

/*
 *
 */
int main(int argc, char **argv) {

    unsigned short int udpPort;

    if (argc != 3) {
        fprintf(stderr, "usage: %s UDP_port filename_to_write\n\n", argv[0]);
        exit(1);
    }

    udpPort = (unsigned short int) atoi(argv[1]);

    reliablyReceive(udpPort, argv[2]);
}

