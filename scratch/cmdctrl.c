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
#include <fcntl.h>           
#include <sys/stat.h>       
#include <semaphore.h>

#define WAIT_COUNT 20
#define REG 'R' 
#define CHAL 'C' 
#define ACC 'A' 
#define DEV 'D' 
#define SERV 'S' 
#define GET 'G' 
#define OURSEM "/goteam" 

int masterSocket = -1, clientSocket = -1, status = -1; 
struct sockaddr_in ourInfo; 
char buffer[512]; 
int numConn = 0;
int devid = -1; 
int locked = 1; 

sem_t * sem = NULL; 

int shares[256]; 



int debugPic = 1; 
/**
 *
 * Key for client company here 
 * Key for our comany here 
 *
 *
 */
/**
 *
 * we should cache some info here in memory. 
 * 
 *
 */
// i wana map more into shm, like recently reg procs.  
//
//
//we can always just make this bigger. 
struct msg {
	char atype;     /*  Either is access or device, ooooor server                         */ 
	char rtype;     /*  Request type, register, 'do i have stuff?', accessr do something  */ 
	char data[506]; /*  Remaining data, probably more than we need but w/e                */ 
	int size;     /*  Should be fixed size but leave this here until we decide together */      
};


void setUp() {
	memset(buffer, 0, sizeof(buffer)); 
    memset(shares, 0, sizeof(shares)); 
    memset(&ourInfo, 0, sizeof(ourInfo));
	ourInfo.sin_family = AF_INET;
	ourInfo.sin_addr.s_addr = htons(INADDR_ANY);
	ourInfo.sin_port = htons(8042);

	sem = sem_open(OURSEM, O_CREAT, 0644, 1); 
     
	status = masterSocket = socket(PF_INET, SOCK_STREAM, 0);
	int setSockopt = 1;    
	status = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, &setSockopt, sizeof(int));

	status = bind(masterSocket, (struct sockaddr *)&ourInfo, sizeof(ourInfo));

	status = listen(masterSocket, WAIT_COUNT); 


}

// key signing here 
void generateChallenge(char * data, char * ans) {
	char * echo = "Echo!!!!\0"; 
	int i;
	for (i = 0; i < strlen(echo); i++) {
		data[i] = echo[i]; 
		ans[i] = echo[i];  
	}


}
void activateDevice(struct msg * myMsg) {
    char c = myMsg->data[0];
    devid = c - '0';  
    printf("Activing device %d\n", devid); 
    locked = 0; 
    //send to firebase
    //TODO  pushDB

}

int challengeDevice() {
	printf("In challenge\n");
	struct msg myMsg; 
	char ans[504]; 
	memset(&myMsg, 0, sizeof(myMsg));
	memset(ans, 0, sizeof(ans)); 
	myMsg.atype = SERV; 
	myMsg.rtype = CHAL; 
	generateChallenge(myMsg.data, ans);
	printf("Sending challenge %s\n", myMsg.data); 
	status = send(clientSocket, (char*)&myMsg, sizeof(myMsg), 0);
	printf("Challenge sent\n");
	//int numRead = 0; 
	//while(numRead < 512) {
	status = recv(clientSocket, buffer, sizeof(buffer) , 0);
		//if (numRead < 0) {
		//	printf("Error in numRead\n"); 
		//}
	//}
	struct msg * response = (struct msg*)buffer;
	printf("answer recieved ans is %s\n", response->data);
	int isReg = strcmp(ans, response->data) == 0; 
	printf("strcmp(%s, %s) == %d", ans, response->data, strcmp(ans, response->data)); 
	//maybe another msg sending saying okay

    //then recv details of device. 
    if (isReg) {
        strcpy(myMsg.data, "OK\0"); 
        send(clientSocket, (char*)&myMsg, sizeof(myMsg), 0); 
        recv(clientSocket, buffer, sizeof(buffer) , 0);
        printf("Sent ok, recieved reply %s\n", buffer); 
        activateDevice((struct msg *)buffer); 
    }
    else {
        printf("Dont think we should b here rn\n"); 
        strcpy(myMsg.data, "NO\0"); 
        send(clientSocket, (char*)&myMsg, sizeof(myMsg), 0); 
        close(clientSocket); 
        exit(1); 
    }
	numConn += isReg;  
	return isReg; 

}

struct msg * parseDB() {
    if (devid == -1) {
        printf("Woah we messed up\n"); 
    }
    struct msg * temp = (struct msg*)malloc(sizeof(struct msg));
    temp->atype = SERV;
    //should have other headers here rn. 
    temp->rtype = CHAL;
    //TODO poll DB here.
    //get shares here
    //myMsg->data = getDB()
    //for now just say NA. and repeat poll.
    strcpy(temp->data, "WAIT\0"); 
    return temp; 
}

//only devices for now.
//should be a better way, probably modify protocol a bit
//FLAG 

void handleConnection() {
	status = recv(clientSocket, buffer, sizeof(buffer), 0);
	printf("Buffer is %s\n", buffer);
	struct msg * myMsg = (struct msg *)buffer;
	if (myMsg->rtype == REG) {
		printf("Reg attempt, numConn = %d\n", numConn); 
		int isReg = challengeDevice();
		printf("Reg finished, numConn = %d\n", numConn); 	
    }
    printf("Beginning poll session\n"); 
   /*
    struct msg tmp; 
   tmp.atype = SERV; 
   tmp.rtype = CHAL; 
   strcpy(tmp.data, "WAIT\0"); 
   */
	while (1) {
        recv(clientSocket, buffer, sizeof(buffer), 0);
        printf("Recieved %s\n", buffer); 
        //have different funcs for what u recv ofc, just simple for now. ask/wait
        struct msg * response = parseDB(); 
        printf("Sending %s\n", response->data); 
        send(clientSocket, (char*)response, sizeof(struct msg), 0);
        //printf("Sending %s\n", tmp.data); 
        //send(clientSocket, (char*)&tmp, sizeof(struct msg), 0); 
	free(response);  

    } 
}

void start() {
	while (1) {
		printf("Is this\n");
		clientSocket = accept(masterSocket, (struct sockaddr*) NULL, NULL); 
		printf("Blocking?\n"); 
		int retval = fork(); 
		if (retval == 0) {
			sem = sem_open(OURSEM, 0); 
            handleConnection(); 
		}
	}
}

int main() {
	//set sigaction handler here 
	//maybe dup fd's to make sure we close them okay 
	//we might not need to even deal with zombies if they clean up after themselves  
	int original_stdin, original_stdout, original_stderr; 


	setUp(); 
	start();
	printf("ending i guess\n"); 
}

