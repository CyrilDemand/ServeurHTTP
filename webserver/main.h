#ifndef MAIN_H
#define MAIN_H

void initialiser_signaux(void);
void traitement_signal();
int process_client(FILE* socket_client);
char *fgets_or_exit(char *buffer, int size, FILE *stream);

#endif

