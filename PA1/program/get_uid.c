#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
int main() {
	struct passwd* user_pw;
	user_pw = getpwnam("root");

	printf("user_id: %d\n",user_pw->pw_uid);




return 0;
}
