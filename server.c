#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <arpa/inet.h>

#define PORT 2025

extern int errno;

int search_product(char *aliment, sqlite3 *db) {

   sqlite3_stmt *statement;
   char *sql = "SELECT Nume_produs FROM DONATII WHERE Nume_produs = ?";

   int rc = sqlite3_open("test.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
   }

   if (sqlite3_prepare_v2(db, sql, -1, &statement, NULL) != SQLITE_OK) {
        fprintf(stderr, "Eroare la compilarea query-ului SQL\n");
        sqlite3_close(db);
        return 1;
   }

   if (sqlite3_bind_text(statement, 1, aliment, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "Eroare la legarea valorii produsului la placeholder\n");
        sqlite3_finalize(statement);
        sqlite3_close(db);
        return 1;
    }

   int gasit = 0;

   if (sqlite3_step(statement) == SQLITE_ROW) {
      // Procesează rândul găsit
      gasit = 1;
    }

   sqlite3_finalize(statement);
   // sqlite3_close(db);

   if (gasit)
      return 1;
   return 0;
}

int quantity_product(char *aliment, sqlite3 *db) {
   sqlite3_stmt *statement;
   char *sql = "SELECT Cantitate FROM DONATII WHERE Nume_produs = ?";

   int rc = sqlite3_open("test.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
   }

   if (sqlite3_prepare_v2(db, sql, -1, &statement, NULL) != SQLITE_OK) {
        fprintf(stderr, "Eroare la compilarea query-ului1 SQL\n");
        sqlite3_close(db);
        return 1;
   }

   if (sqlite3_bind_text(statement, 1, aliment, -1, SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "Eroare la leagarea valorii produsului la placeholder\n");
        sqlite3_finalize(statement);
        sqlite3_close(db);
        return 1;
    }

    rc = sqlite3_step(statement);
    int quantity = 0;

    if (rc == SQLITE_ROW) {
        // Process the result if a row is returned
        // printf("Quantity for %s: %d\n", productName, sqlite3_column_int(stmt, 0));
      quantity = sqlite3_column_int(statement, 0);
    } else if (rc == SQLITE_DONE) {
        // No rows returned
        printf("Quantity not found.\n");
    } else {
        // Error occurred
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(statement);
    // sqlite3_close(db);

    return quantity;
}

void add_product(char *aliment, int cantitate, sqlite3 *db) {

    char *err_msg = 0; 
    sqlite3_stmt *statement;
    
    int rc = sqlite3_open("test.db", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return;
   }

   char *sql = "INSERT INTO DONATII(Nume_produs, Cantitate) VALUES(?, ?)";

   if (sqlite3_prepare_v2(db, sql, -1, &statement, NULL) != SQLITE_OK) {
      fprintf(stderr, "Eroare la compilarea query-ului SQL\n");
      sqlite3_close(db);

      return;
   }

   if (sqlite3_bind_text(statement, 1, aliment, -1, SQLITE_STATIC) != SQLITE_OK) {
      fprintf(stderr, "Eroare la leagarea valorii produsului la placeholder\n");
      sqlite3_finalize(statement);
      sqlite3_close(db);

      return;
   }

   if (sqlite3_bind_int(statement, 2, cantitate) != SQLITE_OK) {
      fprintf(stderr, "Eroare la leagarea valorii cantității la placeholder\n");
      sqlite3_finalize(statement);
      sqlite3_close(db);

        return;
   }

   if (sqlite3_step(statement) != SQLITE_DONE) {
      fprintf(stderr, "Eroare la executarea query-ului\n");
      sqlite3_finalize(statement);
      sqlite3_close(db);

      return;
   }

   sqlite3_finalize(statement);
   // sqlite3_close(db);

   printf("[server]Am adaugat alimentul si cantitatea.\n");
   printf("[server]Donatie inregistrata, va multumim!\n");
}

void update_product(char *aliment, int cantitate, sqlite3 *db) {
   sqlite3_stmt *stmt;

   int rc = sqlite3_open("test.db", &db);

   if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
   }

   const char *updateQuery = "UPDATE DONATII SET Cantitate = ? WHERE Nume_produs = ?;";

   rc = sqlite3_prepare_v2(db, updateQuery, -1, &stmt, NULL);

   if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
   }

   cantitate = cantitate + quantity_product(aliment, db);

   rc = sqlite3_bind_int(stmt, 1, cantitate);

   if (rc != SQLITE_OK) {
        fprintf(stderr, "Error binding quantity: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
   }

   rc = sqlite3_bind_text(stmt, 2, aliment, -1, SQLITE_STATIC);

   if (rc != SQLITE_OK) {
        fprintf(stderr, "Error binding product name: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
   }

   rc = sqlite3_step(stmt);

   if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error updating data: %s\n", sqlite3_errmsg(db));
   }

   sqlite3_finalize(stmt);
   // sqlite3_close(db);

   printf("[server]Am actualizat alimentul si cantitatea.\n");
   printf("[server]Donatie inregistrata, va multumim!\n");
}

