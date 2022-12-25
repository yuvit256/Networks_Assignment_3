//CLIENT

#include <stdio.h> 
#include <stdlib.h> 
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>

#define PORT 9696
#define IP "127.0.0.1"

int main()
{
    FILE *file = fopen("f.txt", "r");
    if(file == NULL)
    {
        return 0;
    }
    fclose(file);


    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock <= 0)
    {
        return 0;
    }
    
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address)); 
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if(connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
    {
        return 0;
    }
    printf("connected!");

    char *message = "Its Maor Berenstein!";
    if(send(sock, message, strlen(message) + 1, 0) <= 0)
    {
        printf("err");
        return 0;
    }

    return 0;

}