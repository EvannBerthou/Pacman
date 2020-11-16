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

#define SPRITE_COUNT 18
const char *sprites_paths[SPRITE_COUNT] = {
    "data/sprites/bille.bmp",
    "data/sprites/bonbon.bmp",
    "data/sprites/mur0.bmp",
    "data/sprites/mur1.bmp",
    "data/sprites/mur2.bmp",
    "data/sprites/mur3.bmp",
    "data/sprites/mur4.bmp",
    "data/sprites/mur5.bmp",
    "data/sprites/mur6.bmp",
    "data/sprites/mur7.bmp",
    "data/sprites/mur8.bmp",
    "data/sprites/mur9.bmp",
    "data/sprites/mur10.bmp",
    "data/sprites/mur11.bmp",
    "data/sprites/mur12.bmp",
    "data/sprites/mur13.bmp",
    "data/sprites/mur14.bmp",
    "data/sprites/mur15.bmp",
};
SDL_Surface* sprites[SPRITE_COUNT];

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

int charger_sprites() {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        SDL_Surface *sprite = SDL_LoadBMP(sprites_paths[i]);
        if (sprite == NULL) {
            fprintf(stderr, "Erreur lors du chargement du sprite %s\n", sprites_paths[i]);
            return -1;
        }
        sprites[i] = sprite;
    }
    if (charger_sprites_fantomes() == -1) {
        return -1;
    }
    if (charger_sprites_pacman() == -1) {
        return -1;
    }
    return 0;
}

// Calculer un nombre pour déterminer le nombre de voisins puis le stocker dans le meme ordre
// que le plateau
short voisins_murs[27 * 21] = {};
// Renvoie le bon sprite
static SDL_Surface *sprite_at(Point pos) {
    // Convertis les coordonnés 2d en index pour un array 1d
    int index = 21 * pos.y + pos.x;
    // Le 2 correspond à l'offset pour avoir les sprites des murs
    return sprites[2 + voisins_murs[index]];
}

char on_grid(Partie *p, int l, int c) {
    if (l >= 0 && c >= 0 && l < p->L && c < p->C)
        return p->plateau[l][c];
    return ' ';
}

int aligne_grille(Partie *p, Posf pos) {
    return (int)(fmod(pos.l, CASE)) <= 1 && (int)(fmod(pos.c, CASE)) <= 1;
}