void update_donations(char *aliment, int cantitate, sqlite3 *db) {
   sqlite3_stmt *stmt;

   int rc = sqlite3_open("test.db", &db);

   if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
   }

   const char *updateQuery = "UPDATE DONATII SET Cantitate = ? WHERE Nume_produs = ?;";

   rc = sqlite3_prepare_v2(db, updateQuery, -1, &stmt, NULL);

   if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
   }

   cantitate = quantity_product(aliment, db) - cantitate;

   rc = sqlite3_bind_int(stmt, 1, cantitate);

   if (rc != SQLITE_OK) {
        fprintf(stderr, "Error binding quantity: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
   }

   rc = sqlite3_bind_text(stmt, 2, aliment, -1, SQLITE_STATIC);

   if (rc != SQLITE_OK) {
        fprintf(stderr, "Error binding product name: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
   }

   rc = sqlite3_step(stmt);

   if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error updating data: %s\n", sqlite3_errmsg(db));
   }

   sqlite3_finalize(stmt);
}

void delete_product(char* aliment, sqlite3 *db) {
   sqlite3_stmt *stmt;

   int rc = sqlite3_open("test.db", &db);

   if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
   }

   const char *deleteQuery = "DELETE FROM DONATII WHERE Nume_produs = ?;";

   rc = sqlite3_prepare_v2(db, deleteQuery, -1, &stmt, NULL);

   if (rc != SQLITE_OK) {
      fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return;
   }

   rc = sqlite3_bind_text(stmt, 1, aliment, -1, SQLITE_STATIC);

   if (rc != SQLITE_OK) {
      fprintf(stderr, "Error binding product name: %s\n", sqlite3_errmsg(db));
      sqlite3_finalize(stmt);
      sqlite3_close(db);
      return;
   }

   rc = sqlite3_step(stmt);
   if (rc != SQLITE_DONE) {
      fprintf(stderr, "Error deleting data: %s\n", sqlite3_errmsg(db));
   }

   sqlite3_finalize(stmt);
}


