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

const char *getMatiereName(matieres_premieres mat)
{
	switch (mat)
	{
	case bois:
		return "bois";
	case salpetre:
		return "salpetre";
	case charbon:
		return "charbon";
	case soufre:
		return "soufre";
	case fer:
		return "fer";
	case chanvre:
		return "chanvre";
	default:
		return "";
	}
}

void afficheRessources(lessitesdumonde nossite)
{
	logClientCOL3(info, "afficheRessources", "sites accessibles du clan:");
	for (int i = 0; i < MAX_SITE_EXTRACTION; i++)
	{
		if (nossite[i].idSite == 0)
			break;
		logClientCOL3(info, "afficheRessources", " -> id=%d nom=%s lon=%d lat=%d stock=%d matiere=%s duree=%d",
					  nossite[i].idSite,
					  nossite[i].nomSite,
					  nossite[i].longitude,
					  nossite[i].latitude,
					  nossite[i].quantite,
					  getMatiereName(nossite[i].matiere),
					  nossite[i].duree);
	}
}

int nbSites(lessitesdumonde nossites)
{
	int nb = 0;
	for (int i = 0; i < MAX_SITE_EXTRACTION; i++)
	{
		if (nossites[i].idSite == 0)
			break;
		nb++;
	}
	return nb;
}

void recupSiteExtraction(int socket, lessitesdumonde nossites, int *nb_chariots, int afficherClan)
{
	char buffer[TAILLE_MAX_MSG];
	capacite_clan resp;

	if (socket == INVALID_SOCKET)
	{
		logClientCOL3(error, "recupSiteExtraction", "Erreur de connexion !");
		return;
	}

	sprintf(buffer, "%s%s%s", MSG_SITE, MSG_DELIMITER, MSG_QUEST);

	if (envoiMessageCOL3_s(socket, buffer) != 1)
	{
		logClientCOL3(error, "recupSiteExtraction", "Erreur d'envoie de message !");
	}
	else
	{
		logClientCOL3(debug, "recupSiteExtraction", "Connexion réussi !");

		lireStructureCOL3_s(socket, &resp, sizeof resp);

		*nb_chariots = resp.nbChariotDisponible;

		if (afficherClan)
			afficheCapaciteDuClan(resp);

		for (int i = 0; i < MAX_SITE_EXTRACTION; i++)
			nossites[i] = resp.sitesAccessibles[i];
	}
}

/* ===================================
	 fonction de recupération de MP
	   (a completer)
  ==================================*/

void envoieChariots(const char *adresseip, int port, const char *tokenduclan, const char *nomduclan)
{
	int socket = connexionServeurCOL3(adresseip, port, tokenduclan, nomduclan);
	lessitesdumonde nossites;
	int nb_chariots = 0;

	recupSiteExtraction(socket, nossites, &nb_chariots, 0);
	close(socket);

	pthread_t thread[MAX_CHARIOT_PAR_CLAN];
	params_thread_gestionAppro params[MAX_CHARIOT_PAR_CLAN];

	// pour chaque chariot disponible, on créer un thread
	for (int i = 0; i < nb_chariots; i++)
	{
		params[i].adresseip = adresseip;
		params[i].port = port;
		params[i].tokenduclan = tokenduclan;
		params[i].nomduclan = nomduclan;
		pthread_create(&thread[i], NULL, gestionAppro, (void *)&params[i]);
		pthread_detach(thread[i]);
	}
}

/**
 * Enregistrer la hutte dans un fichier
 */
void saveHutteClan()
{
	char msg[100]; // On prévoit large
	FILE *fichier = NULL;

	fichier = fopen("hutte.txt", "w");
	if (fichier != NULL)
	{
		hutteToMessage(&HUTTECLAN, &msg);
		fprintf(fichier, "%s", msg);
		fclose(fichier);
	}
	else
	{
		logClientCOL3(error, "saveHutteClan", "Erreur d'enregistrement de la hutte du clan");
	}
}

void loadHutteClan()
{
	logClientCOL3(info, "loadHutteClan", "Chargement de la hutte du clan");

	char msg[100];
	FILE *fichier = NULL;

	fichier = fopen("hutte.txt", "r");

	if (fichier != NULL)
	{
		fscanf(fichier, "%s", msg);
		messageToHutte(msg, &HUTTECLAN);
		fclose(fichier);
	}
	else
	{
		logClientCOL3(error, "loadHutteClan", "Erreur d'ouverture du fichier !");
	}
}

int estQuantiteValide(matieres_premieres matiere, int quantite)
{
	int newQuantite = HUTTECLAN.stock[matiere] + quantite;
	return newQuantite < 0 ? 0 : newQuantite > CAPACITE_MAX_HUTTE[matiere] ? 0
																		   : 1;
}

