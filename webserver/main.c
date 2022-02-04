#include <stdio.h>
#include "main.h"
#include "socket.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>



int main(){
	int socket_serveur=creer_serveur(8080);

	initialiser_signaux();

	int socket_client;
	if (socket_serveur==-1){
		perror("serveur");
		return -1;
	}

	while (1){
		
		socket_client = accept(socket_serveur, NULL, NULL);
		if (socket_client == -1){
			perror("accept");
		}

		for (int i=0;i<10;i++){
			const char *message_bienvenue = "Bonjour, bienvenue sur mon serveur\n";
			
			write(socket_client, message_bienvenue, strlen(message_bienvenue));
			
			sleep(1);
		}
		
	}
}

void initialiser_signaux(void){
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR){
		perror("signal");
	}
}