//           communCOL3.h
//  dim. mars 24 06:35:44 2019
//  Copyright  2019  pernelle
//  <user@host>
/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * lib.h
 * Copyright (C) 2019 Pernelle <philippe.pernelle@gmail.com>
 * 
 * col3 is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * col3 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*************************
   Clash of L3 - 2018
    (ne pas modifier)
**************************/

#ifndef COMMUNCOL3_H_INCLUDED
#define COMMUNCOL3_H_INCLUDED

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define TAILLE_MAX 4196

#define MAX_CLAN 10
#define MAX_SITE_EXTRACTION 25
#define MAX_CAMP 45
#define MAX_CHARIOT 10
#define MAX_CHARIOT_PAR_CLAN 5

#define TAILLE_MAX_NOM_CLAN 20
#define TAILLE_MAX_NOM_SITE 20
#define TAILLE_MAX_MSG 512
#define TAILLE_MAX_ARMEE 15

#define MAX_FORGE_BLE 3
#define MAX_FORGE_BLO 2

#define TPS_FAB_BLO 5
#define TPS_FAB_BLE 2

#define PVIE_CLAN 100

#include <netinet/in.h>


extern const char MSG_HUTTE[];		/* code pour un message de type HUTTE */
extern const char MSG_SITE[];		/* code pour un message de type SITE EXTRACTION */
extern const char MSG_CLAN[];
extern const char MSG_NOMCLAN[];
extern const char MSG_STOP[];
extern const char MSG_LONGITUDE[];
extern const char MSG_LATITUDE[];
extern const char MSG_DUREE[];
extern const char MSG_QUANTITE[];
extern const char MSG_MATIERE[];
extern const char MSG_DELIMINTER[];
extern const char MSG_DELIMINTERARMEE[];
extern const char MSG_DELIMINTERSITE[];
extern const char MSG_QUEST[];
extern const char MSG_TEST[];		/* code pour tester la communication avec le serveur */

extern const char MSG_TOKEN_OK[];		//2019
extern const char MSG_TOKEN_NOK[];		//2019
extern const char MSG_NOMCLAN_OK[];		//2019
extern const char MSG_NOMSITE[];        //2019
extern const char MSG_CHARIOT[];
extern const char MSG_CAMP[];
extern const char MSG_CHARIOT_OK[];
extern const char MSG_CHARIOT_NOK[];

extern const char MSG_GTK_MONDE_OK[];		/* code de validation MONDE recu par client GTK */
extern const char MSG_GTK_BATAILLE_OK[]; 	/* code de validation BATAILLE recue par client GTK */

extern const char MSG_FICHIER_ARMEE[];
extern const char MSG_FICHIER_ARMEE_BALISTE[];


extern const char MSG_STATUT_ARMEE[];
extern const char MSG_STATUT_ARMEE_ENFAB[];
extern const char MSG_STATUT_ARMEE_PRET[];

extern int NCURSE ;

extern const int DEGAT_BALISTE[2]; 			/* degat des balistes BLE et BLO */
extern const int PRECISION_BALISTE[2]; 		/* précision de tir des balistes BLE et BLO */
extern const int PVIE_BALISTE[2];			/* point de vie des balistes BLE et BLO */
extern const int MATERIAUX_BALISTE[2][6]; 	/* matériaux pour la construction des 
 											   balistes BLE et BLO */

extern const int POSITION_EXTRACTION[2][MAX_SITE_EXTRACTION];	/* position des sites d'extraction */
extern const int POSITION_CAMP[2][MAX_CAMP];					/* position des camps */

/* type de demande envoyé entre client et serveur */
typedef enum {nom=0, statutarmee=1, fichierarmee=2, chariot=3, sites=4 , mondev=5, bataillev=6} type_demande;

/* type de matiere premiere */
typedef enum {bois=0, salpetre=1, charbon=2, soufre=3, fer=4, chanvre=5} matieres_premieres;

/* type de chariot et capacite */
typedef enum {lourd=12, leger=5} type_chariot;

/* type de balise (BLO= Baliste LOurde, BLE=Baliste LEgere) */
typedef enum {BLO=1,BLE=0} type_baliste;

/* type tp */
typedef enum {tp1,tp2,tp3} type_serveurCOL3;

/* type de question pour un tp */
typedef enum {q1,q2,q3} type_questionCOL3;


/* structure d'une armmée composé de baliste  */
typedef struct {
    int forge[TAILLE_MAX_ARMEE];			/* num forge ou est produite la baliste*/
    type_baliste baliste[TAILLE_MAX_ARMEE]; /* type de baliste */
    int pvie[TAILLE_MAX_ARMEE];				/* point de vie de la baliste */
    int nbbaliste;							/* nb de baliste produite */
} armee;

