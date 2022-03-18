#ifndef CLIENTCOL3_H_INCLUDED
#define CLIENTCOL3_H_INCLUDED

#include "col3-bibtp/communCOL3-TP.h"


extern hutte HUTTECLAN;


const char *getMatiereName(matieres_premieres mat);
void afficheRessources(lessitesdumonde nossite);
void recupSiteExtraction( int socket, lessitesdumonde nossites);
void gestionAppro( int socket, lessitesdumonde nossites);
int testServeur( const char * adresseip, int port,const char * tokenduclan,const char * nomduclan);



#endif // CLIENTCOL3_H_INCLUDED
