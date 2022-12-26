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
#define BUFSIZ 8192

void sendingFile (char *file, int sock)
{
    char data[BUFSIZ] = {0};
    int bytesSended = send(sock, data, sizeof(data), 0);
    if(bytesSended == -1)
    {
        printf("Error in sending the file\n");
        exit(1);
    }

}

int main()
{
    FILE *file = fopen("bible.txt", "r");
    if(file == NULL)
    {
        printf("Couldn't open the file correctly\n");
        return 0;
    }
    printf("File opened succefully\n");

    fseek(file, SEEK_SET, SEEK_END);
    int size = ftell(file);
    rewind(file);
    int halfFile = size/2;

    if(fclose(file) == EOF)
    {
        printf("Couldn't close the file correctly\n");
        return 0;
    }
    printf("File closed succefully\n");

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        printf("Couldn't create the socket correctly\n");
        return 0;
    }
    printf("Socket created succefully\n");
    
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address)); 
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if(connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
    {
        printf("Couldn't create the connection correctly\n");
        return 0;
    }
    printf("The connection Succeeded!\n");

    char *fp1 = (char*)malloc((halfFile+1)*sizeof(char));
    char *fp2 = (char*)malloc((size-halfFile+1)*sizeof(char));
    memset(fp1, EOF, (halfFile+1)*sizeof(char));
    memset(fp1, EOF, (size-halfFile+1)*sizeof(char));
    fread(fp1, 1, halfFile+1, file);
    fread(fp2, 1, size-halfFile+1, file);

    sendingFile(fp1, sock);
    printf("First part of the file send successfuly\n");
    free(fp1);

    return 0;

}