/* structure préparatoire d'une attaque (recu par chaque clan avant attaque) 
   a recevoir par le serveur */
typedef struct {
    int dispoBaliste[2];							     /* nb de baliste disponible BLO et BLE pour l'attaque */
    type_baliste balisterestante[TAILLE_MAX_ARMEE];      /* type de chaque baliste restante pour l'attaque */
    int pvieBaliste[TAILLE_MAX_ARMEE];                   /* point de vie de chaque baliste restante pour l'attaque */
    int nbcl;										     /* nb clan restant */
    int pvieClan;									     /* point de vie du clan */
    char listeClan[MAX_CLAN][TAILLE_MAX_NOM_CLAN] ; /* liste des nom de clan srestant */

} preAttaque;

/* structure d'une attaque : nb baliste et id clan attaqué (-1 pour utiliser toutes
   les balistes en défense ) 
   à envoyer au serveur */
typedef struct {
    int balisteAttaque[2];     /* nb de baliste attaquantes par type (BLO et BLE) */
    int idClan;                /* identifiant du clan attaqué */
} attaque;



/* définition d'un site extraction */
typedef struct {
    char nomSite[TAILLE_MAX_NOM_SITE];
	int idSite;
    int longitude;
    int latitude;
    int quantite;
    matieres_premieres matiere;
    int duree;
} site_extraction;

/* liste des sites d'extractions du monde CoL3   */
typedef site_extraction lessitesdumonde[MAX_SITE_EXTRACTION];

/* structure indiquant la capacité d'extraction d'un clan */
typedef struct {
	int idClan;
	char nomClan[TAILLE_MAX_NOM_CLAN];
    lessitesdumonde sitesAccessibles;
    int nbChariotDisponible;
} capacite_clan;


/* définition d'un camp */
typedef struct {
	int idCamp;						/* identifiant du camp */
    int longitude;					/* position sur la carte */
    int latitude;					/* position sur la carte */
	capacite_clan capaciteEnCours;  /* capacité d'extraction du clan 
	                                   qui a prit ce camp */
} camp;

/* définition de la liste des camps du monde CoL3   */
typedef camp lescampsdumonde[MAX_CAMP];


/* structure d'une hutte de stockage  */
typedef struct {
    char nomClanHutte[TAILLE_MAX_NOM_CLAN];   /* nom du clan */
    int stock[6];							  /* quantité par matiere */	
    time_t tps_debut;						  /* date premier remplissage */
    time_t tps_fin;                           /* date fin remplissage */
} hutte;

typedef enum {sarmee, stourAttaque, spreAttaque} type_structure;

/*  ------------------------------------------------
    structure des VUES
    ------------------------------------------------*/

/* Cette structure décrit la vue d'un CLAN */
typedef struct {
	int idClan;
    int longitude;
    int latitude;	
	int pVie;
    char nom[TAILLE_MAX_NOM_CLAN];
} clanVue;

/* Cette structure décrit la vue du MONDE */
typedef struct {
    int portServ;
    char addrServ[20];
    clanVue clans[MAX_CLAN];
    int nbclan;
} mondeVue;

/* cette structure décrit la vue d'une BATAILLE-*/
typedef struct { 
    int debutCombat;
	int finCombat;
    int numTour;
    int nbClanEnBataille;
    attaque lesAttaques[MAX_CLAN];
    int pvieClan[MAX_CLAN];
	char nomClan[MAX_CLAN][TAILLE_MAX_NOM_CLAN];
    armee lesarmees[MAX_CLAN];
} batailleVue;


/*  ------------------------------------------------
    fonctions de connexion et d'echange de message
    avec le serveur CoL3
    ------------------------------------------------*/

/**
    cette fonction crée une socket et tente une connexion TCP à un serveur COL3. 
    si la connexion TCP est validée, la fonction echange avec le serveur pour valider
    le tocken du client 

    entrées : 
     - adresse IP du serveur à connecter
     - port du serveur à connecter
     - token id client 

    retour:
     - valeur de la socket créé (meme code que socket())
 
     p.pernelle / d.wayntal - 2019
*/
int connexionServeurCOL3(const char *adresse_serveur,int port, const char *token) ;
/**
    cette fonction lit un message en TCP (msgrecu) sur la socket (sock)
	
	entrées : 
     - sock : numéro de la socket
     - msgrecu : chaine recue
 
     p.pernelle / d.wayntal - 2019
*/
void lireMessageCOL3(int sock, char * msgrecu);
/**
    cette fonction envoi un message (msgresponse) par la socket (sock)

	entrées : 
     - sock : numéro de la socket
     - msgresponse : chaine à envoyé
 
     p.pernelle / d.wayntal - 2019
*/
void envoiMessageCOL3(int sock, const char * msgresponse);


