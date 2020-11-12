
#ifndef PATH_FINDING_H
#define PATH_FINDING_H

#include "partie.h"
#include "entite.h"

void find_path(Partie *p,Pos depart,Pos arriver,Entite* F);
void get_path(Noeud* last_noeud,int nb_tour,Entite* F);
int recherche_voisin(Partie* p,Noeud* current_noeud,Noeud liste_fermer[],Noeud  liste_ouverte[],Noeud* Noeud_depart,Noeud* Noeud_arriver,int nb_openlist,int nb_closelist);
int find_min(Noeud liste_ouverte[],int nb_openlist , int nb_voisin);
int distance(Pos pointA, Pos pointB);
int inliste(Pos element,Noeud liste_fermer[],int nb_closelist);
#endif
