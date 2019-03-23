#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS

int mselect(); // files, protect, hide, status, exit, others(help)
int main() {
	int type;
	char pid[50] = "";
    const char* PROC_PATH = "/proc/mexe";
//      const char* PROC_PATH = "/Users/imseongbin/OneDrive - handong.edu/19-1/OS/OS_PA1/PA1/program/example";
    const char* command_echo1 = "exec echo ";
    const char* command_echo2 = " > /proc/mexe";
    while(1) {
        type = mselect();
        char command[70] = "";
        if(type == 5) break;
        switch(type) {
        case 1: {
            printf("1 is executed\n");
            break;
        }
        case 2: {
            printf("pid: ");
            scanf("%s",pid);
            strcat(command,command_echo1);
            strcat(command,pid);
            strcat(command,command_echo2);
	    printf("%s\n",command);

            system(command); //execute echo command
            break;
        }
        case 3: {
            
            break;
        }
        case 4: {
            printf("current protected pid: %s\n",pid);
            break;
        }
        default: {
        
            break;
        } }
    }
	
	return 0;
}

int mselect() {
	printf("Enter: ");
	int type;
	char in[128] = "";
	scanf("%s",in);
	if((strcmp(in,"fils")) == 0)
		return 1;
	else if((strcmp(in,"protect")) == 0)
		return 2;
	else if((strcmp(in,"hide")) == 0)
		return 3;
	else if((strcmp(in,"status")) == 0)
		return 4;
	else if((strcmp(in,"exit")) == 0)
		return 5;
	else
		return -1;
	return -1;
}
