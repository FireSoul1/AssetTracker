#include <stdio.h>
#include <stdlib.h>



void exec_pycode(const char * code) {
	Py_Initialize();
	PyRun_SimpleString(code);
	Py_Finalize(); 
}

int main() {
	exec_pycode("from picamera import PiCamera\ncamera = PiCamera()\ncamera.capture(\'/home/pi/Desktop/dev3-new.jpg\')"); 
	return 0;


}
