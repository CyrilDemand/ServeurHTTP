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

#include "http_parse.h"

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

			http_request request;
			int parse_ret=parse_http_request(buffer, &request);
			
			char* motd="Bienvenue chez Zyzz !\n";

			if (parse_ret == -1) {
				if (request.method == HTTP_UNSUPPORTED){
					send_response(file_client, 405, "Method Not Allowed", "Method Not Allowed\r\n");
				}else{
					send_response(file_client, 400, "Bad Request", "Bad request\r\n");
				}
			}else if (strcmp(request.target, "/") == 0){

				int contentLength=skip_headers(file_client);

				char buffer[50];
				snprintf(buffer, sizeof(buffer), "Content-Length: %d\r\n%s\r\n",contentLength,motd);
				
				send_response(file_client, 200, "OK", buffer);
			}else{
				send_response(file_client, 404, "Not Found", "Not Found\r\n");
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

int skip_headers(FILE *client){
	int contentLength=0;
	char buffer[TAILLE_MAX];
	do{
		fgets_or_exit(buffer,TAILLE_MAX,client);
		contentLength+=strlen(buffer);
	}while(strcmp(buffer,"\r\n"));
	contentLength-=2;//pour pas compter la dernière ligne "\r\n" 
	return contentLength;
}

void send_status(FILE *client, int code, const char *reason_phrase){
	fprintf(client,"HTTP/1.1 %d %s\r\n",code,reason_phrase);
}

void send_response(FILE *client, int code, const char *reason_phrase,const char *message_body){
	send_status(client,code,reason_phrase);
	fprintf(client,"%s\r\n",message_body);
}

char* rewrite_target(char *target){
    int i=0;
    while (target[i]!='?' && i<strlen(target)){
        i++;
    }

    char* truc=malloc(sizeof(char)*i);
    strncpy(truc,&target[0],i);
    truc[i]='\0';
    
    return truc;
}