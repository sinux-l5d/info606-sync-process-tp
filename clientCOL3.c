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

void recupSiteExtraction(int socket, lessitesdumonde nossites, int *nb_chariots)
{

	int msg_size = strlen(MSG_SITE) + strlen(MSG_DELIMITER) + strlen(MSG_QUEST);
	char buffer[msg_size];
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
		logClientCOL3(info, "recupSiteExtraction", "Connexion réussi !");

		lireStructureCOL3_s(socket, &resp, sizeof resp);

		logClientCOL3(info, "recupSiteExtraction", "id du clan: %d", resp.idClan);
		logClientCOL3(info, "recupSiteExtraction", "nombre de chariot du clan: %d", resp.nbChariotDisponible);
		*nb_chariots = resp.nbChariotDisponible;
		logClientCOL3(info, "recupSiteExtraction", "nom du clan: %s", resp.nomClan);
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

	recupSiteExtraction(socket, nossites, &nb_chariots);
	close(socket);

	pthread_t thread[MAX_CHARIOT_PAR_CLAN];
	params_thread_gestionAppro params[MAX_CHARIOT_PAR_CLAN];
	// pour chaque chariot disponible, on créer un thread
	for (int i = 0; i < nb_chariots; i++)
	{
		params[i].site = nossites[i];
		params[i].adresseip = adresseip;
		params[i].port = port;
		params[i].tokenduclan = tokenduclan;
		params[i].nomduclan = nomduclan;
		pthread_create(&thread[i], NULL, gestionAppro, (void *)&params[i]);
	}
	// on attend que les threads se terminent
	for (int i = 0; i < nb_chariots; i++)
	{
		if (pthread_join(thread[i], NULL) == -1)
		{
			logClientCOL3(error, "envoieChariots", "Erreur de join de thread !");
			exit(EXIT_FAILURE);
		}
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
	// TODO: Gerer ajout 1 par 1
	HUTTECLAN.stock[matiere] += nb;

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
	int site;
	int socket;
	params_thread_gestionAppro *donnees = (params_thread_gestionAppro *)params;
	site = donnees->site.idSite; // site à extraire

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
			logClientCOL3(info, "gestionAppro", "mat=%s qt=%s", getMatiereName(atoi(tab_msg[1])), tab_msg[3]);
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
	return NULL;
}

void *merlin_syncronisateur()
{
	// todo : save uniquement si variable inchangé
	while (1)
	{
		pthread_mutex_lock(&mutex_lect_hutte);
		saveHutteClan();
		pthread_mutex_unlock(&mutex_lect_hutte);
		sleep(10);
	}
}

void *pretresse(void *param)
{
	params_thread_pretresse *donnees = (params_thread_pretresse *)param;
	char req[TAILLE_MAX_MSG];
	char resp[TAILLE_MAX_MSG];
	const char *func_name = donnees->MSG_QUEST == MSG_QUEST_FEU ? "pretresse (feu)" : "pretresse (guerre)";

	int socket = connexionServeurCOL3(donnees->adresseip, donnees->port, donnees->tokenduclan, donnees->nomduclan);

	if (socket == INVALID_SOCKET)
	{
		logClientCOL3(error, func_name, "Erreur de connexion !");
		exit(EXIT_FAILURE);
	}

	sprintf(req, "%s%s%s", MSG_HUTTE, MSG_DELIMITER, donnees->MSG_QUEST);

	logClientCOL3(debug, func_name, "Démarrage pretresse");

	while (1)
	{
		sleep(5);

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
			logClientCOL3(info, func_name, "Réponse1: MSG_HUTTE_OK");
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
