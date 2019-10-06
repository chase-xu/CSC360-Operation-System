#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>



/*constants*/
#define CMDSIZE 25
/*linklist constructer
*Get from lab notes
*/

typedef struct bg{
	struct bg* next;
	pid_t pid;
	char* cmd;
	int num;
} bg;


/*point list to null which is the first node in the linklist*/
bg* list = NULL;
void append(pid_t pid, char** cmdf){
	/*create a node that is going to be appended*/
	bg* curr = (bg*) malloc(sizeof(bg));
	printf("in append\n");
	char* cmdc = (char*) malloc(sizeof(char)*70);
	int i = 0;
	printf("cmd is %s\n", cmdf[0]);
	while(cmdf[i] != NULL){
		strcat(cmdc, cmdf[i]);
		strcat(cmdc, " ");
		i++;
	}
	printf("%s\n", cmdc);
	printf("passed concat\n");
	curr->pid = pid;
	curr->cmd = "";
	strcat(curr->cmd, cmdc);
	printf("cmd is %s\n", curr->cmd);
	/*check if it is the first node*/
	if(list == NULL){
		list = curr;
		curr->num = 1;
		list->next = NULL;
	}
	else{
		bg* temp = list; 
		while(temp->next != NULL){
			temp = temp->next;
		}

		temp->next = curr;
		curr->num = temp->num + 1;
		temp->next->next = NULL;
	} 
}

int isEmpty(){
	if(list == NULL){
		return 1;
	}
	return 0;
}

void printList(){
	printf("in print list\n");
	int n = 0;
	bg* curr = list;
	printf("passed concatenation\n");
	if(list == NULL){
		perror("no bg in the list\n");
	}
	while(curr != NULL){
		n++;
		printf("%d: %s%d\n", curr->pid, curr->cmd, curr->num);
		strcat(curr->cmd, " ");
		curr = curr->next;
	}
	printf("Total Background jobs: %d\n", n);
}

/*Delete node*/
void delete(pid_t pid){
	char* cmd;
	bg* curr = list->next;
	bg* prev = list;
	/*case if the list is empty*/
	if(list == NULL){
		perror("list is empty!");
	}

	/*first one matched*/
	else if(list->pid == pid){
		bg* temp = list;
		while(temp->next != NULL){
			temp->num = temp->num - 1;
			temp = temp->next;
		}
		list = list->next;
		printf("%d: %s has terminated.\n", pid, prev->cmd);
		free(prev->cmd);
		free(prev);
	}
	/*Go through list to find same pid*/
	else{
		while(curr != NULL){
			/*case find the pid*/
			if(curr->pid == pid){
					bg* temp = curr->next;
					int num = curr->num;
					while(temp != NULL){
						temp->num = num;
						temp = temp->next;
						num++;
					}
					prev->next = curr->next;
					printf("%d: %s %d has terminated.\n", pid, curr->cmd, curr->num);
					free(curr->cmd);
					free(curr);
					break;
			}
			/*go check next node*/
			prev = curr;
			curr = curr->next;
		}
	}
}

void check_list(){
	/*check if any child terminated*/
	if(isEmpty() == 0){
		pid_t pid = waitpid(0, NULL, WNOHANG);
		while(pid > 0){
			printf("in pid %d\n", pid);
			delete(pid);			
			pid = waitpid(0, NULL, WNOHANG);
		}
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
        //char* cmd = (char*)malloc(sizeof(char)*CMDSIZE);
		//char argv[CMDSIZE];
		//char** cmdf = (char**) malloc(sizeof(char));
		char cmd[CMDSIZE];
		char* cmdf[CMDSIZE];
		
		printf("string is: %s\n", cmd);
		printf("%s", cmd);

		/*tokenize input*/			 
		char* tok;
		fgets(cmd, sizeof(cmd), stdin);
		strtok(cmd, "\n");
		printf("%s\n", cmd);
		int i = 0;
		int j = 0;
		tok = strtok(cmd, " ");
		while(tok != NULL){
			cmdf[i++] = tok;
			tok = strtok(NULL, " ");
			j++;
		}
		cmdf[1] = strtok(cmdf[1], " ");  //not sure why if the input is cd and + space, after tokenize the space will remains. So I just use this line to delete the space;
		printf("%d\n", i);
		printf("3\n");
		for(i = 0; i < j;i++){
			printf("%s ", cmdf[i]);
		}
		printf("\n");
		printf("%s", cmdf[0]);
		printf("\n");
		//free(cmd);
		/*exit cmd*/
		if(strcmp(cmdf[0], "quit") == 0){
			check_list();
			exit(0);
		}
		/* directory cmd*/
		else if(strcmp(cmdf[0], "cd") == 0){ 
			check_list();
			printf("enter cd\n");
			printf("path is%s grid\n", cmdf[1]);
			if(cmdf[1] == NULL){

				printf("find null \n");
			}
			if(cmdf[1] == NULL|| strcmp(cmdf[1],"~") == 0){
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
		else if(strcmp(cmdf[0], "bg") == 0){
			check_list();

			/*abandon first cmd "bg"*/
			char* cmdbg[CMDSIZE];
			int i = 0;
			printf("size of cmdf is %d\n",(int) sizeof(cmdf));
			while(cmdf[i+1] != NULL){
				cmdbg[i] = cmdf[i+1];
				printf("%s ", cmdbg[i]);
				i++;
			}
			printf("\n");
			printf("passed copy\n");
			pid_t pid = fork();
			/*in parent*/
			if(pid > 0){ 
				append(pid, cmdbg);             
				continue;
			}
			/*in child*/
			else if(pid == 0){
				printf("cmdbg[0] is %s\n", cmdbg[0]);
				if(execvp(cmdbg[0], cmdbg) == -1){
					perror("child execvp() error!");
				}
				return 0;
			}
		}
		/*print bglist*/
		else if(strcmp(cmdf[0], "bglist") == 0){
			printList(cmdf);
		}
	    else {
			check_list();
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
	}
	
	return 0;
}