void calculer_voisins(Partie *p) {
    for (int i = 0; i < p->L; i++) {
        for (int j = 0; j < p->C; j++) {
            short voisins = 0;
            /*
             Ici on encode les voisins mais au lieu de stocker si un mur a des voisins
             dans 4 variables pour chaque direction, on stocke tous les voisins dans une seule variable
             Cela permet d'économiser beaucoup de mémoire
             Un int est encodé sur 4 bytes (du moins sur ma machine), ce qui donne :
             27 * 21 * 4 (int) * 4 (voisins) = 9,072 bytes
             En utilisant d'un seul short de 2 bytes, ça donne
             27 * 21 * 2 (short) * 1 (une seule variable) = 1,134 bytes
             Cela permet en plus de plus facilement déterminer quel sprite doit être afficher
             pour chaque mur
            */
            if (on_grid(p, i - 1, j) == '*')
                voisins |= 0b0001;
            if (on_grid(p, i + 1, j) == '*')
                voisins |= 0b0010;
            if (on_grid(p, i, j - 1) == '*')
                voisins |= 0b0100;
            if (on_grid(p, i, j + 1) == '*')
                voisins |= 0b1000;
            voisins_murs[21 * i + j] = voisins;
        }
    }
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

int deplacement(int touche, int direction){
    switch (touche) {
        case SDLK_UP: return DIR_HAUT;
        case SDLK_DOWN: return DIR_BAS;
        case SDLK_LEFT: return DIR_GAUCHE;
        case SDLK_RIGHT: return DIR_DROITE;
        default: return direction;
    }
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
                    terminer_partie(p);
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
    p->pacman.etat.prochaine_direction = deplacement(touche, p->pacman.etat.prochaine_direction);

    bouger_pacman(p, timer->dt);
    bouger_fantomes(p, timer->dt);
    maj_etat(p);

    if (p->nbbonus == 0) {
        terminer_partie(p);
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
                afficher_surface(sprites[0], pos);
            }
            // Bonus
            else if (type == 'B') {
                afficher_surface(sprites[1], pos);
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


void terminer_partie(Partie *p) {
    char *nom = entrer_nom();

    const char *post_req =
        "POST / HTTP/1.0\r\n"
        "Host: pacman-leaderboard.herokuapp.com\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: %d\r\n\r\n"
        "%s\r\n";
    const char post_params[] = "joueur=%s&points=%d";
    char params[64] = {};
    sprintf(params, post_params, nom, p->pacman.etat.score);
    char req[2048] = {};
    sprintf(req, post_req, strlen(params), params);

    //char *reponse = envoyer_requete("localhost", 3000, req);
    char *reponse = envoyer_requete("pacman-leaderboard.herokuapp.com", 80, req);
    if (reponse != NULL) {
        free(reponse);
    }
    free(nom);
    charger_accueil();
}

char * entrer_nom() {
    // Nom a 5 lettres + \0
    char nom[6] = "AAAAA";
    int touche = 0, index = 0;
    afficher_nom(nom, index);
    while (touche != SDLK_RETURN) {
        traiter_evenements();
        touche = nouvelle_touche();
        if (touche == SDLK_RIGHT)
            index = (index + 1) % 5;
        else if (touche == SDLK_LEFT) {
            index--;
            if (index < 0) index = 4;
        }
        // Autorise que les lettres majuscules
        else if (touche == SDLK_UP) {
            if (nom[index] - 1 >= 'A')
                nom[index]--;
        }
        else if (touche == SDLK_DOWN) {
            if (nom[index] + 1 <= 'Z')
                nom[index]++;
        }
        afficher_nom(nom, index);
        reinitialiser_evenements();
        attente(100);
    }
    return strdup(nom);
}

Point centrer_boite(Point centre, Point taille) {
    return (Point) {
        centre.x - (taille.x / 2),
        centre.y - (taille.y / 2),
    };
}

void afficher_nom(char *nom, int index) {
    // Dessine la boite autour du texte au centre de l'écran
    Point coin = centrer_boite((Point){ECRAN_W / 2, ECRAN_H / 2}, (Point){5 * 46, 60});
    dessiner_rectangle(coin, 5 * 46, 60, noir);
    // Ligne haut
    dessiner_ligne(coin, (Point){coin.x + 5 * 46, coin.y}, blanc);
    // Ligne bas
    dessiner_ligne((Point){coin.x, coin.y + 60}, (Point){coin.x + 5 * 46, coin.y + 60}, blanc);
    // Ligne droite
    dessiner_ligne((Point){coin.x + 5 * 46, coin.y}, (Point){coin.x + 5 * 46, coin.y + 60}, blanc);
    // Ligne gauche
    dessiner_ligne(coin, (Point){coin.x, coin.y + 60}, blanc);

    // Permet de dessiner chaque caractère du texte dans une position fixe au lieu de le texte change de taille
    for (int i = 0; i < 5; i++) {
        // La lettre a affichée
        char c = nom[i];
        // Fabrique un string contenant seulement la lettre à affiche, on ne peut pas directement afficher
        // &c car le buffer ne se serait pas terminé par un \0 et
        char buff[2];
        sprintf(buff, "%c", c);
        // Détermine la position de la lettre
        Point pos = (Point){coin.x + i * 46 + 5, coin.y};
        // Affiche la lettre
        afficher_texte(buff, 46, pos, blanc);
        // Souligne la lettre en cours de modification
        if (i == index) {
            dessiner_ligne((Point){pos.x, pos.y + 50}, (Point){pos.x + 35, pos.y + 50}, blanc);
        }
    }
    actualiser();
}