int main () {

   sqlite3 *db;
   char *err_msg = 0;

   int rc = sqlite3_open("test.db", &db);

   if (rc != SQLITE_OK) {
      fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return 1;
   }

   char *sql = "DROP TABLE IF EXISTS DONATII;"
               "CREATE TABLE DONATII(Nume_produs TEXT, Cantitate INT);";


   rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

   if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", err_msg);
      sqlite3_free(err_msg);
      sqlite3_close(db);
      return 1;
   }

    struct sockaddr_in server;	
    struct sockaddr_in from;
    char msg[100];		
    char msgrasp[100]=" ";       
    int sd;			

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
    	perror ("[server]Eroare la socket().\n");
    	return errno;
    }


    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);

    int optval = 1;

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));

    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1) {
    	perror ("[server]Eroare la bind().\n");
    	return errno;
    }

    if (listen (sd, 1) == -1) {
    	perror ("[server]Eroare la listen().\n");
    	return errno;
    }

    while (1) {
    	int client;
    	int length = sizeof (from);

    	printf ("[server]Server accesibil la portul %d...\n",PORT);
    	fflush (stdout);

    	client = accept (sd, (struct sockaddr *) &from, &length);

    	if (client < 0)
    	{
    		perror ("[server]Eroare la accept().\n");
    		continue;
    	}

      printf ("[server]S-a conectat un client...\n");
      fflush (stdout);

    	int pid;
    	if ((pid = fork()) == -1) {
    		close(client);
    		continue;
    	} else if (pid > 0) {
    		// parinte
    		close(client);
    		while(waitpid(-1,NULL,WNOHANG));
    		continue;
    	} else if (pid == 0) {
    		// copil
    		close(sd);

    		bzero (msg, 100);

    		printf ("[server]Asteptam sa verificam tipul de client ...\n");
    		fflush (stdout);

    		if (read (client, msg, 100) <= 0) {
    		   perror ("[server]Eroare la read() de la client.\n");
    			close (client);	
    			continue;		
    		}

    		printf ("[server]Tipul de client: %s\n", msg);

         bzero(msgrasp,100);
         strcat(msgrasp,"Server-ul a primit optiunea dvs.: ");
         strcat(msgrasp,msg);

         printf("[server]Trimitem mesajul catre client...%s\n",msgrasp);

         if (write(client, msgrasp, sizeof(msgrasp)) <= 0) {
            perror ("[server]Eroare la write() catre client.\n");
            continue;
         }else
            printf ("[server]Mesajul a fost trasmis cu succes.\n \n");

      /* TE CONTINUI MAINE */

         if (strcmp(msg, "restaurant") == 0) {
             printf("Clientul este de tip restaurant\n \n");
            // fflush(stdout);

            char response[100];
            bzero(response, 100);

            printf("[server]Astept alimentul dorit: \n");
            fflush(stdout);

            if (read(client, response, sizeof(response)) <= 0) {
               perror("[server]Eroare la read din client.\n");
               close(client);
               continue;
            }

            printf("[server]Am primit alimentul: %s\n", response);

            int cantitate = 0;

            printf("[server]Astept cantitatea: \n");
            fflush(stdout);

            if (read(client, &cantitate, sizeof(cantitate)) <= 0) {
               perror("[server]Eroare la read din client.\n");
               close(client);
               continue;
            }

            printf("[server]Am primit cantitatea: %d\n", cantitate); 

            if (search_product(response, db)) {
               update_product(response, cantitate, db);
            }else 
               add_product(response, cantitate, db);
 
            close(client);
            exit(0);

         }else if (strcmp(msg, "cetatean") == 0) {
             printf("Clientul este de tip cetatean\n");
             fflush(stdout);

            char response[100];
            bzero(response, 100);

            printf("[server]Astept alimentul dorit: \n");
            fflush(stdout);

            if (read(client, response, sizeof(response)) <= 0) {
               perror("[server]Eroare la read din client.\n");
               close(client);
               continue;
            }

            printf("[server]Am primit alimentul: %s\n", response);

            int searched_product = search_product(response, db);

            if (write(client, &searched_product, sizeof(searched_product)) <= 0) {
               perror ("[server]Eroare la write() catre client.\n");
               continue;
            }else
               printf ("[server]Mesajul a fost trimis.\n \n");

            if (search_product(response, db) == 1) {

               int cantitate = 0;

               printf("[server]Astept cantitatea: \n");
               fflush(stdout);

               if (read(client, &cantitate, sizeof(cantitate)) <= 0) {
                  perror("[server]Eroare la read din client.\n");
                  close(client);
                  continue;
               }

               printf("[server]Am primit cantitatea: %d\n", cantitate);
               fflush(stdout);

               // printf("%d\n", quantity_product(response, db));
               printf("[server]Va multumim si va mai asteptam!\n");

               if (cantitate == quantity_product(response, db)) {
                  delete_product(response, db);
               }else if (cantitate < quantity_product(response, db)) {
                  update_donations(response, cantitate, db);
               }else if (cantitate > quantity_product(response, db)) 
                  printf("[server]Nu avem aceasta cantitate!\n");


               close(client);
               exit(0);
            }else printf ("[server]Produsul nu exista in baza de date.\n \n");
         }else {
            printf("[server]Optiune inexistenta. La revedere!\n");
            fflush(stdout);
         }
            

    		close (client);
    		exit(0);

         printf("\n \n \n");
         
    	} //child

    }		// while	

}	// main		