/**
 * Modifier le stock d'un matériel donné d'une quantité donnée.
 * La quantité peut être négative.
 *
 * @param matière la matière_premiere que vous souhaitez modifier
 * @param nb Le nombre d'unités à ajouter au stock, peut-être négatif
 */
int modifieStock(matieres_premieres matiere, int nb)
{
	if (!estQuantiteValide(matiere, nb))
		return 0;

	pthread_mutex_lock(&mutex_prio_hutte);
	pthread_mutex_lock(&mutex_red_hutte);
	pthread_mutex_unlock(&mutex_prio_hutte);
	while (estQuantiteValide(matiere, nb < 0 ? -1 : 1) && nb != 0)
	{
		HUTTECLAN.stock[matiere] += nb < 0 ? -1 : 1;
		if (nb < 0)
		{
			nb++;
		}
		else
		{
			nb++;
		}
	}

	pthread_mutex_unlock(&mutex_red_hutte);
	return 1;
}

int lisStock(matieres_premieres matiere)
{
	int nb_matiere;
	pthread_mutex_lock(&mutex_prio_hutte);
	pthread_mutex_lock(&mutex_lect_hutte);

	if (++nbLecteur_huttes == 1)
		pthread_mutex_lock(&mutex_red_hutte);

	pthread_mutex_unlock(&mutex_lect_hutte);
	pthread_mutex_unlock(&mutex_prio_hutte);

	nb_matiere = HUTTECLAN.stock[matiere];

	pthread_mutex_lock(&mutex_lect_hutte);

	if (--nbLecteur_huttes == 0)
		pthread_mutex_unlock(&mutex_red_hutte);

	pthread_mutex_unlock(&mutex_lect_hutte);

	return nb_matiere;
}

/**
 * Gère l'approvisionnement d'une ressource avec un chariot.
 * Fonction thread.
 */
void *gestionAppro(void *params)
{
	char req[TAILLE_MAX_MSG];
	char resp[TAILLE_MAX_MSG];
	char **tab_msg;
	int socket;
	lessitesdumonde nossites;
	int nb_chariots = 0;
	params_thread_gestionAppro *donnees = (params_thread_gestionAppro *)params;
	int site;
	int aleat;

	// On récupere les sites depuis le serveur
	socket = connexionServeurCOL3(donnees->adresseip, donnees->port, donnees->tokenduclan, donnees->nomduclan);
	recupSiteExtraction(socket, nossites, &nb_chariots, 0);
	close(socket);

	while (1)
	{

		// On choisi le site que l'on va extraire
		// aleat = rand() % nbSites(nossites);
		// site = nossites[aleat].idSite;
		site = 9;

		// affiche site choisi
		logClientCOL3(info, "gestionAppro", "Site choisi : %d", site);

		// On envoie le chariot récuperer la ressource

		socket = connexionServeurCOL3(donnees->adresseip, donnees->port, donnees->tokenduclan, donnees->nomduclan);

		if (socket == INVALID_SOCKET)
		{
			logClientCOL3(error, "gestionAppro", "Erreur de connexion !");
			exit(EXIT_FAILURE);
		}

		sprintf(req, "%s%s%s", MSG_CHARIOT, MSG_DELIMITER, MSG_QUEST);
		logClientCOL3(debug, "gestionAppro", "req1: %s", req);

		if (envoiMessageCOL3_s(socket, req) == -1)
		{
			logClientCOL3(error, "gestionAppro", "Erreur d'envoie de message !");
			exit(EXIT_FAILURE);
		}

		if (lireMessageCOL3_s(socket, &resp) == -1)
		{
			logClientCOL3(error, "gestionAppro", "Erreur reception message !");
			exit(EXIT_FAILURE);
		}

		logClientCOL3(debug, "gestionAppro", "Reponse1: %s", resp);

		if (strcmp(resp, MSG_CHARIOT_OK) == 0)
		{
			sprintf(req, "%s%s%d", MSG_CHARIOT, MSG_DELIMITER, site);
			logClientCOL3(debug, "gestionAppro", "req2: %s", req);
			logClientCOL3(info, "gestionAppro", "Envoi d'un chariot vers le site %s(%d)", nossites[aleat].nomSite, site);
			logClientCOL3(info, "gestionAppro", "Le trajet va durer %ds", nossites[aleat].duree);

			if (envoiMessageCOL3_s(socket, req) == -1)
			{
				logClientCOL3(error, "gestionAppro", "Erreur d'envoie de message !");
				exit(EXIT_FAILURE);
			}

			if (lireMessageCOL3_s(socket, resp) == -1)
			{
				logClientCOL3(error, "gestionAppro", "Erreur reception message !");
				exit(EXIT_FAILURE);
			}

			logClientCOL3(debug, "gestionAppro", "Réponse2: %s", resp);

			tab_msg = split(resp, MSG_DELIMITER, 0);

			if (strcmp(tab_msg[0], MSG_STOP) == 0)
			{
				logClientCOL3(error, "gestionAppro", "Réponse2 : MSG_STOP");
				exit(EXIT_FAILURE);
			}
			else if (strcmp(tab_msg[0], MSG_MATIERE) == 0)
			{
				logClientCOL3(info, "gestionAppro", "Un chariot avec %dkg de %s est arrivé !)", atoi(tab_msg[3]), getMatiereName(atoi(tab_msg[1])));
				modifieStock(atoi(tab_msg[1]), atoi(tab_msg[3]));
				saveHutteClan();
			}
		}
		else if (strcmp(resp, MSG_STOP) == 0)
		{
			logClientCOL3(error, "gestionAppro", "Réponse1: MSG_STOP");
			exit(EXIT_FAILURE);
		}
		else
		{
			logClientCOL3(error, "gestionAppro", "UNKNOWN ERROR");
			exit(EXIT_FAILURE);
		}

		close(socket);
		free(tab_msg);
	}

	return NULL;
}

