#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
int main() {
	printf("user_id: %d\n",get_u_id_from_username("ubuntu"));
	
return 0;
}
int get_u_id_from_username(char* u_name) {
	struct passwd* user_pw;
	user_pw = getpwnam(u_name);
	return user_pw->pw_uid;
}
