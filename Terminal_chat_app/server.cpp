#include <iostream>
#include<bits/stdc++.h>
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

#define Backlog 10
#define PORT 8080
#define input 100

unordered_map<string, int> client_fd;
unordered_map<string, vector <int> > groups;

void addGrp(string groupname,int clientfd)
{	
	// cout<<groupname<<" "<<clientfd<<"\n";
    groups[groupname].push_back(clientfd);
	return;
}

void addUsrToGrp(string username,string groupname)
{
	int clientfd = client_fd[username];
	// cout<<clientfd<<"\n";
	groups[groupname].push_back(clientfd);
	return;
}

void groupmsg(string groupname,char message[])
{
    int n = groups[groupname].size();
	cout<<n<<"\n";
	for (int i = 0; i < n; i++)
	{
		int clientfd = groups[groupname][i];
		// cout<<clientfd<<"\n";
		write(clientfd,message,strlen(message));
		// cout<<message<<"\n";
	}
	return;
    // memset(message,0,sizeof(message));
}

void usermsg(string username, char message[])
{
     int clientfd = client_fd[username];
	 write(clientfd, message, strlen(message));
	 //memset(message, 0, sizeof(message));
	 return;
}

void setusername(string username,int clientfd)
{
    // cout<<username<<" "<<clientfd<<"\n";
    client_fd[username] = clientfd;
	return;
}

void Deliver_Message(char request[],int clientfd)
{
	char type[30],message[100];
	string header,msg;
    char *p;
	p = strtok(request,":");
	strcpy(type,p);
	p = strtok(NULL, ":");
	strcpy(message, p);
	// cout<<type<<" "<<message<<"\n";
	for(int i=0;i<strlen(type);i++)
	{
        header += type[i];
	}
	for (int i=0;i<strlen(message);i++)
	{
		msg += message[i];
	}
    // cout<<header<<" "<<msg<<"\n";
	if(header=="addGrp")
	{
		// printf("a\n");
        addGrp(msg,clientfd);
	}
	else if(header=="addUsrToGrp")
	{
		// printf("b\n");
		string username,groupname;
		int found = msg.find_first_of(",");
		username = msg.substr(0,found);
		groupname = msg.substr(found+1,msg.length());
		// cout<<username<<" "<<groupname<<"\n";
		addUsrToGrp(username,groupname);
	}
	else if(header=="myId")
	{
		// printf("c\n");
		setusername(msg,clientfd);
	}
	else
	{
		if(groups[header].size()>0)
		{
			// printf("d\n");
			groupmsg(header, message);
		}
		else
		{
			// printf("e\n");
			usermsg(header,message);
		}
	}
	header.clear();
	msg.clear();
	return;
}

void *chatting(void* cfd)
{
	printf("chatting start\n");
	int clientfd =  *(int *)cfd;
	char request[30];
	 while(1){
		memset(request,0,sizeof(request));
		read(clientfd,request,input);
		printf("%s \n",request);
		if(strcmp(request,"exit")==0)
		{
			write(clientfd,request,strlen(request));
			memset(request, 0, sizeof(request));
			return cfd;
		}
		Deliver_Message(request,clientfd);
		memset(request,0,sizeof(request));
	 }
}

int main(int argc, char const *argv[])
{
	int sockfd,fd;
	struct sockaddr_in my_addr,client_addr;
	socklen_t client_addr_size;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1)
	{
		printf("socket creation failed...\n"); 
		exit(0);
	}
	memset(&my_addr,0,sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(PORT);
	int out = bind(sockfd,(struct sockaddr *) &my_addr,sizeof(struct sockaddr_in));
	if(out==-1)
	{
		printf("socket bind failed...\n"); 
		exit(0);
	}
	else
	{
		printf("socket bind successfully\n");
	}
	int l = listen(sockfd,Backlog);
	if(l==-1)
	{
		printf("socket listen failed...\n"); 
		exit(0);
	}
	if(l==0)
	{
		printf("server listening\n");
	}
	pthread_t threadid;
	while(1)
	{
		fd = accept(sockfd,(struct sockaddr *) &client_addr,&client_addr_size);
		// printf("%d",fd);
		if(fd==-1)
		{
			printf("socket accept failed\n");
		}
		else{
			// printf("new client added\n");
			pthread_create(&threadid,NULL,chatting,&fd);
		}
    }
	return 0;
}