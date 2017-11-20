/*
 * Ian, Guna n Carson 
 */

#include "tracker.h" 
#define TCP_00 49908

char * serverLocation = "128.10.3.70";
char responseBuffer[BUFFER_SIZE]; //increase size if we need it  
int writeNum;
int master_fd;
struct  sockaddr_in servAddr;


int getTemp() {

    return 42; 
}

char * takePicture() {

    return NULL; 
}


/**
 * This is our main loop for the device
 * We set up some initial variables like where the server is and our master socket
 * We use DNS to get the actual IP, then we send a registration request to the server
 *
 */
int main() {
    //MAYBE MAKE THESE GLOBAL??? 
    //create socket. 




    bzero(responseBuffer, sizeof(responseBuffer)); 

    //create internet socket with tcp connection 
    master_fd = socket(AF_INET, SOCK_STREAM, 0); 


    //Create socket information, Internet, Port, IP. 
    bzero((char *) &servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET; 
    servAddr.sin_port = htons(TCP_00);
    //change back to serverLocation, rn this is xinu00
    servAddr.sin_addr.s_addr = inet_addr("128.10.3.50");

    //Connect using our actual socket, master_fd, given the socket information, servAddr. 
    if (connect(master_fd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        printf("RIP no connection\n"); 
        exit(1);
    }

    struct p_header * header = (struct p_header *)responseBuffer; 
    header->resptype = REGISTER;
    header->isDevice = 1;
    header->dash = '-'; //so we know end of header. 
    memcpy(header->name, "Guna#1", strlen("Guna#1")); 

    //writeNum = write(master_fd, responseBuffer, sizeof(p_header));
    writeNum = write(master_fd, "heybby", strlen("heybby")); 
    int numRead = 0; 
    while (1) {
        //wait for cmd
      //  printf("reading...\n"); 
        printf("reading?\n"); 
        numRead = accept(master_fd, responseBuffer, BUFFER_SIZE);
        //printf("stuff %s\n", responseBuffer);  
        //make thread 
        
        //int cmd = parseCommand(responseBuffer); 
        
        
        //int retval = fork(); 
        //if (retval
        //read 
        //fork(); 
        //if not child loop back up 
        //

    }
    /*struct  protoent * proto;
      proto = getprotobyname("tcp");
      struct  hostent * hostinfo;*/ 


}
