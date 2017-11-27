#include <stdio.h> 


struct msg {
    char atype; 
    char rtype; 
    char data[506]; 
    int size; 
}; 


int main() {
    printf("%d\n", sizeof(struct msg)); 

}
