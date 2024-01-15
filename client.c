#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sqlite3.h>

extern int errno;

int port;

int main (int argc, char *argv[]) {
  int sd;			
  struct sockaddr_in server;	
  char msg[100];		

  if (argc != 3) {
    printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
    return -1;
  }

  port = atoi (argv[2]);

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
    perror ("Eroare la socket().\n");
    return errno;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons (port);
  
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1) {
    perror ("[client]Eroare la connect().\n");
    return errno;
  }

  bzero (msg, 100);
  printf ("[client]Introduceti tipul de client: [ restaurant / cetatean ] ");
  fflush (stdout);

  read (0, msg, 100);
  msg[strlen(msg) - 1] = 0;

  printf("[client]Tipul de client ales: %s\n", msg);
  fflush(stdout);

  if (write (sd, msg, 100) <= 0) {
    perror ("[client]Eroare la write() spre server.\n");
    return errno;
  }

  char replica[100];
  strcpy(replica, msg);


  if (read (sd, msg, sizeof(msg)) < 0) {
      perror ("[client]Eroare la read() de la server.\n");
      return errno;
  }

  printf ("[client]Mesajul primit este: %s\n", msg);
  fflush(stdout);

  if (strcmp(replica, "cetatean") == 0) {
    char response[100];
    bzero(response, 100);

    printf("[client]Ce alimente doriti? ");
    fflush(stdout);

    read(0, response, 100);
    response[strlen(response) - 1] = 0;

    if (write(sd, response, sizeof(response)) <= 0) {
      perror("[client]Eroare la scrierea catre server.\n");
      return errno;
    }


    int response_for_product = 0;

    if (read (sd, &response_for_product, sizeof(response_for_product)) < 0) {
         perror ("[client]Eroare la read() de la server.\n");
         return errno;
    }

    if (response_for_product) {

      int cantitate = 0;
      printf("[client]Introduceti cantitatea dorita: ");
      fflush(stdout);

      scanf("%d", &cantitate);
      printf("[client]Cantitatea introdusa: %d\n", cantitate);
      fflush(stdout);

      if (write(sd, &cantitate, sizeof(cantitate)) <= 0) {
        perror("[client]Eroare la scrierea catre server.\n");
        return errno;
      }
    } 

  }else if (strcmp(replica, "restaurant") == 0) {
    char response[100];
    bzero(response, 100);

    printf("[client]Ce alimente doriti sa donati? ");
    fflush(stdout);

    read(0, response, 100);
    response[strlen(response) - 1] = 0;

    if (write(sd, response, sizeof(response)) <= 0) {
      perror("[client]Eroare la scrierea catre server.\n");
      return errno;
    }

    int cantitate = 0;
    printf("[client]Introduceti cantitatea dorita: ");
    fflush(stdout);

    scanf("%d", &cantitate);
    printf("[client]Cantitatea introdusa: %d\n", cantitate);
    fflush(stdout);

    if (write(sd, &cantitate, sizeof(cantitate)) <= 0) {
      perror("[client]Eroare la scrierea catre server.\n");
      return errno;
    } 
    
  }

  close (sd);
}
