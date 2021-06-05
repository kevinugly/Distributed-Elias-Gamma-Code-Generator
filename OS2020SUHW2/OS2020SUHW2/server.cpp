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

void error(char *msg)
{
    perror(msg);
    exit(1);
}

string decimalToBinary(unsigned long n)
{
    string b;
    for(int i = 0; n > 0; i++)
    {
        b += to_string(n % 2);
        n = n / 2;
    }
    
    for (int i = 0; i < b.length() / 2; i++)
    {
        swap(b[i], b[b.length() - i - 1]);
    }
    return b;
}

string decimalToEliasGamma(unsigned long n)
{
    string binary = decimalToBinary(n);
    string eliasGamma = "";
    for(int x = 0; x < binary.length() - 1; x++)
    {
        eliasGamma += "0";
    }
    string Gamma = eliasGamma += binary;
    return Gamma;
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    char recvBuffer[256];
    string Message[sizeof(buffer)];
    string Symbol[sizeof(buffer)];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if(argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        error((char *)"ERROR opening socket");
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error((char *)"ERROR on binding");
    }
    listen(sockfd, 100);
    clilen = sizeof(cli_addr);
    while((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen)) > 0)
    {
        if(newsockfd < 0)
        {
            error((char *)"ERROR on accept");
        }
        
        bzero(buffer, 256);
        //receives value from client
        recv(newsockfd, recvBuffer, sizeof(recvBuffer), 0);
        
        //value
        string v = recvBuffer;
        //generates binary
        string b = decimalToBinary(stoi(v));
        //generate elias
        string e = decimalToEliasGamma(stoi(v));
        
        char bin[sizeof(b)];
        //stores binary into char array
        strcpy(bin, b.c_str());
        char elias[sizeof(e)];
        //stores elias into char array
        strcpy(elias, e.c_str());
        
        sprintf(buffer,"%s", bin);
        //sends binary back to client
        send(newsockfd, buffer, sizeof(buffer), 0);
        
        sprintf(buffer,"%s", elias);
        //sends elias back to client
        send(newsockfd, buffer, sizeof(buffer), 0);
    }
    close(sockfd);
    return 0;
}
