#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
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
char pureBuffer[512];           /*for ez picture shenanigans*/  
struct  sockaddr_in servAddr;   /*Server information*/
char devid = '3';               /*device id assigned by us at compile time*/ 
char replybuffer[512]; 



int regConfirm = 0; 



struct msg {
    char atype;     /*  Either is access or device, ooooor server                         */
    char rtype;     /*  Request type, register, 'do i have stuff?', accessr do something  */
    char data[506]; /*  Remaining data, probably more than we need but w/e                */
    int size;     /*  Should be fixed size but leave this here until we decide together */      
};




//should do RSA here. 
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
        //error hanndle ^^ 
        status = recv(ourSocket, buffer, sizeof(buffer), 0);
        printf("servMsg->data = %s\n", servMsg->data); 
        if (strcmp("OK", servMsg->data) == 0) {
            printf("Server confirmed reg, sending details.\n");
            myMsg.data[0] = devid;
            status = send(ourSocket, (char*)&myMsg, sizeof(struct msg), 0);
            regConfirm = 1; 
            break; 
        }
        else {
            continue; 
        }
    }
}
int main() {
    //set up sockect connection, we know server at compile time. 

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(8042);
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv XINU20 
    servAddr.sin_addr.s_addr = inet_addr("128.10.3.70");
    //i think move this to reg process. 
    //ourSocket = socket(AF_INET, SOCK_STREAM, 0);
    while (1) {
        registerServer(servAddr); 
        //restructure loop and give command to "de register" device from serverside. 
        //this loop should be somewhere else while(reg); 
        printf("Forever polling\n");  
        struct msg sayhi; 
        sayhi.atype = 'D'; 
        sayhi.rtype = 'G';
        sayhi.data[0] = 'h';
        sayhi.data[1] = 'i'; 
        int status; 
        struct msg * myMsg = (struct msg *)buffer; 
        //while (1) { 
        while (regConfirm == 1) {
            sleep(3);
            printf("Sending get request %s\n", (char*)&sayhi); 
            status = send(ourSocket, (char*)&sayhi, sizeof(struct msg), 0);
            printf("Sent %d\n", status); 
            status = recv(ourSocket, buffer, sizeof(buffer), 0); 
            printf("Status is %d\n", status); 
            printf("Recieved %s\n", buffer);  
            if (myMsg->rtype == 'X') {
                regConfirm = 0; 
                break; 
            }
            if (myMsg->rtype == 'U') {
                printf("Recieved update request, performing IO\n"); 
                //perofmr i/o
                //send size of image, and lat/long or w/e 
                //assume image is cat.jpg 
                
                int fd = open("cat.jpg", O_RDONLY);
                struct stat fileStats; 
                fstat(fd, &fileStats); 
                int size = fileStats.st_size; 
                printf("Size is %d\n", size); 
                //send file size. 
                struct msg updatemsg; 
                updatemsg.rtype = 'U'; 
                updatemsg.atype = 'D'; 
                updatemsg.size = size; 
                send(ourSocket, (char*)&updatemsg, sizeof(struct msg), 0); 
                //wait for the okay.
                sleep(3); 
                //recv(ourSocket, buffer, sizeof(buffer), 0); 
                int numSent = 0; 
                int numRead = 0;
                memset(buffer, 0, sizeof(buffer));  
                while (numSent < size) {
                    numRead = read(fd, buffer, sizeof(buffer)); 
                    printf("Read buffer, it is %s\n", buffer); 
                    send(ourSocket, buffer, numRead, 0);
                    numSent += numRead;
                    printf("Num read was %d\n", numRead); 
                }
                printf("Update done\n"); 
                sleep(3); 
            }

        }
    }
}
