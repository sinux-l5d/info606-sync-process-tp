#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include "col3-bibtp/communCOL3-TP.h"
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

int testServeur( const char * adresseip, int port,const char * tokenduclan,const char * nomduclan) {
	
	int socket,ret;

	char msgrecu[TAILLE_MAX_MSG];
	

	printf("%s %s %s","\n *** le clan[",nomduclan,"] crée une socket pour tester le serveur *** \n");

	/* -----------------------------
	   ECHANGE 1 : envoi du token de test 
	   ----------------------------- */

	/* creation et connexion au serveur de socket */
	socket = connexionServeurCOL3(adresseip,port,tokenduclan,nomduclan); // on met MSG_TEST à la place du TOKEN


	/* -----------------------------
	   ECHANGE 2 : valildation echange  
	   ----------------------------- */
	if (socket != INVALID_SOCKET) {

		logClientCOL3(info,"test", 
					  "le clan[%s] a validé son test de connexion  %b ",
					  nomduclan,debug_ok); 
		close(socket);
			
	} 
	else {
				logClientCOL3(error,"test", 
					  "le clan[%s] n'a pas validé son test de connexion  %b ",
					  nomduclan,debug_nok); 
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

