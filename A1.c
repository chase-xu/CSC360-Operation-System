#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
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
		printf("1\n");
        char* tok = strtok(cmd, " ");
		printf("2\n");
		char* cmdf[100];
		int i = 0;
		int j = 0;
		while( tok != NULL){
			cmdf[i++] = tok;
			tok = strtok(NULL, " ");
			j++;
		}
		printf("3\n");
		for(i = 0; i < j;i++){
			printf("%s ", cmdf[i]);
		}
		printf("\n");
		printf("%s", cmdf[0]);
		printf("\n");

		printf("%s", typeof(cmdf[0]));	
                printf(*cmdf[0]);
		/*exit cmd*/
		if(strcmp(*cmdf[0], "exit") == 0){  
			printf("enter exit\n");
			exit(0);
		} 
		if(strcmp(*cmdf[0],"cd") == 0){ /* directory cmd*/
			printf("enter cd");
			char* path = cmdf[1];
			if(strcmp(*cmdf[1],"~") == 0 || *cmdf[1] == NULL){
				if(getenv("HOME") != NULL){
					if(chdir(path) == -1){
						perror("chdir() error!");
					} 
				}// else{
				// 	char* dir = getpwuid(getuid())
				// }	
			} else {
				if(chdir(path) == -1){
					perror("chdir() error!");
				} 
			}
		} else {
			pid_t pid = fork();
			/*in parent*/
			if(pid > 0){ 
				pid_t wt = waitpid(0, NULL, 1);
				if(wt == -1){
					perror("parten wt error!");
				} else{
					printf("child end success!\n");
				}
			}
			/*in child*/
			else if(pid == 0){
				if(execvp(cmdf[0], cmdf) == -1){
					perror("child execvp() error!");
				}
				return 0;
			}
		}
			 
	}
	return 0;
}
