
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
#define BUFSIZ 8192

//Function that send the file to the receiver:

int sendall(int socket, char * buf, int *len){
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

// Reading the file:

    FILE *file = fopen("bible.txt", "r"); // Opening the file
    if(file == NULL) // Checking if the file opened proparlly
    {
        printf("Couldn't open the file correctly\n");
        return -1;
    }
    printf("File opened succefully\n");

// Calculating the length of the file:

    fseek(file, SEEK_SET, SEEK_END);
    int size = ftell(file);
    rewind(file);
    int halfFile = size/2;

// Creating the socket:

    int sock = socket(AF_INET, SOCK_STREAM, 0); 
    if(sock == -1) // Checking if the socket opened proparlly
    {
        printf("Couldn't create the socket correctly\n");
        close(sock);
        return -1;
    }
    printf("Socket created succefully\n");

// Creating a TCP Connection between the sender and receiver:
    
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address)); // reset  
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if(connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) // Checking if the connection opened proparlly
    {
        printf("Couldn't create the connection correctly\n");
        close(sock);
        exit(1);
        return -1;
    }
    printf("The connection Succeeded!\n");

//Sending the first part of the file:

Sending:

    char *fp1 = (char*)malloc((halfFile+1)*sizeof(char)); // Creating a memory in the heap
    char *fp2 = (char*)malloc((size-halfFile+1)*sizeof(char)); // Creating a memory in the heap
    memset(fp1, EOF, (halfFile+1)*sizeof(char)); // reset
    memset(fp1, EOF, (size-halfFile+1)*sizeof(char)); // reset
    
    fread(fp1, 1, halfFile+1, file); // Reads data from the given stream
    fread(fp2, 1, size-halfFile+1, file); // Reads data from the given stream

// Sending the first half of the file:

    int p1 = halfFile+1; // First part length
    int *p11 = &p1; // argument for the function sendall()
    if(setsockopt(sock,IPPROTO_TCP,TCP_CONGESTION,"cubic",5)<0){  // Sending the file with "cubic" cc algorithm 
        close(sock);
        exit(1);
        return -1; 
    }  
    if(sendall(sock, fp1, p11) == -1){
        printf("Error in sending the file\n");
        close(sock);
        exit(1);
        return -1;
    }
    printf("First part of the file send successfuly and sent %d bytes\n", *p11);
    free(fp1); // We used the heap therefor we need to free the memory

// Authentication:

    int YUVAL_ID = 8039;
    int MAOR_ID = 5965;
    int YxM = YUVAL_ID^MAOR_ID;
    printf("Waiting for authentication...\n");
    int authenticationReply; 
    int byteRecieved  = recv(sock, &authenticationReply, 12, 0); // Recv the authentication 
    if(byteRecieved == -1){
        printf("recv() failed"); 
        close(sock);
        exit(1);
        return -1; 
    }     
    printf("recieved %d bytes from server :\n", byteRecieved);    
    if(authenticationReply != YxM){ 
        printf("authentication faild\n");
        close(sock);
        exit(1);
        return -1; 
    }
    printf("Authentication succeeded\n");

//Sending the second part of the file:

    if(setsockopt(sock,IPPROTO_TCP,TCP_CONGESTION,"reno",sizeof("reno"))<0){ // Sending the file with "cubic" cc algorithm
        close(sock);
        exit(1);
        return -1; 
    }
    int p2 = size-halfFile+1; // Second part length
    int *p22 = &p2; // argument for the function sendall()
    if(sendall(sock, fp2, p22) == -1){ //Cheaking if the data sent proparlly
        printf("Error in sending the file\n");
        close(sock);
        exit(1);
        return -1;
    }
    printf("Second part of the file send successfuly and sent %d bytes\n", *p22);
    free(fp2); // We used the heap therefor we need to free the memory
    if(fclose(file) != EOF) // Cheaking if its the end of the file
    {
        printf("Couldn't close the file correctly\n");
        close(sock);
        exit(1);
        return -1;
    }
    printf("File closed succefully\n");

//User decision:

TryAgain:

    char q;
    printf("Do you want to send the file againg? \n Type Y for yes and N for no \n");
    scanf("%c", &q);
    printf("\n");
    if(q == 'Y'){ // Cheacking the user decision
        goto Sending;
    }else if(q == 'N'){
        goto exit;
    }else{
        printf("Wrong input please try again\n");
        goto TryAgain;
    }

exit:

    printf("Exiting...\n");
    exit(1); // Exiting the connection
    close(sock); // Closing the socket

    return 0;

}