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
#define FILE_SIZE 1356519
#define YxM "0000100000101010"

int main()
{
    FILE *fp = fopen("bible.txt", "r");
    if (fp == NULL)
    {
        printf("cannot open file, error number: %d\n", errno);
        exit(1);
    }
    printf("file opened succefully\n");

    fseek(fp, SEEK_SET, SEEK_END);
    long fileSize = ftell(fp);
    rewind(fp);
    fclose(fp);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) // Checking if the socket opened proparlly
    {
        printf("Couldn't create the socket correctly, error number: %d\n", errno);
        close(sock);
        exit(1);
    }
    printf("Socket created succefully\n");

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        printf("Couldn't create the connection correctly, error number: %d\n", errno);
        close(sock);
        exit(1);
    }
    printf("The connection Succeeded!\n");

    send(sock, "hello!", strlen("hello!") + 1, 0);

    char buffer[BUFSIZ] = {'\0'};
    recv(sock, buffer, BUFSIZ, 0);
    if(strcmp(buffer, YxM) != 0)
    {
        printf("not match!!!!!\n");
        close(sock);
        close(1);

    }

    send(sock, "goodbye!", strlen("goodbye!") + 1, 0);

    printf("message: %s\n", buffer);

    return 0;
}