#include <iostream>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
using namespace std;

#define PORT 8080
#define input 100

int stop = 0;

pthread_mutex_t clientlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clientCond = PTHREAD_COND_INITIALIZER;

void *read(void* fd)
{
    printf("kro na read\n");
    int sockfd = *(int *)fd;
    char msgrd[input];
    while(1)
    {
        memset(msgrd, 0, sizeof(msgrd));
        read(sockfd, msgrd, input);
        printf("%s \n", msgrd);
        if (strcmp(msgrd, "exit") == 0){
            stop = 1;
            pthread_exit(NULL);
        }
        memset(msgrd, 0, sizeof(msgrd));
    }
}

void *write(void* sfd)
{
    printf("kro na write\n");
    int sockfd = *(int *)sfd;
    char msgwt[input];
    while(1)
    {
        memset(msgwt, 0, sizeof(msgwt));
        scanf("%s",msgwt);
        write(sockfd, msgwt, strlen(msgwt));
        if(strcmp(msgwt,"exit")==0)
            pthread_exit(NULL);
        memset(msgwt, 0, sizeof(msgwt));
    }
}


void Bootstrap(int sockfd,char usrname[])
{
    char Buff[30] = "myId:";
    sprintf(Buff,"%s%s",Buff,usrname);
    write(sockfd,Buff,strlen(Buff));
    memset(Buff, 0, sizeof(Buff));
}

int main() {
    const char* ServerIP = "127.0.0.1";
    int sockfd,serverfd;
    struct sockaddr_in myaddr,server_addr;
    socklen_t server_addr_size;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd==-1)
    {
        printf("socket creation failed");
        exit(1);
    }
    else
    {
        printf("socket created successfully \n");
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ServerIP);
    server_addr.sin_port = htons(PORT);
    char usrname[30];
    printf("Enter your username :  \n");
    scanf("%s",usrname);
    pthread_t threadid[2];
    int out = connect(sockfd,(struct sockaddr *) &server_addr,sizeof(server_addr));
    if(out == -1)
    {
        printf("Not connected \n");
        exit(0);
    }
    else
    {
        printf("connected \n");
        Bootstrap(sockfd,usrname);
        pthread_create(&threadid[0],NULL,read,&sockfd);
        pthread_create(&threadid[1],NULL,write,&sockfd);

        pthread_join(threadid[0],NULL);
        pthread_join(threadid[1],NULL);
    }
    return 0;
}