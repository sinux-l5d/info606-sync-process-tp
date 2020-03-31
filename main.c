#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "col3-bib/communCOL3.h"

/* variables globales */

int debug = 0;						// mode debug
int NCURSE =0;						// affichage avec NCURSE

char  NOMDUCLAN[TAILLE_MAX_NOM_CLAN];  	// nom du clan
capacite_clan CAPACITECLAN;				// capacite d'extraction du clan
hutte HUTTECLAN;					  	// hutte du clan

int PORT;								// port de connexion au serveur
int PORTBATAILLE ;						// port de connexion pour la bataille
char ADRESSE[20];						// adresse IP du serveur
char MONTOKEN[100];						// token unique du clan

/*  programme principal du client CoL3  */
int main(int argc, char *argv[])
{
	int i=1 ;
	int help=0;
	int test=0;
	
	/* --- cette zone est à modifier à l'issue du jeu --- */
	strcpy(MONTOKEN,"TEST"); 
    strcpy(NOMDUCLAN,"TEST");	
	strcpy(ADRESSE,"127.0.0.1");
	PORT = 8080;
    /* --- cette zone est à modifier à l'issue du jeu --- */

	
	/* boucle de traitements des arguments */
    while(i < argc && help==0) {

		/* si demande d'aide */
		if ((0 == strcmp("-h",argv[i])) || (0 == strcmp("--help",argv[i]))){
			help=1;
        } 
		else if (0 == strcmp("--test",argv[i])){
			test=1;
        } 
		else if (0 == strcmp("--debug",argv[i])){
			debug=1;
        } 
        else if (0 == strcmp("-p",argv[i])){
			if (argv[i+1]!= NULL) PORT  = (int) strtol(argv[i+1], (char **)NULL, 10);
		} 
		else if (0 == strcmp("-n",argv[i])){
            if (argv[i+1]!= NULL)  strcpy(NOMDUCLAN,argv[i+1]);
        }
		else if (0 == strcmp("-s",argv[i])){
            if (argv[i+1]!= NULL)  strcpy(ADRESSE ,argv[i+1]);
        }		
        i++;
    }

	/* en cas d'aide : affichage des options */
	if (help == 1)
	{
		printf(" Description : pgm client Clash of L3 \n");
		printf(" Aide : [-h | --help] \n");
		printf(" Usage : col3-client-etu -p [port] -s [adresseIP] -n [nomduclan]\n");
		printf("	        	         [--debug] [--test]\n");
		printf("  -p [port]      : port d'accès du serveur CoL3 (valeur par defaut = 8080) \n");
		printf("  -s [adresseIP] : adresse IP du serveur CoL3 (valeur par defaut = 127.0.0.1) \n");
		printf("  -n [nomduclan] : nom du clan (valeur par défaut = TEST \n");
		printf("  --debug : lance le client en mode debug \n");
		printf("  --test  : lance le test de connexion avec le serveur CoL3\n\n");
	}
	/* sinon je lance la fonction principale */
	else
	{
		printf("\n  *** Clash Of L3 ****\n");

		printf("\n%s %s", "nom du Clan = ",NOMDUCLAN);
		printf("\n%s %s", "token du Clan = ",MONTOKEN);
		printf("\n%s %s", "adresse IP du serveur= ",ADRESSE);
		printf("\n%s %d", "port du serveur = ",PORT);
		printf("\n");

		if (test==1)
			testServeur(NOMDUCLAN,ADRESSE,PORT);
		else
		{
			printf("%s", "a completer \n");	
		}
	}
		
    return 0;
}
