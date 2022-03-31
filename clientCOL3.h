#ifndef CLIENTCOL3_H_INCLUDED
#define CLIENTCOL3_H_INCLUDED

#include "col3-bibtp/communCOL3-TP.h"

extern hutte HUTTECLAN;
extern pthread_mutex_t mutex_lect_hutte; // mutex pour la protection de la hutte
extern pthread_mutex_t mutex_red_hutte;  // mutex pour la protection de la hutte
extern pthread_mutex_t mutex_prio_hutte; // mutex pour la protection de la hutte
extern int nbLecteur_huttes;             // nombre de lecteurs sur la hutte
extern pthread_t thread_merlin;
extern pthread_t thread_pretresse_feu;
extern pthread_t thread_pretresse_guerre;

// structure contenant un site_extraction et les paramètres de la socket
typedef struct
{
    const char *adresseip;
    int port;
    const char *tokenduclan;
    const char *nomduclan;

} params_thread_gestionAppro;

typedef struct
{
    const char *adresseip;
    int port;
    const char *tokenduclan;
    const char *nomduclan;

    const char *MSG_QUEST;

} params_thread_pretresse;

extern params_thread_pretresse feu;
extern params_thread_pretresse guerre;

const char *getMatiereName(matieres_premieres mat);
void afficheRessources(lessitesdumonde nossite);
void recupSiteExtraction(int socket, lessitesdumonde nossites, int *nb_chariots);
void envoieChariots(const char *adresseip, int port, const char *tokenduclan, const char *nomduclan);
void loadHutteClan();
void saveHutteClan();
void loadHutteClan();
int modifieStock(matieres_premieres matiere, int nb);
int lisStock(matieres_premieres matiere);
int estQuantiteValide(matieres_premieres matiere, int quantite);
void *gestionAppro(void *params);
void *merlin_syncronisateur();
void *pretresse(void *param);
int testServeur(const char *adresseip, int port, const char *tokenduclan, const char *nomduclan);
int nbSites(lessitesdumonde nossites);
void demarreForges();
void forgerBLE();
int stockOkPourBLE();
void utiliseRessourcesPourBLE();

#endif // CLIENTCOL3_H_INCLUDED
