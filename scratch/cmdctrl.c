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

#define WAIT_COUNT 20
#define REG 'R' 
#define CHAL 'C' 
#define ACC 'A' 
#define DEV 'D' 
#define SERV 'S' 

int masterSocket = -1, clientSocket = -1, status = -1; 
struct sockaddr_in ourInfo; 
char buffer[512]; 
int numConn = 0; 

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

//we can always just make this bigger. 
struct msg {
	char atype;     /*  Either is access or device, ooooor server                         */ 
	char rtype;     /*  Request type, register, 'do i have stuff?', accessr do something  */ 
	//int size;     /*  Should be fixed size but leave this here until we decide together */      
	char data[510]; /*  Remaining data, probably more than we need but w/e                */ 
};


void setUp() {
	memset(&ourInfo, 0, sizeof(ourInfo));
	ourInfo.sin_family = AF_INET;
	ourInfo.sin_addr.s_addr = htons(INADDR_ANY);
	ourInfo.sin_port = htons(8042);

	//map unnamed sem here 

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

	numConn += isReg;  
	return isReg; 

}
//only devices for now. 
void handleConnection() {
	status = recv(clientSocket, buffer, sizeof(buffer), 0); 
	printf("Buffer is %s\n", buffer);
	struct msg * myMsg = (struct msg *)buffer;
	if (myMsg->rtype == REG) {
		printf("Reg attempt, numConn = %d\n", numConn); 
		int isReg = challengeDevice();
		printf("Reg finished, numConn = %d\n", numConn); 
		if (isReg == 0) {
			//maybe send msg saying failure 
			//kill conn
			//close(ourSocket);
			close(clientSocket); 
			exit(1); 
			//exit 
		}
		//temp
		printf("Reg success!\n"); 
		close(clientSocket);
		exit(1);
		//if actually registered place in db. and go on. 
	}

	//challengeDevice();  
}

void start() {
	while (1) {
		printf("Is this\n");
		clientSocket = accept(masterSocket, (struct sockaddr*) NULL, NULL); 
		printf("Blocking?\n"); 
		int retval = fork(); 
		if (retval == 0) {
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