/*  ------------------------------------------------
    fonctions de connexion et d'echange de structure
    avec le serveur CoL3
    ------------------------------------------------*/

int testServeur(const char * nomduclan, const char * adresseip, int port);
void envoiStructureCOL3(int sock, void * mastructure,int taille);
int lireArmeeCOL3(int sock, armee * monarmee);
int lirePreAttaqueCOL3(int sock, preAttaque * mapreattaque);
int lireAttaqueCOL3(int sock, attaque * monattaque);
int lireHutteCOL3(int sock, hutte * mahutte);
int lireSitesDuMondeCOL3(int sock, capacite_clan * messites);
int lireBatailleVueCOL3(int sock, batailleVue * mabataille);
int lireMondeVueCOL3(int sock, mondeVue * monmonde);


/*  ------------------------------------------------
    fonctions d'afichage
    ------------------------------------------------*/

/**
    cette procedure affiche la hutte d'un clan
 
    p.pernelle / d.wayntal - 2019
*/
void afficheHutte(hutte mahutte);
/**
    cette procedure affiche l'armee d'un clan c'est à dire
 	l'ensemble de ses balistes

    p.pernelle / d.wayntal - 2019
*/
void afficheArmee(armee monarmee);
/**
    cette procedure affiche la preparation d une attaque c'est à dire
    les balistes restantes et les clans restants

    p.pernelle / d.wayntal - 2019
*/
void affichePreAttaque(preAttaque mapreattaque);
/**
    cette procedure affiche la configuration d attaque choisit par un clan

    p.pernelle / d.wayntal - 2019
*/
void afficheAttaque(attaque monattaque);
/**
    cette procedure affiche la capacité d'extraction d'un clan clan

    p.pernelle / d.wayntal - 2019
*/
void afficheSiteExtractionClan(capacite_clan capa) ;

/*  ------------------------------------------------
    fonctions de gestion des parametres des TP
    ------------------------------------------------*/

/**
    fonction de conversion type_serveurCOL3 --> string

    p.pernelle / d.wayntal - 2019
*/
char* typeServeurCOL3toString(type_serveurCOL3 tp);
/**
    fonction de conversion string --> type_serveurCOL3

    p.pernelle / d.wayntal - 2019
*/
type_serveurCOL3 stringtotypeServeurCOL3(char tp[]);
/**
    fonction de conversion type_serveurCOL3 --> string

    p.pernelle / d.wayntal - 2019
*/
char* typeQuestionCOL3toString(type_questionCOL3 qn);
/**
    fonction de conversion string --> type_serveurCOL3

    p.pernelle / d.wayntal - 2019
*/
type_questionCOL3 stringtotypeQuestionCOL3(char qn[]);


/*  ------------------------------------------------
    fonctions de tests des messages echangés
    ------------------------------------------------*/

int estDemandeConforme(type_demande demande,const char * msg);
int estReponseConforme(type_demande demande, const char * valeurReponse, const char * msg);


/*  ------------------------------------------------
    fonctions de conversion message (chaine de texte)
	en structure
    ------------------------------------------------*/

/**
    cette fonction convertit un message en strucure de type hutte 

    p.pernelle / d.wayntal - 2019
*/
int messageToHutte(const char * msg, hutte * mahutte);
/**
    cette fonction convertit un message en strucure de type 
	site extraction

    p.pernelle / d.wayntal - 2019
*/
int messageToSiteExtraction(const char * msg, site_extraction * site);
/**
    cette fonction convertit une strucure de type hutte
	en un message

    p.pernelle / d.wayntal - 2019
*/
void hutteToMessage(hutte * mahutte,  char * msg);
/**
    cette fonction convertit une strucure de type site extraction
	en un message

    p.pernelle / d.wayntal - 2019
*/
void SiteExtractionToMessage(site_extraction * site,  char * msg, const char * nomClan);

/*  ------------------------------------------------
    fonctions d'accès aux fichiers
    ------------------------------------------------*/

void lectureFichierHutte (hutte * mahutte, char path[]);

/*  ------------------------------------------------
    fonctions génériques
    ------------------------------------------------*/

/** source : www.code-source.com
    Retour tableau des chaines recupérer. Terminé par NULL.
    chaine : chaine à splitter
    delim : delimiteur qui sert à la decoupe
    vide : 0 : on n'accepte pas les chaines vides
           1 : on accepte les chaines vides
*/
char** split(char* chaine,const char* delim,int vide);



#endif // COMMUNCOL3_H_INCLUDED

