/******************************************************************************/
/* CHARGEMENT.c                                                               */
/******************************************************************************/
#include <math.h>
#include <assert.h>
#include "./partie.h"
#include "entite.h"
#include "pacman.h"
#include "fantome.h"
#include "leaderboard.h"
#include "main.h"
#include "entrer.h"
#include "affichage.h"


/******************************************************************************/
 /* CHARGE PLAN                                                                */
/******************************************************************************/
int charger_plan(char *chemin, Partie *p) {
    FILE *f = fopen(chemin, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur dans l'ouverture de %s", chemin);
        return -1;
    }

    int res = fscanf(f,"%d %d\n", &p->L, &p->C); // Lecture de deux entiers

    // Si on n'a pas pu lire deux entiers ou s'ils sont incorrects
    if (res != 2 || p->C < 2 || p->L < 2 || p->C > 800 || p->L > 600) {
        fprintf(stderr, "Dimensions du tableau lues dans '%s' incorrectes\n", chemin);
        fclose(f);
        return -1;
    }

    /* ALLOCATION DYNAMIQUE                                                       */
    /* Allocation du tableau de *L pointeurs sur lignes                           */
    p->plateau = malloc(p->L * sizeof(char *));
    if(p->plateau == NULL) {
        fprintf(stderr, "Allocation dynamique impossible\n");
        fclose(f);
        exit(1);
    }
/* Allocation des tableaux de *C caractères                                   */
    for (int l = 0; l != p->L; l++) {
        p->plateau[l] = malloc(p->C * sizeof(char));
        if(p->plateau[l] == NULL) {
            fprintf(stderr, "Allocation dynamique impossible\n");
            fclose(f);
            exit(1);
        }
    }

    int l = 0;
    res = 0;
    char ch;
    while(res != EOF) { // Lecture de chaque ligne
        int c = 0;
        while(1) {
            res = fscanf(f,"%c", &ch); // Lecture d'un caractère
            if (res == EOF) // Si fin de fichier
                break; // Quittons la boucle interne

            if(c>p->C) { // Si trop de colonnes...
                fprintf(stderr, "Ligne %d colonne %d: trop de colonnes\n", l, c);
                fclose(f);
                return -1;
            }

            if(c==p->C) { // Si fin de ligne supposée...
                if(ch=='\n') { // Si fin de ligne réelle, on quitte la boucle
                    break;
                }
                else { // Sinon trop de caractères
                    fprintf(stderr, "Ligne %d: trop de caractères\n", l);
                    fclose(f);
                    return -1;
                }
            }
            /* ...sinon, nous ne sommes pas à la fin de la ligne.                         */
            /* Si on lit un caractère interdit...                                         */
            if (ch != '.' && ch != ' ' && ch != '*' && ch != 'P' && ch != 'F' && ch != 'B' && ch != 'C') {
                if(ch=='\n') // Si c'est un saut de ligne
                    printf("Ligne %d: trop peu de caractères\n",l);
                else
                    printf("Ligne %d: caractère '%c' incorrect\n",l,ch);
                fclose(f);
                return -1;
            }
            else if (ch == 'P') {
                if (p->pacman_place == 0) {
                    p->pacman = nouvelle_entite((Posf){l * CASE, c * CASE}, (Posf){l * CASE, c * CASE}, ENTITE_PACMAN);
                    p->pacman_place = 1;
                }
                else {
                    fprintf(stderr, "Ligne %d: Pacman en trop!\n", l);
                    c++;
                    continue;
                }
            }
            else if (ch == 'F') {
                if (p->nbf >= 4) {
                    fprintf(stderr, "Ligne %d:  un fantôme de trop!\n", l);
                    c++;
                    continue;
                }
                p->fantomes[p->nbf] = nouvelle_entite((Posf){l * CASE, c * CASE},(Posf){l * CASE, c * CASE}, p->nbf);
                p->nbf++;
            }

            p->plateau[l][c] = ch; // Ecriture dans le plan

            c++; // caractère suivant
        }
        l++; // ligne suivante
    }

    fclose(f); // Fermeture du flux de lecture du fichier

    /* Si à la lecture de EOF on n'est pas sur la *V+1 ème ligne...               */
    if(l != p->L+1) {
        fprintf(stderr, "Ligne %d: nb de lignes incorrect\n", l);
        return -1;
    }

    if (p->pacman_place == 0 || p->nbf == 0) {
        fprintf(stderr, "Aucun pacman ou fantôme sur la grille\n");
    }

    return 0;
}

 
char on_grid(Partie *p, int l, int c) {
    if (l >= 0 && c >= 0 && l < p->L && c < p->C)
        return p->plateau[l][c];
    return ' ';
}

