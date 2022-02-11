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

#define TAILLE_MAX 8192

int process_client(FILE* file_client){
	for (int i=0;i<3;i++){
	
		fprintf(file_client,"[%d] Bonjour !\n",i);
		fflush(file_client);

		//write(socket_client, message_bienvenue, strlen(message_bienvenue));
		
		sleep(1);
	}

	char buffer[TAILLE_MAX];
	fgets(buffer,TAILLE_MAX,file_client);

	fprintf(file_client,"<Zyzz> %s\n",buffer);
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
