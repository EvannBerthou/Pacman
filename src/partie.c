/******************************************************************************/
/* CHARGEMENT.c                                                               */
/******************************************************************************/
#include <assert.h>
#include "./partie.h"
#include "entite.h"
#include "pacman.h"
#include "fantome.h"

/******************************************************************************/
/* CHARGE PLAN                                                                */
/******************************************************************************/
Partie charge_plan(char *fichier)
    {
    Partie  p;
    FILE    *f = fopen(fichier,"r"); // Ouverture en lecture du fichier
    p.plateau = NULL;   // Le futur plan, à allouer dynamiquement.
    int     res, l, c;
    int     nbf;        // Nb de fantômes trouvés sur le plan
    int     nbb;        // Nb de bonus trouvés sur le plan
    char    ch;

    if(!f) // Si f== NULL, ouverture ratée
        {
        printf("Impossible d'ouvrir '%s'\n",fichier);
        exit(0);
        }

/* Lecture des dimensions du plan en en-tête                                  */
    res = fscanf(f,"%d %d\n",&p.L,&p.C); // Lecture de deux entiers

// Si on n'a pas pu lire deux entiers ou s'ils sont incorrects
    if(res !=2 || p.C<2 || p.L<2 || p.C >800 || p.L>600)
        {
        printf("Dimensions du tableau lues dans '%s' incorrectes\n",fichier);
        fclose(f);
        exit(0);
        }
    printf("Dimensions lues: %d x %d\n",p.L,p.C);

/* ALLOCATION DYNAMIQUE                                                       */
/* Allocation du tableau de *L pointeurs sur lignes                           */
    p.plateau = (char **) malloc(p.L * sizeof(char *));
    if(p.plateau == NULL)
        {
        printf("Allocation dynamique impossible\n");
        fclose(f);
        exit(0);
        }

/* Allocation des tableaux de *C caractères                                   */
    for(l=0;l!=p.L;l++)
        {
        p.plateau[l] = (char *) malloc(p.C * sizeof(char));
        if(p.plateau[l] == NULL)
            {
            printf("Allocation dynamique impossible\n");
            fclose(f);
            exit(0);
            }
        }


/* LECTURE DES LIGNES DU PLAN                                                 */
    l = 0;
    res = 0;
    nbf = 0;
    nbb = 0;
    while(res != EOF) // Lecture de chaque ligne
        {
        c=0;
        while(1)
            {
            res = fscanf(f,"%c",&ch); // Lecture d'un caractère
            if (res == EOF) // Si fin de fichier
                break; // Quittons la boucle interne

            if(c>p.C) // Si trop de colonnes...
                {
                printf("Ligne %d colonne %d: trop de colonnes\n",l,c);
                fclose(f);
                exit(0);
                }

            if(c==p.C) // Si fin de ligne supposée...
                {
                if(ch=='\n') // Si fin de ligne réelle, on quitte la boucle
                    {
                    break;
                    }
                else // Sinon trop de caractères
                    {
                    printf("Ligne %d: trop de caractères\n",l);
                    fclose(f);
                    //exit(0);
                    }
                }
/* ...sinon, nous ne sommes pas à la fin de la ligne.                         */
/* Si on lit un caractère interdit...                                         */
            if(ch!='.' && ch!=' ' && ch!= '*' && ch!='P' && ch!='F' && ch!='B' && ch!='C')
                {
                if(ch=='\n') // Si c'est un saut de ligne
                    printf("Ligne %d: trop peu de caractères\n",l);
                else
                    printf("Ligne %d: caractère '%c' incorrect\n",l,ch);

                fclose(f);
                exit(0);
                }

            if(ch=='P')
                {
                    p.pacman = nouvelle_entite((Pos){l,c}, ENTITE_PACMAN);
                }
            else if(ch=='F')
                {
                if(nbf>NBFANTOMES)
                    {
                    printf("Ligne %d:  un fantôme de trop!\n",l);
                    fclose(f);
                    exit(0);
                    }
                p.fantomes[nbf] = nouvelle_entite((Pos){l,c}, ENTITE_FANTOME);
                nbf++;
                }
            else if(ch=='B')
                nbb++;

            p.plateau[l][c] = ch; // Ecriture dans le plan

            c++; // caractère suivant
            }
        l++; // ligne suivante
        }

    fclose(f); // Fermeture du flux de lecture du fichier

/* Si à la lecture de EOF on n'est pas sur la *V+1 ème ligne...               */
    if(l != p.L+1)
        {
        printf("Ligne %d: nb de lignes incorrect\n",l);
        exit(0);
        }

/* Si nb de fantômes incorrect...                                             */
    if(nbf!=NBFANTOMES)
        {
        printf("Nb de fantômes incorrect sur le plan\n");
        fclose(f);
        exit(0);
        }

/* Si pas de bonus....                                                       */
    if(nbb==0)
        {
        printf("Aucun bonus sur le plan!\n");
        fclose(f);
        exit(0);
        }
    p.nbbonus = nbb;

    // TODO: Ne pas avoir une taille fixe
    // Cette taille ne marche que pour le plateau test.txt (27x21)
    p.taille_case[0] = 420 / p.C;
    p.taille_case[1] = 540 / p.L;

    return p;
    }

