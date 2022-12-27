//Authors: Yuval Musseri & Maor Berenstein:

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

//Function that send the file to the receiver:

long sendall(int socket, char * buf, long *len){
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(socket, buf + total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

int main()
{

//Level 1 - Reading the file:

    char authentication[BUFSIZ] = {'\0'};

    FILE *fp = fopen("bible.txt", "r"); // reading the file with the flag "r"
    if (fp == NULL) // cheacking if the file opened correctly
    {
        printf("cannot open file, error number: %d\n", errno);
        exit(1);
    }
    printf("file opened succefully\n");

    fseek(fp, SEEK_SET, SEEK_END); // seeking the start of the file
    long fileSize = ftell(fp); // the length of the file
    rewind(fp); // returns the pointer of the file to the begining fo the file

    long p1 = fileSize/2; // First part length
    long *pp1 = &p1;
    char fp1[p1];
    fread(fp1, 1, p1+1, fp); // inserting the first half into the array
    long p2 = fileSize - p1; // second part length
    long *pp2 = &p2;
    char fp2[p2];
    fread(fp2, 1, p2+1, fp);// inserting the second half into the array

    fclose(fp); // closing the file

    int sock = socket(AF_INET, SOCK_STREAM, 0); //creating the socket
    if (sock == -1) // Checking if the socket opened proparlly
    {
        printf("Couldn't create the socket correctly, error number: %d\n", errno);
        close(sock);
        exit(1);
    }
    printf("Socket created succefully\n");

    
//Level 2 - Creating a TCP Connection between the sender and receiver:
    
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address)); // reset
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_port = htons(PORT); //translates an integer from host byte order to network byte order
    server_address.sin_addr.s_addr = INADDR_ANY; //here we set the received ip address to be any ip we get

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) // cheaking if we connected proparlly
    {
        printf("Couldn't create the connection correctly, error number: %d\n", errno);
        close(sock);
        exit(1);
    }
    printf("The connection Succeeded!\n");

//Level 3 - Sending the first part of the file:

Sending:

    if(sendall(sock, fp1, pp1) == -1){
        printf("Error in sending the file\n");
        close(sock);
        exit(1);
        return -1;
    }
    printf("First part of the file send successfuly and sent %ld bytes\n", *pp1);

//Level 4 - Checking for authentication

    recv(sock, authentication, BUFSIZ, 0);
    if(strcmp(authentication, "-1") == 0){
        printf("Error in function recv()\n");
        close(sock);
        close(1);
    }

    if(strcmp(authentication, YxM) != 0) //cheaking if the receiver sent us the authentication correctly
    {
        printf("not match!!!!!\n");
        close(sock);
        close(1);

    }
    printf("message: %s \n", authentication);

//Level 5 - Changing the CC Algorithm:

    char *algo = "reno";
    setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, &algo, sizeof(algo)); // change CC
    printf("Changed CC algorithm to reno\n");

//Level 6 - Sending the second part:

    if(sendall(sock, fp2, pp2) == -1){
        printf("Error in sending the file\n");
        close(sock);
        exit(1);
        return -1;
    }
    printf("Second part of the file send successfuly and sent %ld bytes\n", *pp2);

//Level 7 - User decision:
 
    while(1){
        printf("Do you want to send the file againg? \n Type Y for yes and N for exit \n");
tryAgain:
        char decision ='0';
        scanf("%c", &decision);
        if(decision == 'N'){
            send(sock, &decision, strlen(&decision) + 1, 0);
            goto exit;
        }
        else if(decision == 'Y'){
            send(sock, &decision, strlen(&decision) + 1, 0);
            goto Sending;
        }else{goto tryAgain;}
    }

exit:

    printf("Exiting...\n");
    exit(1); // Exiting the connection
    close(sock); // Closing the socket

    return 0;

}