int aligne_grille(Partie *p, Posf pos) {
    return (int)(fmod(pos.l, CASE)) <= 1 && (int)(fmod(pos.c, CASE)) <= 1;
}


// Renvoie le type de case vers laquelle se déplace l'entitée
char case_direction(Partie *p, Entite *e, int direction) {
    Pos pos = ecran_vers_grille(e->pos);
    switch(direction) {
    case DIR_HAUT: pos.l -= 1; break;
    case DIR_BAS: pos.l += 1; break;
    case DIR_GAUCHE: pos.c -= 1; break;
    case DIR_DROITE: pos.c += 1; break;
    default: return '*';
    }
    return on_grid(p, pos.l, pos.c);
}

void maj_etat(Partie *p){
    Pos pos_pacman = ecran_vers_grille(p->pacman.pos); 
    for (int i = 0; i != p->nbf; i++) {
        Pos pos_fantome = ecran_vers_grille(p->fantomes[i].pos);

        // Détection de la collision entre fantome et pacman
        if ((pos_pacman.l == pos_fantome.l) && (pos_pacman.c == pos_fantome.c)){
            if (!(p->fantomes[i].etat.fuite)){
                p->pacman.etat.nb_vie-=1;
                if (p->pacman.etat.nb_vie == 0) {
                    envoyer_score(p);
                    charger_accueil();
                    return;
                }

                p->pacman.pos = p->pacman.pos_init;
                for (int b =0;b!=p->nbf;b++ ){
                    p->fantomes[b].pos=p->fantomes[b].pos_init;
                    p->fantomes[b].pos_cible=(Posf){0,0};
                    reset_timer_fantomes();
                }
            }
            else {
                p->fantomes[i].etat.manger=1;
            }
        }
    }
}

void actualiser_partie(Partie *p, Timer *timer) {
    int touche = nouvelle_touche();
    if (touche == SDLK_q) {
        charger_accueil();
    }

    bouger_pacman(p, timer->dt, touche);
    bouger_fantomes(p, timer->dt);
    maj_etat(p);

    if (p->nbbonus == 0) {
        envoyer_score(p);
        charger_accueil();
    }
}

void dessiner_grille(Partie *p, int dans_editeur) {
    // Efface la grille pour la redessiner de 0
    dessiner_rectangle((Point){0,0}, PLATEAU_W, ECRAN_H, noir);
    for (int i = 0; i < p->L; i++) {
        for (int j = 0; j < p->C; j++) {
            Point pos = {j * CASE, i * CASE};
            char type = p->plateau[i][j];
            // Mur
            if (type == '*') {
                afficher_surface(sprite_at((Point){j,i}), pos);
            }
            // Bonbon
            else if (type == '.') {
                afficher_surface(sprite_index(0), pos);
            }
            // Bonus
            else if (type == 'B') {
                afficher_surface(sprite_index(1), pos);
            }
            if (dans_editeur) {
                if (type == 'P') {
                    afficher_surface(sprite_pacman(0,0), pos);
                }
                else if (type == 'F') {
                    afficher_surface(sprite_fantome(0,0,0), pos);
                }
            }
        }
    }
}

void dessiner_texte(Partie *p) {
    // efface le texte précédent
    // Remarque : on fait ça pour tricher un peu, lorsque pacman arrive sur la droite du plateau, il est dessiné
    // au dessus de la zone de texte. En faisant ça, on efface les pixels de pacman qui sont en dehors du plateau
    // ce qui donne une impression de transition
    dessiner_rectangle((Point){p->C * CASE, 0}, 200, ECRAN_H, noir);

    // Espace entre les éléments
    const int padding = 5;

    Point point_affichage = {CASE * p->C + padding, 0}; // Origine des texte (en haut à droite du plateau)
    // Titre score
    afficher_texte("Score", 26, point_affichage, rouge);

    // Score
    point_affichage.y += 26 + padding; // Descend la positon de la taille du texte + padding pour avoir de l'espace entre les textes
    char score[6];
    sprintf(score, "%05d", p->pacman.etat.score); // Converties le score de pacman de int à string
    afficher_texte(score, 26, point_affichage, blanc);

    // Titre vies
    point_affichage.y += (26 + padding) * 2; // Grand espace entre les deux sections
    afficher_texte("Vies", 26, point_affichage, rouge);

    // Vies
    point_affichage.y += 26 + padding;
    char vies[2];
    sprintf(vies, "%d", p->pacman.etat.nb_vie); // Converties les vies de pacman de int à string
    afficher_texte(vies, 26, point_affichage, blanc);
}

void dessiner_partie(Partie *p) {
    dessiner_grille(p, 0);
    dessiner_pacman(p);
    dessiner_fantomes(p);
    dessiner_texte(p);
    actualiser();
}