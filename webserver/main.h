#ifndef MAIN_H
#define MAIN_H

void initialiser_signaux(void);
void traitement_signal();
int process_client(FILE* socket_client);
char *fgets_or_exit(char *buffer, int size, FILE *stream);

int skip_headers(FILE *client);
void send_status(FILE *client, int code, const char *reason_phrase);
void send_response(FILE *client, int code, const char *reason_phrase,const char *message_body);

char *rewrite_target(char *target);

#endif

