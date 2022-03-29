#ifndef CLIENTCOL3_H_INCLUDED
#define CLIENTCOL3_H_INCLUDED

#include "col3-bibtp/communCOL3-TP.h"

extern hutte HUTTECLAN;

// structure contenant un site_extraction et les paramètres de la socket
typedef struct
{
    const char *adresseip;
    int port;
    const char *tokenduclan;
    const char *nomduclan;
    site_extraction site;

} params_thread_gestionAppro;

const char *getMatiereName(matieres_premieres mat);
void afficheRessources(lessitesdumonde nossite);
void recupSiteExtraction(int socket, lessitesdumonde nossites, int *nb_chariots);
void envoieChariots(const char *adresseip, int port, const char *tokenduclan, const char *nomduclan);
void *gestionAppro(void *params);
int testServeur(const char *adresseip, int port, const char *tokenduclan, const char *nomduclan);

#endif // CLIENTCOL3_H_INCLUDED