void monAfficheHutte()
{

	pthread_mutex_lock(&mutex_prio_hutte);
	pthread_mutex_lock(&mutex_lect_hutte);

	if (++nbLecteur_huttes == 1)
		pthread_mutex_lock(&mutex_red_hutte);

	pthread_mutex_unlock(&mutex_lect_hutte);
	pthread_mutex_unlock(&mutex_prio_hutte);

	afficheHutte(HUTTECLAN);

	pthread_mutex_lock(&mutex_lect_hutte);

	if (--nbLecteur_huttes == 0)
		pthread_mutex_unlock(&mutex_red_hutte);

	pthread_mutex_unlock(&mutex_lect_hutte);
}

void *merlin_syncronisateur()
{
	int i = 0;
	// todo : save uniquement si variable inchangé
	while (1)
	{
		sleep(10);

		pthread_mutex_lock(&mutex_lect_hutte);
		saveHutteClan();
		pthread_mutex_unlock(&mutex_lect_hutte);

		// On affiche toute les 20 secondes la hutte
		if ((i = !i))
			monAfficheHutte();
	}
}

void *pretresse(void *param)
{
	params_thread_pretresse *donnees = (params_thread_pretresse *)param;
	char req[TAILLE_MAX_MSG];
	char resp[TAILLE_MAX_MSG];
	int socket;
	const char *func_name = donnees->MSG_QUEST == MSG_QUEST_FEU ? "pretresse (feu)" : "pretresse (guerre)";

	sprintf(req, "%s%s%s", MSG_HUTTE, MSG_DELIMITER, donnees->MSG_QUEST);

	logClientCOL3(debug, func_name, "Démarrage pretresse");

	while (1)
	{
		sleep(5);

		socket = connexionServeurCOL3(donnees->adresseip, donnees->port, donnees->tokenduclan, donnees->nomduclan);

		if (socket == INVALID_SOCKET)
		{
			logClientCOL3(error, func_name, "Erreur de connexion !");
			exit(EXIT_FAILURE);
		}

		if (envoiMessageCOL3_s(socket, req) == -1)
		{
			logClientCOL3(error, func_name, "Erreur d'envoie de message !");
			exit(EXIT_FAILURE);
		}

		if (lireMessageCOL3_s(socket, &resp) == -1)
		{
			logClientCOL3(error, func_name, "Erreur reception message !");
			exit(EXIT_FAILURE);
		}

		if (strcmp(resp, MSG_STOP) == 0)
		{
			logClientCOL3(error, func_name, "Réponse1: MSG_STOP");
			exit(EXIT_FAILURE);
		}
		else if (strcmp(resp, MSG_HUTTE_OK) == 0)
		{
			logClientCOL3(debug, func_name, "Réponse1: MSG_HUTTE_OK");
		}
		else
		{
			logClientCOL3(error, func_name, "UNKNOWN ERROR");
			exit(EXIT_FAILURE);
		}

		// MSG_HUTTE_OK est envoyé par le serveur, on peut lire le reste du message

		pthread_mutex_lock(&mutex_prio_hutte);
		pthread_mutex_lock(&mutex_lect_hutte);

		if (++nbLecteur_huttes == 1)
			pthread_mutex_lock(&mutex_red_hutte);

		pthread_mutex_unlock(&mutex_lect_hutte);
		pthread_mutex_unlock(&mutex_prio_hutte);

		// traitement
		if (envoiStructureCOL3_s(socket, &HUTTECLAN, sizeof(HUTTECLAN)) == -1)
		{
			logClientCOL3(error, func_name, "Erreur d'envoie de structure !");
			exit(EXIT_FAILURE);
		}

		pthread_mutex_lock(&mutex_lect_hutte);

		if (--nbLecteur_huttes == 0)
			pthread_mutex_unlock(&mutex_red_hutte);

		pthread_mutex_unlock(&mutex_lect_hutte);

		// MSG_HUTTE_OK est envoyé par le serveur, on peut lire le reste du message

		if (lireMessageCOL3_s(socket, &resp) == -1)
		{
			logClientCOL3(error, func_name, "Erreur reception message !");
			exit(EXIT_FAILURE);
		}

		if (strcmp(resp, MSG_STOP) == 0)
		{
			logClientCOL3(error, func_name, "Hutte invalide");
			exit(EXIT_FAILURE);
		}
		else if (strcmp(resp, MSG_HUTTE_OK) == 0)
		{
			logClientCOL3(info, func_name, "Hutte valide");
		}
		else
		{
			logClientCOL3(error, func_name, "UNKNOWN ERROR");
			exit(EXIT_FAILURE);
		}
	}

	return NULL;
}

