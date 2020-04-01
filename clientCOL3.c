#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include "col3-bibtp1/communCOL3-TP1.h"
#include "clientCOL3.h"



/* ===================================
     fonction d'extraction des sites 
       (a completer)
  ==================================*/
		   
void recupSiteExtraction()
{

 printf("\n  *** au boulot ... ****\n");
	
}


/* ===================================
     fonction de recupération de MP 
       (a completer)
  ==================================*/
		   
void gestionAppro()
{

 printf("\n  *** au boulot ... ****\n");
	
}


/*  ======================================
	  fonction de test d'échange initiale 
      avec le serveur
    =====================================*/ 
int testServeur(const char * nomduclan, const char * adresseip, int port) {
	
	int socket;

	char msgrecu[TAILLE_MAX_MSG];
	char msgreponse[TAILLE_MAX_MSG];

	printf("%s %s %s","\n *** le clan[",nomduclan,"] crée une socket pour tester le serveur *** \n");

	/* -----------------------------
	   ECHANGE 1 : envoi du token de test 
	   ----------------------------- */

	/* creation et connexion au serveur de socket */
	socket = connexionServeurCOL3(adresseip,port,MSG_TEST); // on met MSG_TEST à la place du TOKEN


	/* -----------------------------
	   ECHANGE 2 : valildation du token  
	   ----------------------------- */
	if (socket != INVALID_SOCKET) {
		printf("%s %s %s","\n *** le can[",nomduclan,"] attend la validation du token de test ***\n");
		/* en attente de message */
		lireMessageCOL3(socket,msgrecu);

		/* test si le message est une demande de nom */
		if (strcmp(msgrecu,MSG_TOKEN_OK)==0)
		{
			printf("%s %s %s %s %s","\n *** le clan[",nomduclan,"] a ete reconnu par le serveur, code recu ",msgrecu," [OK] ***\n" );
		} else {
			printf("%s %s %s %s %s","\n *** le clan[",nomduclan,"] n a ete reconnu par le serveur , token recu ",msgrecu," [NOK] ***\n" );
			socket=-1;
		}

	/* -----------------------------
	   ECHANGE 3 : envoi du nom  
	   ----------------------------- */

	printf("%s %s %s","\n *** le clan[",nomduclan,"] envoie son nom pour tester le serveur *** \n");
	strcpy(msgreponse,MSG_CLAN);
	strcat(msgreponse,MSG_DELIMINTER);
	strcat(msgreponse,nomduclan);

	/* envoi du nom */
	envoiMessageCOL3(socket,msgreponse);

	/* -----------------------------
	   ECHANGE 4 : validation du nom  
	   ----------------------------- */
	
	printf("%s %s %s","\n *** le clan[",nomduclan,"] attend la validation de son nom  ***\n");
    /* en attente de message */
	lireMessageCOL3(socket,msgrecu);

	/* test si le nom a été recu*/
	if (strcmp(msgrecu,MSG_NOMCLAN_OK)==0)
	{
		printf("%s %s %s %s %s","\n *** le clan[",nomduclan,"] a été identifié , fin TEST , nom recu ",msgrecu," [OK] ***\n" );

	} else {
		printf("%s %s %s %s %s","\n *** le clan[",nomduclan,"] n a pas été identifié , fin TEST, nom pas recu ",msgrecu," [NOK] ***\n" );
		socket=-1;
	}
	}

	return socket;
}







/* ========================================================
       les fonctions de lecture et d ecriture de la hutte
          (attention sans mutex .... à modifier )
   ===================================================*/ 

/** 
 * cette fonction permet d'écrire un fichier de hutte
 * en utilisant la variable globale HUTTECLAN qui est
 * en section critique donc pensez au verrou .....
 *
 */
void ecritureHutteSansMutex(char path[]) {

  FILE *fichier = NULL;
  char tmp[TAILLE_MAX];


  /* conversion de hutte en texte */ 
  hutteToMessage(&HUTTECLAN,tmp);

  fichier = fopen(path, "w");
  if(fichier != NULL){
     fprintf(fichier, tmp);
  }
  fclose(fichier);

}

void lectureFichierHutteSansMutex (hutte * mahutte, char path[]) {

  FILE *fichier;
  char ligne[40] = "";
  int i=0;

  if (!NCURSE) printf("%s %s %s", "--> lecture du fichier ",path,"\n");

  fichier = fopen(path, "r");
  if(fichier != NULL){

     while (fgets(ligne, 40, fichier) != NULL)
        {
            if (!NCURSE) printf("%s %d %s %s %s", "lecture fichier hutte ligne",i," = ",ligne,"\n");
			messageToHutte(ligne, mahutte);
        }


  } else {
    perror("Fichier hutte abs.");
    exit(errno);
  }
  fclose(fichier);
}
