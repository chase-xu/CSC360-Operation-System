#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include  <sys/types.h>
#include <sys/wait.h>

int main(){

	char buff[1024];
	if(getcwd(buff, sizeof(buff)) != NULL){
		printf("current path is %s\n", buff);
	}else{
		perror("path error");
	}
	
	while(1){
		char buff[1024];
		//char usenam[1024];
		char hstname[1024]; 
		//usenam = getlogin();
		if(getlogin() == NULL){
			perror("getlogin() error!");
			continue;
		}
		int hst = gethostname(hstname, sizeof(hstname));
		if(hst == -1){
			perror("gethostname() erro!");
			continue;
		}
	
       	 	if(getcwd(buff, sizeof(buff)) != NULL){
                	printf("SSI: %s@%s: %s > ", getlogin(), hstname, buff);

		}else{
                	perror("path error");
			continue;
       	 	}

		/*get command input*/
        char cmd[100];
		fgets(cmd, sizeof(cmd), stdin);
		printf("string is: %s", cmd);
		strtok(cmd, "\n");
        char* tok = strtok(cmd, " ");
		char* cmdf[10];
		int i = 0;
		while( tok != NULL){
			cmdf[i++] = tok;
			tok = strtok(NULL, " ");
		}
		for(i = 0; i <3;i++){
			printf("%s ", cmdf[i]);
		}
		printf("\n");
		if(cmdf[0] == "exit"){
			exit(0);
		} else {
			pid_t pid = fork();
			/*in child*/
			if(pid == 0){
				if(execvp(cmdf[0], cmdf) == -1){
					perror("child execvp() error!");
				}
			} else if(pid > 0){ /*in parent*/
				pid_t wt = waitpid(0, NULL, 1);
				if(wt == -1){
					perror("parten wt error!");
				} else{
					printf("child end success!\n");
				}
			}
		}
		break;
			 
	}
	return 0;
}
