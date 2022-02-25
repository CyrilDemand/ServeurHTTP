#include <stdio.h>
#include "main.h"
#include "socket.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>
#include <assert.h>

#define TAILLE_MAX 8192

int process_client(FILE* file_client){
	fprintf(file_client,"Bienvenue chez Zyzz !\n");
	fflush(file_client);
	return 0;
}

int main(){
	int socket_serveur=creer_serveur(8080);
	if (socket_serveur==-1){
		perror("serveur");
		return EXIT_FAILURE;
	}

	initialiser_signaux();

	int socket_client;
	
	

	while (1){

			
		socket_client = accept(socket_serveur, NULL, NULL);
		if (socket_client == -1){
			perror("accept");
		}
		FILE* file_client=fdopen(socket_client,"a+");

		int pid=fork();
		if (pid==0) {
			process_client(file_client);

			char buffer[TAILLE_MAX];
			fgets_or_exit(buffer,TAILLE_MAX,file_client);

			if (strcmp("GET / HTTP/1.1\r\n",buffer)==0){
				int contentLength=0;

				do{
					fgets_or_exit(buffer,TAILLE_MAX,file_client);
					contentLength+=strlen(buffer);
				}while(strcmp(buffer,"\r\n"));
				contentLength-=2;//pour pas compter la dernière ligne "\r\n" 
				fprintf(file_client,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\n",contentLength);
			}else if (strcmp("GET /inexistant HTTP/1.1\r\n",buffer)==0){
				fprintf(file_client,"HTTP/1.1 404 File Not Found\r\n");
			}else{
				fprintf(file_client,"HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 17\r\n");
			}

			fclose(file_client);
			return EXIT_SUCCESS;
		}
		fclose(file_client);
		
	}

	return EXIT_SUCCESS;
}

void initialiser_signaux(void){
	struct sigaction sa;
	sa.sa_handler = traitement_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1){
		perror("sigaction(SIGCHLD)");
	}
}

void traitement_signal(){
	while (waitpid(-1,NULL,WNOHANG)>0);
}

char *fgets_or_exit(char *buffer, int size, FILE *stream){
	char* res=fgets(buffer,size,stream);
	if (res==NULL){
		exit(0);
	}
	return res;
}