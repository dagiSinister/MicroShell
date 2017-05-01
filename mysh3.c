#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024

int splitInput(char*,char**);
int main(int argc, const char * argv[]){
	
	char input[80];
	char* params[5];
	char* paramPrePipe[5];
	char* paramPostPipe[5];
	int p[2];
	int piped = 0;
	int count_arg;
	
	
	do{	
		printf(">");

			//Start reading input
		if(fgets(input, sizeof(input),stdin) == NULL) break;
		
			//read end of input		
		if(input[strlen(input)-1]=='\n'){
			input[strlen(input)-1] = '\0';
		}
			//read "exit" from keyboard to quit
		if(strcmp(input, "exit")==0){
			char space = " ";
			int i;
			printf("Exiting MyShell");
			for(i = 0; i < 4; i++){	
				usleep(10000);			
				space +=" ";				
				printf(space +"...");
				
			}
			return 0;
		}
		
		count_arg = splitInput(input, params);
		int i;
		//looks for the token "|"

		for(i = 0; i< count_arg; i++){
			if(strcmp(params[i],"|") ==0){
				int x;
				for(x = 0; x<i; x++){
					paramPrePipe[x] = params[x];	
				}			
				paramPrePipe[x] = NULL;
				
				for(x = 0; x<5-i-1; x++){
					paramPostPipe[x] = params[x+1+i];	
				}			
				paramPostPipe[5 - i-1] = NULL;
				if(pipe(p)<0){
					perror("Error in creating piped process");
					exit(1);
				}
		
			piped = 1;
			break;
				
			}
		}
		
		pid_t pid;
		pid = fork();

		//PROCESS HANDLING IN EACH PIPE CREATED
		if(pid < 0){
			perror("Error in creating process");
			exit(1);
		}

		else if(pid==0){
			if(piped){
				close(p[0]);
				//checking if the first piped process is created correctly
				if(dup2(p[1], STDOUT_FILENO)<0){
					perror("piped process failed!");
					exit(1);				
				}
				execvp(paramPrePipe[0], paramPrePipe);
				printf("UNKOWN INPUT");
			}
			else{
				execvp(input, params);
				printf("UNKOWN INPUT");
			}
			return 0;
		}	
		else{
			if(piped){
				close(p[1]);
			}
			//checking if the second piped process is created correctly
			wait(NULL);
			if(piped){
				piped = 0;
				pid = fork();
				if(pid<0){
					perror("Post piped input failed!");
					exit(1);				
				}
				else if(pid==0){
					close(p[1]);
					if(dup2(p[0],STDIN_FILENO)< 0){
						perror("UNKOWN INPUT");	
						exit(1);				
					}		
					execvp(paramPostPipe[0],paramPostPipe);
					printf("Unkown  input!");
					return 0;		
				}		
				else {
					wait(NULL);	
				}	
			}
		}
	}while(1);
	
	return 0;
}

int splitInput(char *input, char **params){
	int i;
	for(i = 0; i < 5; i++){
		//tokenize input and look for a start of an additional param
		params[i]=strsep(&input," ");
		
		//check if there is any parameter, if none, break and perform main command
		if(sizeof(params) ==0) break;
	}
	return i;

}
