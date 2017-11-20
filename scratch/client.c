#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

//GPIO Pins go here. 
//Noir camera has python doc but lets do C for now


/**
 * Client needs to interface with I/O  
 * Poll server 
 * Respond to server challenge
 * Send back data to server. 
 */

int ourSocket;                  /*ourSocket is the fd of the socket we use to talk*/
char * server = "128.10.3.70";  /*address of server to connect to*/
char buffer[512];               /*buffer for requests/response*/
struct  sockaddr_in servAddr;   /*Server information*/

struct msg {
    char atype;     /*  Either is access or device, ooooor server                         */
    char rtype;     /*  Request type, register, 'do i have stuff?', accessr do something  */
    //int size;     /*  Should be fixed size but leave this here until we decide together */      
    char data[510]; /*  Remaining data, probably more than we need but w/e                */
};

char * pollServer() {
    int n = recv(ourSocket, buffer, sizeof(buffer) , 0); 
    printf("Buffer is %s\n", buffer); 

}

void * takeAction() {
    if (strcmp(buffer, "kek") == 0) {
        printf("kek it is\n"); 

    }
}

void getCommand() {
    printf("In getComm\n"); 
    char * c = pollServer();

    //parseResponse(buffer, c); 
    //takeAction(buffer); 
}


void answerChallenge(char * ans, char * data) {
    int i; 
    for (i = 0; i < strlen(data); i++) {
        ans[i] = data[i]; 
    }

}
//dev should have dev p_ik 
//challenge sohuld happen here.
//i see bugs in this function, like maybe we should create and return socket id
//because we might call conn way to many times. 
void registerServer() {
    int isReg = 0, status; 
    while (!isReg) {
        ourSocket = socket(AF_INET, SOCK_STREAM, 0);
        status = connect(ourSocket, (struct sockaddr*)&servAddr, sizeof(servAddr)); 

        if (status < 0) {
            printf("Error send\n"); 
            close(ourSocket); 
            sleep(1); 
            continue; 
        }
        struct msg ans; 
        struct msg myMsg; 
        memset(&myMsg, 0, sizeof(myMsg)); 
        memset(&ans, 0, sizeof(ans)); 
        myMsg.atype = 'D'; 
        myMsg.rtype = 'R'; 
        printf("Sending reg request\n");  
        status = send(ourSocket, (char*)&myMsg, sizeof(myMsg), 0); 
        if (status < 0) {
            printf("Error send\n"); 
            close(ourSocket); 
            sleep(1); 
            continue; 
        }
        status = recv(ourSocket, buffer, sizeof(buffer), 0); 
        if (status < 0) {
            printf("Error recv\n"); 
            printf("Buffer was %s\n", buffer); 
            close(ourSocket); 
            sleep(1);
            continue;
        }
        
        struct msg * servMsg = (struct msg *)buffer;
        printf("buffer %s\n", buffer); 
        printf("servMsg->data %s\n", servMsg->data); 
        printf("Recieved challenge %s\n", servMsg->data); 
        answerChallenge(ans.data, servMsg->data);  
        printf("Sending response %s, %d\n", ans.data, sizeof(struct msg)); 
        //status = send(ourSocket, servMsg, sizeof(struct msg), 0); 
        status = send(ourSocket, &ans, sizeof(struct msg), 0); 
        break;  
        //printf("Recv success, buffer is %s\n", buffer); 
        //close(ourSocket); 
        //break; 
    }

}


int main() {
    //set up sockect connection, we know server at compile time. 

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(8042);
    servAddr.sin_addr.s_addr = inet_addr("128.10.3.70");

    //i think move this to reg process. 
    //ourSocket = socket(AF_INET, SOCK_STREAM, 0);

    registerServer(servAddr); 

    //restructure loop and give command to "de register" device from serverside. 
    //this loop should be somewhere else while(reg); 
    while (1) {
        //getCommand(buffer);  
        //sleep(3); 

    }

}

