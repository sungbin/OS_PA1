#include <stdio.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS

int select(); // files, protect, hide, status, exit, others(help)
int main() {
	int type;
	char pid[50] = "";
    const char* PROC_PATH = "/proc/mexe";
    while(1) {
        type = select();
        if(type == 5) break;
        switch(type) {
        case 1: {
            printf("1 is executed\n");
            break;
        }
        case 2: {
            printf("pid: ");
            scanf("%s",pid);
            FILE *fp = fopen(PROC_PATH, "w");
            fputs(pid, fp);
            fclose(fp);
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

int select() {
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
