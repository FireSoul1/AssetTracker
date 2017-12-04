#include <stdio.h>
#include <stdlib.h>

struct msg {
    char atype;     /*  Either is access or device, ooooor server                         */
    char rtype;     /*  Request type, register, 'do i have stuff?', accessr do something  */
    int size;     /*  Should be fixed size but leave this here until we decide together */      
    char data[502]; /*  Remaining data, probably more than we need but w/e                */
};
   
int main() {
    printf("%d\n", sizeof(struct msg)); 

}
