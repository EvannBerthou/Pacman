#include "fantome.h"
#include <unistd.h>
#include <math.h>

void bouger_fantomes(Partie *p, float dt) {
    for (int i = 0; i < NBFANTOMES; i++) {
        Entite *fantome = &p->fantomes[i];   
        Pos current_pos=ecran_vers_grille(fantome->pos,p->tc);
        printf("\n");
        if (fantome->etat.fuite && ((fantome->pos.l == fantome->pos_init.l) && (fantome->pos.l == fantome->pos_init.l))){
            fantome->etat.fuite=0;
        }
        if (fantome->etat.fuite){
            find_path(p,current_pos,ecran_vers_grille(fantome->pos_init,p->tc),fantome);
        }
        else if ((fantome->pos_cible.l != p->pacman.pos.l) || (fantome->pos_cible.c!=p->pacman.pos.c)){
            printf("bob\n");
            if (fantome->type==ENTITE_FANTOME_R){
                fantome->etat.suiv=0;
                fantome->pos_cible=p->pacman.pos;
                find_path(p,current_pos,ecran_vers_grille(p->pacman.pos,p->tc),fantome);
                printf("follow: %d Noeud: %p Noeud2: %p\n",fantome->etat.suiv,fantome->chemin_noeud[fantome->etat.suiv],fantome->chemin_noeud[fantome->etat.suiv+1] );            
            }
        }
        else {
             
            if (((float)(fantome->chemin_noeud[fantome->etat.suiv]->pos.l*p->tc.l)<=fantome->pos.l && fantome->pos.l<(float)(fantome->chemin_noeud[fantome->etat.suiv+1]->pos.l*p->tc.l)) || ((float)(fantome->chemin_noeud[fantome->etat.suiv+1]->pos.l*p->tc.l)<fantome->pos.l && fantome->pos.l<=(float)(fantome->chemin_noeud[fantome->etat.suiv]->pos.l*p->tc.l))
                ||((float)(fantome->chemin_noeud[fantome->etat.suiv]->pos.c*p->tc.c)<=fantome->pos.c && fantome->pos.c<(float)(fantome->chemin_noeud[fantome->etat.suiv+1]->pos.c*p->tc.c)) || ((float)(fantome->chemin_noeud[fantome->etat.suiv+1]->pos.c*p->tc.c)<fantome->pos.c && fantome->pos.c<=(float)(fantome->chemin_noeud[fantome->etat.suiv]->pos.c*p->tc.c))){
                    //printf(" : %d pos x: %d y: %d  Noeud pos x: %d y: %d\n",b,current_pos.c,current_pos.l,fantome->chemin_noeud[b]->pos.c,fantome->chemin_noeud[b]->pos.l);
                fantome->etat.suiv+=1;

            }
            //fantome->etat.suiv+=1;
            
        }

        printf("// Verifie quelle direction est la bonne follow: %d\n ",fantome->etat.suiv); 
        //       printf("// Verifie quelle direction est la bonne\n follow: %d  Noeud: %d ",fantome->etat.follow,fantome->chemin_noeud[fantome->etat.follow]); 
     
        DirEntite direction = DIR_INCONNUE;
        printf("Pos x: %f y: %f  Noeud pos x: %f y: %f  \n",fantome->pos.c,fantome->pos.l,(float)(fantome->chemin_noeud[fantome->etat.suiv]->pos.c*p->tc.c),(float)(fantome->chemin_noeud[fantome->etat.suiv]->pos.l*p->tc.l));
        if (fantome->pos.l> (float)(fantome->chemin_noeud[fantome->etat.suiv]->pos.l*p->tc.l)){
            direction = DIR_HAUT;
            printf(" DIR_HAUT \n");}
        else if (fantome->pos.l <(float)(fantome->chemin_noeud[fantome->etat.suiv]->pos.l*p->tc.l)){
            direction = DIR_BAS;
            printf(" DIR_BAS \n");}
        else if (fantome->pos.c > (float)(fantome->chemin_noeud[fantome->etat.suiv]->pos.c*p->tc.c)){
            direction = DIR_GAUCHE;
            printf(" DIR_GAUCHE \n");}
        else if (fantome->pos.c < (float)(fantome->chemin_noeud[fantome->etat.suiv]->pos.c*p->tc.c)){
            direction = DIR_DROITE;
            printf(" DIR_DROITE \n");}
        // Change la direction du fantôme ssi il est aligné à la grille pour éviter qu'il traverse les murs
        if (aligne_grille(p, fantome->pos)) {
            fantome->etat.direction = direction;
        }

        // Déplace le fantôme dt * vitesse
        const float vitesse = 50;
        switch (fantome->etat.direction) {
            case DIR_HAUT:   fantome->pos.l-=dt * vitesse; break;
            case DIR_BAS:    fantome->pos.l+=dt * vitesse; break;
            case DIR_GAUCHE: fantome->pos.c-=dt * vitesse; break;
            case DIR_DROITE: fantome->pos.c+=dt * vitesse; break;
        }
    }
}

void dessiner_fantomes(Partie *p) {
    for (int i = 0; i < NBFANTOMES; i++) {
        Entite *fantome = &p->fantomes[i];
        Point pos = {fantome->pos.c, fantome->pos.l};
        if (fantome->etat.direction == DIR_INCONNUE) 
            afficher_surface(fantome->sprite[1][0], pos);
        else
            afficher_surface(fantome->sprite[fantome->etat.direction][0], pos);
    }
}
  //   if (current_pos.c == fantome->chemin_noeud[b]->pos.c && current_pos.l == fantome->chemin_noeud[b]->pos.l ){
    //                if ((abs(fantome->pos.l-(float)(fantome->chemin_noeud[b]->pos.l*p->tc.l))<p->tc.l) && (abs(fantome->pos.c-(float)(fantome->chemin_noeud[b]->pos.c*p->tc.c))<p->tc.c)){
