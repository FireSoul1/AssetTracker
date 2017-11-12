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


#define SERVER_PORT 101010
#define devid 0 
#define REGISTER 'R' 
#define DATA 'D' 
#define BUFFER_SIZE 2049

/*
 * GPIO defs go here 
 */

int getTemp();
char * takePicture();


struct p_header {
    char resptype;
    int isDevice;
    int id;
    char name[20];
    int datasize; 
    char dash;
};