void demarreForges()
{
	pthread_t threadForgeBLE[MAX_FORGE_BLE];
	pthread_t threadForgeBLO[MAX_FORGE_BLO];

	for (int i = 0; i < MAX_FORGE_BLE; i++)
	{
		pthread_create(&threadForgeBLE[i], NULL, forgerBLE, NULL);
		pthread_detach(threadForgeBLE[i]);
	}

	// for (int i = 0; i < MAX_FORGE_BLO; i++)
	// {
	// 	pthread_create(&threadForgeBLO[i], NULL, forgerBLO, NULL);
	// 	pthread_detach(threadForgeBLO[i]);
	// }
}

void forgerBLE()
{

	while (1)
	{
		if (stockOkPourBLE())
		{
			// Fabrication de la baliste légere
			utiliseRessourcesPourBLE();
			sleep(TPS_FAB_BLE);
			logClientCOL3(info, "forgerBLE", "Baliste légere forgé !");
		}
	}
}

int stockOkPourBLE()
{
	return lisStock(bois) >= MATERIAUX_BALISTE[(int)BLE][bois] &&
		   lisStock(salpetre) >= MATERIAUX_BALISTE[(int)BLE][salpetre] &&
		   lisStock(charbon) >= MATERIAUX_BALISTE[(int)BLE][charbon] &&
		   lisStock(soufre) >= MATERIAUX_BALISTE[(int)BLE][soufre] &&
		   lisStock(fer) >= MATERIAUX_BALISTE[(int)BLE][fer] &&
		   lisStock(chanvre) >= MATERIAUX_BALISTE[(int)BLE][chanvre];
}

void utiliseRessourcesPourBLE()
{
	modifieStock(bois, -MATERIAUX_BALISTE[(int)BLE][bois]);
	modifieStock(salpetre, -MATERIAUX_BALISTE[(int)BLE][salpetre]);
	modifieStock(charbon, -MATERIAUX_BALISTE[(int)BLE][charbon]);
	modifieStock(soufre, -MATERIAUX_BALISTE[(int)BLE][soufre]);
	modifieStock(fer, -MATERIAUX_BALISTE[(int)BLE][fer]);
	modifieStock(chanvre, -MATERIAUX_BALISTE[(int)BLE][chanvre]);
}

/*  ======================================
	  fonction de test d'échange initiale
	  avec le serveur
	=====================================*/

int testServeur(const char *adresseip, int port, const char *tokenduclan, const char *nomduclan)
{

	int socket;

	logClientCOL3(info, "test",
				  "le clan[%s] crée une socket pour tester le serveur",
				  nomduclan);

	/* -----------------------------
	   ECHANGE 1 : envoi du token de test
	   ----------------------------- */

	/* creation et connexion au serveur de socket */
	socket = connexionServeurCOL3(adresseip, port, tokenduclan, nomduclan); // on met MSG_TEST à la place du TOKEN

	/* -----------------------------
	   ECHANGE 2 : valildation echange
	   ----------------------------- */
	if (socket != INVALID_SOCKET)
	{

		logClientCOL3(info, "test",
					  "le clan[%s] a validé son test de connexion  %b ",
					  nomduclan, debug_ok);
		close(socket);
	}
	else
	{
		logClientCOL3(error, "test",
					  "le clan[%s] n'a pas validé son test de connexion  %b ",
					  nomduclan, debug_nok);
	}

	return socket;
}
