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

	pthread_t thread[nb_chariots];
	params_thread_gestionAppro params[nb_chariots];
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
			logClientCOL3(error, "gestionAppro", "MSG_STOP");
			exit(EXIT_FAILURE);
		}
		else if (strcmp(tab_msg[0], MSG_MATIERE) == 0)
		{
			logClientCOL3(info, "gestionAppro", "mat=%s qt=%s", getMatiereName(atol(tab_msg[1])), tab_msg[3]);
		}
	}
	else if (strcmp(resp, MSG_STOP) == 0)
	{
		logClientCOL3(error, "gestionAppro", "MSG_STOP");
		exit(EXIT_FAILURE);
	}
	else
	{
		logClientCOL3(error, "gestionAppro", "UNKNOWN ERROR");
		exit(EXIT_FAILURE);
	}

	close(socket);
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
