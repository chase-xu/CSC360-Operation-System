#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

/*linklist constructer
*Get from lab notes
*/

typedef struct bg{
	pid_t pid;
	char* cmd;
	struct bg* next;
}

bg* first = NULL;
void append(bg* list, pid_t pid, char* cmd){
	bg* curr = (bg*) malloc(sizeof(bg));
	curr->pid = pid;
	curr->cmd = cmd;
	if(list == NULL){
		list = curr;
		list->next = NULL;
	}
	else{
		bg* temp = first; 
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = curr;
		list = 
		
	} 
}


int main(){
	/*Get current path*/
	char buff[1024];
	if(getcwd(buff, sizeof(buff)) != NULL){
		printf("current path is %s\n", buff);
	}else{
		perror("path error");
	}
	
	/*while loop to process command*/
	while(1){
		char buff[1024];
		char hstname[1024]; 
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
		char* cmdf[100];
		char* tok;
		fgets(cmd, sizeof(cmd), stdin);
		printf("string is: %s", cmd);
		printf("%s", cmd);
		strtok(cmd, "\n");

		/*tokenize input*/
		tok = strtok(cmd, " ");
		printf("2\n");
		int i = 0;
		int j = 0;
		while(tok != NULL){
			cmdf[i++] = tok;
			tok = strtok(NULL, " ");
			j++;
		}
		printf("%d\n", i);
		printf("3\n");
		for(i = 0; i < j;i++){
			printf("%s ", cmdf[i]);
		}
		printf("\n");
		printf("%s", cmdf[0]);
		printf("\n");

		/*exit cmd*/
		if(strcmp(cmdf[0], "exit") == 0){
			exit(0);
		}

		/*bg command*/
		if(strcmp(cmdf[0], "bg") == 0){
			pid_t pit = fork();
			/*create a child*/
			if(pit == 0){
				if(execvp(cmdf[1], &cmdf[1]) == -1){
					perror("child execvp() error!");
				}
				else {

				}
				return 0;
			}
		}
		
		/* directory cmd*/
		if(strcmp(cmdf[0], "cd") == 0){ 
			printf("enter cd\n");
			printf("path is %s\n", cmdf[1]);
			if(cmdf[1] == NULL){
				printf("find null \n");
			}
			if(cmdf[1] == NULL || strcmp(cmdf[1],"~") == 0){
				printf("enter path\n");
				char dir[strlen(getenv("HOME"))];
				if(getenv("HOME") != NULL){
					if(chdir(strcpy(dir, getenv("HOME"))) == -1){
						perror("chdir() error!");
					} 
				}
			} else {
				if(chdir(cmdf[1]) == -1){
					perror("chdir() error!");
				} 
			}
			//free(path);
		}
		/*background command*/
		if(strcmp(cmdf[0], "bg") == 0){
			pid_t pid = fork();
						/*in parent*/
			if(pid > 0){ 
				pid_t wt = waitpid(0, NULL, 1);
				if(wt == -1){
					perror("parten wt error!");
				} else{
					printf("child end success!\n");
				}
				continue;
			}
			/*in child*/
			else if(pid == 0){
				if(execvp(cmdf[0], cmdf) == -1){
					perror("child execvp() error!");
				}
				return 0;
			}

		}
	    else {
			pid_t pid = fork();
			/*in parent*/
			if(pid > 0){ 
				pid_t wt = waitpid(0, NULL, 1);
				if(wt == -1){
					perror("parten wt error!");
				} else{
					printf("child end success!\n");
				}
				continue;
			}
			/*in child*/
			else if(pid == 0){
				if(execvp(cmdf[1], &cmdf[1]) == -1){
					perror("child execvp() error!");
				}
				return 0;
			}
		}
	// 	free(cmd);
			 
	// }
	}
	return 0;
}
