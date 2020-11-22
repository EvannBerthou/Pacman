#include "path_finding.h"
#define max_size 1000

void find_path(Partie* p,Pos depart, Pos arriver,Entite* F){
    int liste_nb_choix[max_size]={0};
    int nb_openlist=0;
    int nb_closelist=0;
    int nb_voisin;
    int nb_tour=0;
    int way_find=0;
    Noeud liste_ouverte[max_size]={0};
    Noeud liste_fermer[max_size]={0};
    // initialisation des noeud principaux
    Noeud Noeud_depart={depart,NULL};
    Noeud Noeud_arriver={arriver,NULL};
    // initialisation des liste de noeud 
    liste_ouverte[0]=Noeud_depart;
    liste_fermer[0]=Noeud_depart;
    liste_nb_choix[0]+= 1;
    nb_openlist++;
    nb_closelist++;
    // si pacman en dehors de la  grille ne pas faire de path_finding 
    if ((arriver.c>(p->C-1)) || (arriver.c<0) || (arriver.l>(p->L-1)) || (arriver.l<0)){
        way_find=1;   
    }
    while(!way_find){
        nb_voisin=nb_openlist;	
        // chaque noeud ajouter rechercher les voisin 	
        for(int v = liste_nb_choix[nb_tour]; v >= 0; v--){
            // Si trop de test, alors dire qu'aucun chemin n'a été trouvé
            if (v > 1000) {
                F->prochain_noeud = NULL;
                return;
            }

            if  ((liste_fermer[nb_closelist-v].pos.l==Noeud_arriver.pos.l) && (liste_fermer[nb_closelist-v].pos.c == Noeud_arriver.pos.c)){
                way_find = 1;
                get_path(&(liste_fermer[nb_closelist-v]),nb_tour,F);
                break;
            }
            nb_openlist += recherche_voisin(p,&(liste_fermer[nb_closelist-v]),liste_fermer,liste_ouverte,&Noeud_depart,&Noeud_arriver,nb_openlist,nb_closelist);		
        }
        if (way_find){
            break;
        }
        //recherche ajout de tout les voisins   
        nb_voisin = nb_openlist - nb_voisin;
        for (int k = (nb_openlist - nb_voisin); k < nb_openlist; k++) {
            liste_fermer[nb_closelist] = liste_ouverte[k];
            nb_closelist += 1;
            liste_nb_choix[nb_tour + 1] += 1;
        }
        nb_tour += 1;
    }
}

//recuperation de la filiation entre les noeud parents
void get_path(Noeud* last_noeud,int nb_tour,Entite* F){
    Noeud* local=last_noeud;
    int index = nb_tour;

    while (local != NULL) {
        index--;
        if (index <= 0) {
            break;
        }
        local = local->parent;
    }
    F->prochain_noeud = local;
}

static int sur_grille(Partie *p, Pos pos) {
    return (pos.l >= 0 && pos.c >= 0 && pos.l < p->L && pos.c < p->C);
}

int recherche_voisin(Partie* p,Noeud* current_noeud,Noeud liste_fermer[],Noeud liste_ouverte[],Noeud* Noeud_depart,Noeud* Noeud_arriver,int nb_openlist,int nb_closelist){
    int directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
    int nb_voisin = 0;
    for (int i = 0; i < 4; i++){
        Pos position = {current_noeud->pos.l + directions[i][1], current_noeud->pos.c + directions[i][0]};
        if (!sur_grille(p, position)) continue;
        if (p->plateau[position.l][position.c] != '*' && !(inliste(position, liste_fermer,nb_closelist)) && !(inliste(position, liste_ouverte, nb_openlist))) {
            liste_ouverte[nb_openlist] = (Noeud) {
                position,
                current_noeud
            };
            nb_openlist += 1;
            nb_voisin += 1;
        }
    }
    return nb_voisin;
}

int inliste(Pos element,Noeud liste_fermer[],int nb_closelist) {
    for (int n = 0; n < nb_closelist; n++){
        if (element.c==liste_fermer[n].pos.c && element.l==liste_fermer[n].pos.l){
            return 1;
        }
    }
    return 0;
}
int distance(Pos pointA,Pos pointB){
    int result=abs(pointB.c - pointA.c) + abs(pointB.l - pointA.l);
    return result;
}