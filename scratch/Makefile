

all:
	gcc -g -o client client.c -lm -L /usr/lib/python3.4/config-3.4m-arm-linux-gnueabihf/ -lpython3.4
	gcc -g -o cmdctrl cmdctrl.c -lpthread
sim: 
	gcc -g -o simclient simclient.c 
	gcc -g -o simctrl simctrl.c -lpthread

server:
	gcc -g -o cmdctrl cmdctrl.c -lpthread

client: 
	gcc -g -o client client.c -lm -L /usr/lib/python3.4/config-3.4m-arm-linux-gnueabihf/ -lpython3.4



