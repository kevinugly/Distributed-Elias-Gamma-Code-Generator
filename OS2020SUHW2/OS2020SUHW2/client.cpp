#include <iostream>
#include <fstream>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
using namespace std;

int g_argc;
char **g_argv;

void error(char *message)
{
    perror(message);
    exit(0);
}

struct Code
{
    string value;
    string binary;
    string eliasGamma;
    int index;
};

struct Code test[100];

void *clientThread(void *arg)
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    char recvBuffer[256];
    struct Code *pkt = (struct Code *)arg;
    int index = pkt->index;
    
    if (g_argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", g_argv[0]);
        exit(0);
    }
    
    portno = atoi(g_argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(sockfd < 0)
    {
        error((char *)"ERROR opening socket");
    }
    
    server = gethostbyname(g_argv[1]);
    
    if(server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error((char *)"ERROR connecting");
    }
    
    bzero(buffer, 256);
    memset(buffer, '\0', strlen(buffer));
    sprintf(buffer,"%s", (pkt->value).c_str());
    test[index].value = pkt->value;
    
    //sending value
    send(sockfd, buffer, sizeof(buffer), 0);
    
    //receiving binary
    recv(sockfd, recvBuffer, sizeof(recvBuffer), 0);
    
    //setting binary
    test[index].binary = recvBuffer;
    
    //receiving eliasGamma
    recv(sockfd, recvBuffer, sizeof(recvBuffer), 0);
    
    // //setting eliasGamma
    test[index].eliasGamma = recvBuffer;
    
    //closes socket
    close(sockfd);
    return NULL;
}

int main(int argc, char *argv[])
{
    g_argc = argc;
    g_argv = argv;
    vector<string> values;
    string line;
    int length = 0;
    while(getline(cin, line))
    {
        if(length == 0)
        {
            length = stoi(line);
        }
        else
        {
            values.push_back(line);
        }
    }
    
#define NTHREADS length
    pthread_t tid[NTHREADS];
    
    for(int x = 0; x < NTHREADS; x++)
    {
        test[x].index = x;
        test[x].value = values[x];
        
        if(pthread_create(&tid[x], NULL, clientThread, &test[x]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }
    
    for(int x = 0; x < NTHREADS; x++)
    {
        pthread_join(tid[x], NULL);
    }
    
    for(int x = 0; x < NTHREADS; x++)
    {
        cout << "Value: " << test[x].value << ", Binary Code: " << test[x].binary << ", Elias-Gamma code: " << test[x].eliasGamma << endl;
    }
    
    return 0;
}
