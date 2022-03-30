#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include "col3-bibtp/communCOL3-TP.h"
#include "clientCOL3.h"

/* variables globales */

niveau_log NIVEAULOG = none; // mode debug
int NCURSE = 0;				 // affichage avec NCURSE

char NOMDUCLAN[TAILLE_MAX_NOM_CLAN]; // nom du clan
capacite_clan CAPACITECLAN;			 // capacite d'extraction du clan
hutte HUTTECLAN;					 // hutte du clan

pthread_mutex_t mutex_lect_hutte; // mutex pour la protection de la hutte
pthread_mutex_t mutex_red_hutte;  // mutex pour la protection de la hutte
pthread_mutex_t mutex_prio_hutte; // mutex pour la protection de la hutte
int nbLecteur_huttes = 0;		  // nombre de lecteurs sur la hutte

int PORT; // port de connexion au serveur
int PORT_PRETRESSE;
int PORTBATAILLE;	// port de connexion pour la bataille
char ADRESSE[100];	// adresse IP du serveur
char MONTOKEN[100]; // token unique du clan

pthread_t thread_merlin;
pthread_t thread_pretresse_feu;
pthread_t thread_pretresse_guerre;
params_thread_pretresse feu;
params_thread_pretresse guerre;

/*  programme principal du client CoL3  */
int main(int argc, char *argv[])
{
	int i = 1;
	int help = 0;
	int test = 0;

	pthread_mutex_init(&mutex_lect_hutte, NULL);
	pthread_mutex_init(&mutex_red_hutte, NULL);
	pthread_mutex_init(&mutex_prio_hutte, NULL);

	/* --- cette zone est à modifier à l'issue du jeu --- */
	strcpy(MONTOKEN, "IC:58:NC:2:LS:1-10-12-13-9-2-6-15-7-18-11-19-4-17-5-3");
	// strcpy(MONTOKEN, "IC:3:NC:5:LS:1-10-12-13-9-2-6-15-7-18-11-19-4-17-5-3");
	strcpy(NOMDUCLAN, "pajux.sinux.sh");
	strcpy(ADRESSE, "134.214.147.120");
	PORT = 8080;
	PORT_PRETRESSE = 8083;
	/* --- cette zone est à modifier à l'issue du jeu --- */

	/* boucle de traitements des arguments */
	while (i < argc && help == 0)
	{

		/* si demande d'aide */
		if ((0 == strcmp("-h", argv[i])) || (0 == strcmp("--help", argv[i])))
		{
			help = 1;
		}
		else if (0 == strcmp("--test", argv[i]))
		{
			test = 1;
		}
		else if (0 == strcmp("--full", argv[i]))
		{
			NIVEAULOG = full;
		}
		else if (0 == strcmp("--debug", argv[i]))
		{
			NIVEAULOG = debug;
		}
		else if (0 == strcmp("--info", argv[i]))
		{
			NIVEAULOG = info;
		}
		else if (0 == strcmp("--error", argv[i]))
		{
			NIVEAULOG = error;
		}
		else if (0 == strcmp("-p", argv[i]))
		{
			if (argv[i + 1] != NULL)
				PORT = (int)strtol(argv[i + 1], (char **)NULL, 10);
		}
		else if (0 == strcmp("-n", argv[i]))
		{
			if (argv[i + 1] != NULL)
				strcpy(NOMDUCLAN, argv[i + 1]);
		}
		else if (0 == strcmp("-t", argv[i]))
		{
			if (argv[i + 1] != NULL)
				strcpy(MONTOKEN, argv[i + 1]);
		}
		else if (0 == strcmp("-a", argv[i]))
		{
			if (argv[i + 1] != NULL)
				strcpy(ADRESSE, argv[i + 1]);
		}
		i++;
	}

	/* en cas d'aide : affichage des options */
	if (help == 1)
	{
		printf(" Description : pgm client Clash of L3 \n");
		printf(" Aide : [-h | --help] \n");
		printf(" Usage : col3-client-etu -a [adresseIP] -p [port] -n [nomduclan] -t [token]   \n");
		printf("	        	 [--test]\n");
		printf("	        	 [--full | --debug | --info | --error] \n");
		printf("  -p [port] : port d'accès du serveur CLAN (valeur par defaut = 8080) \n");
		printf("  -a [adresseIP] : adresse IP du serveur  (valeur par defaut = 127.0.0.1) \n");
		printf("  -n [nomduclan] : nom du clan) \n");
		printf("  -t [token] : token du clan) \n");
		printf("  --test  : lance le test de connexion avec le serveur CoL3\n");
		printf("  --full | --debug | --info | --error : lance le client avec un niveau de log \n\n");
	}
	/* sinon je lance la fonction principale */
	else
	{
		printf("\n  *** Clash Of L3 ****\n");

		printf("\n%s %s", "nom du Clan = ", NOMDUCLAN);
		printf("\n%s %s", "token du Clan = ", MONTOKEN);
		printf("\n%s %s", "adresse IP du serveur = ", ADRESSE);
		printf("\n%s %d", "port du serveur = ", PORT);
		printf("\n");

		loadHutteClan();

		logClientCOL3(debug, "main", "Hutte restauré !");
		afficheHutte(HUTTECLAN);

		if (test == 1)
			testServeur(ADRESSE, PORT, MSG_TEST, NOMDUCLAN);
		else
		{

			// Invoque Merlin, la pretresse de feu et la pretresse de la guerre
			pthread_create(&thread_merlin, NULL, merlin_syncronisateur, NULL);
			pthread_detach(thread_merlin);

			feu.adresseip = ADRESSE;
			feu.port = PORT_PRETRESSE;
			feu.tokenduclan = MONTOKEN;
			feu.nomduclan = NOMDUCLAN;
			feu.MSG_QUEST = MSG_QUEST_FEU;

			guerre.adresseip = ADRESSE;
			guerre.port = PORT_PRETRESSE;
			guerre.tokenduclan = MONTOKEN;
			guerre.nomduclan = NOMDUCLAN;
			guerre.MSG_QUEST = MSG_QUEST_GUERRE;

			logClientCOL3(debug, "main", "Creation thread pretresse feu !");
			pthread_create(&thread_pretresse_feu, NULL, pretresse, (void *)&feu);
			// pthread_detach(thread_pretresse_feu);
			pthread_create(&thread_pretresse_guerre, NULL, pretresse, (void *)&guerre);
			// pthread_detach(thread_pretresse_guerre);

			pthread_join(thread_pretresse_feu, NULL);
			pthread_join(thread_pretresse_guerre, NULL);

			/*
			int nb_char;
			lessitesdumonde ns;
			int socket = connexionServeurCOL3(ADRESSE, PORT, MONTOKEN, NOMDUCLAN);
			recupSiteExtraction(socket, ns, &nb_char);

			envoieChariots(ADRESSE, PORT, MONTOKEN, NOMDUCLAN);

			afficheHutte(HUTTECLAN);
			*/
		}

		pthread_mutex_destroy(&mutex_lect_hutte);
		pthread_mutex_destroy(&mutex_red_hutte);
		pthread_mutex_destroy(&mutex_prio_hutte);
	}

	return 0;
}