// Renvoie le type de case dans vers laquelle se déplace l'entitée
char case_direction(Partie *p, Entite *e, int sens) {
    Pos pos = e->pos;
    switch(sens) {
    case 1: return p->plateau[pos.l-1][pos.c];
    case 2: return p->plateau[pos.l+1][pos.c];
    case 3: return p->plateau[pos.l][pos.c-1];
    case 4: return p->plateau[pos.l][pos.c+1];
    }
    return '*';
}

int mouvement_clavier(int sens){
    if (touche_a_ete_pressee(SDLK_UP)){//fleche du haut pressé
        return 1;
    }
    else if (touche_a_ete_pressee(SDLK_DOWN)){//fleche du bas pressé
        return 2;
    }
    else if (touche_a_ete_pressee(SDLK_LEFT)){//fleche de gauche pressé
        return 3;
    }
    else if (touche_a_ete_pressee(SDLK_RIGHT)){//fleche de droite pressé
        return 4;
    }
    else {
        return sens;
    }
}

void maj_etat(Partie *p){
    if (p->plateau[p->pacman.pos.l][p->pacman.pos.c]=='.'){
        p->pacman.etat.score+=1;
    }
    else if (p->plateau[p->pacman.pos.l][p->pacman.pos.c]=='B') {
        p->pacman.etat.score+=50;
        p->pacman.etat.fuite=1;
        //metre compteur de temps a 0
    }
    /*
    ajout de temps ecouler a la structure partie
    else if (temps ecouler > x ){
        p->pacman.etat.fuite=0;
    }*/
    for (int i =0;i!=NBFANTOMES;i++){
        if ((p->pacman.pos.l == p->fantomes[i].pos.l) && (p->pacman.pos.c == p->fantomes[i].pos.c)){
            p->pacman.etat.nb_vie-=1;
            p->pacman.pos = (Pos){10, 0};
        }
    }
}


void actualiser_partie(Partie *p, Timer *timer) {
    p->pacman.etat.prochain_sens = mouvement_clavier(p->pacman.etat.prochain_sens);
    p->pacman.delai_deplacement -= timer->dt;
    for (int i = 0; i < NBFANTOMES; i++) {
        p->fantomes[i].delai_deplacement -= timer->dt;
    }

    bouger_pacman(p);
    bouger_fantomes(p);
    maj_etat(p);
}

void dessiner_grille(Partie *p) {
    int cx = p->taille_case[0];
    int cy = p->taille_case[1];
    for (int i = 0; i < p->L; i++) {
        for (int j = 0; j < p->C; j++) {
            Point pos = {j * cx, i * cy};
            char type = p->plateau[i][j];
            // Mur
            if (type == '*')
                dessiner_rectangle(pos, cx, cy, rouge);
            // Pacman
            else if (type == 'P')
                dessiner_rectangle(pos, cx, cy, vert);
            // Bonbon
            else if (type == '.')
                dessiner_rectangle(pos, cx, cy, gris);
            // Bonus
            else if (type == 'B')
                dessiner_rectangle(pos, cx, cy, orange);
            // Fantôme
            else if (type == 'F')
                dessiner_rectangle(pos, cx, cy, bleu);
            // Vide
            else if (type == ' ')
                dessiner_rectangle(pos, cx, cy, blanc);
        }
    }
}

void dessiner_partie(Partie *p) {
    dessiner_grille(p);
    actualiser();
}
