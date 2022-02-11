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

int process_client(int socket_client){
	for (int i=0;i<10;i++){
		const char *message_bienvenue = "Bonjour, bienvenue sur mon serveur\n";
		
		write(socket_client, message_bienvenue, strlen(message_bienvenue));
		
		sleep(1);
	}

	return 0;
}

int main(){
	int socket_serveur=creer_serveur(8080);

	initialiser_signaux();

	int socket_client;
	if (socket_serveur==-1){
		perror("serveur");
		return EXIT_FAILURE;
	}

	while (1){

			
		socket_client = accept(socket_serveur, NULL, NULL);
		if (socket_client == -1){
			perror("accept");
		}
		if (fork()==0) {
			process_client(socket_client);
			close(socket_client);
			return EXIT_SUCCESS;
		}
		close(socket_client);
		
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
