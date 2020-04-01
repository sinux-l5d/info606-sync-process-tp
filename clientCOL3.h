#ifndef CLIENTCOL3_H_INCLUDED
#define CLIENTCOL3_H_INCLUDED

#include "col3-bibtp1/communCOL3-TP1.h"


extern hutte HUTTECLAN;


void recupSiteExtraction();
void gestionAppro();
int testServeur(const char * nomduclan, const char * adresseip, int port);

void ecritureHutteSansMutex(char path[]);
void lectureFichierHutteSansMutex (hutte * mahutte, char path[]) ;

#endif // CLIENTCOL3_H_INCLUDED
