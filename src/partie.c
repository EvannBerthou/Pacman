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

#define SPRITE_COUNT 17
const char *sprites_paths[SPRITE_COUNT] = {
    "data/sprites/bille.bmp",
    "data/sprites/bonbon.bmp",
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
    if(res !=2 || p.C<2 || p.L<2 || p.C >800 || p.L>ECRAN_W)
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
                    p.pacman = nouvelle_entite((Posf){l * CASE, c * CASE},(Posf){l * CASE, c * CASE}, ENTITE_PACMAN);
                }
            else if(ch=='F')
                {
                if(nbf>NBFANTOMES)
                    {
                    printf("Ligne %d:  un fantôme de trop!\n",l);
                    fclose(f);
                    exit(0);
                    }
                p.fantomes[nbf] = nouvelle_entite((Posf){l * CASE, c * CASE},(Posf){l * CASE, c * CASE}, ENTITE_FANTOME_R);
                nbf++;
                }
            else if(ch=='B' || ch == '.')
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

    return p;
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
    //return sprites[2 + voisins_murs[index]];
    return sprites[1 + voisins_murs[index]];
}

char on_grid(Partie *p, int l, int c) {
    if (l >= 0 && c >= 0 && l < p->L && c < p->C)
        return p->plateau[l][c];
    return ' ';
}

int aligne_grille(Partie *p, Posf pos) {
    return (int)(fmod(pos.l, CASE)) <= 1 && (int)(fmod(pos.c, CASE)) <= 1;
    //return ((int)(p->pacman.pos.l) % CASE == 0 && (int)(p->pacman.pos.c) % CASE == 0);
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
    /*if (p->plateau[pos.l][pos.c] != '*' && p->plateau[pos.l][pos.c] != 'F')
        printf("%c\n", p->plateau[pos.l][pos.c]);*/
    return on_grid(p, pos.l, pos.c);
}

int deplacement(SDL_Joystick *manette, int direction){
    int croix = 0;
    if (manette != NULL) croix = SDL_JoystickGetHat(manette, 0);
    if (touche_a_ete_pressee(SDLK_UP) || croix & SDL_HAT_UP){//fleche du haut pressé
        return DIR_HAUT;
    }
    else if (touche_a_ete_pressee(SDLK_DOWN) || croix & SDL_HAT_DOWN){//fleche du bas pressé
        return DIR_BAS;
    }
    else if (touche_a_ete_pressee(SDLK_LEFT) || croix & SDL_HAT_LEFT){//fleche de gauche pressé
        return DIR_GAUCHE;
    }
    else if (touche_a_ete_pressee(SDLK_RIGHT) || croix & SDL_HAT_RIGHT){//fleche de droite pressé
        return DIR_DROITE;
    }
    else {
        return direction;
    }
}

void maj_etat(Partie *p){
    /*
    ajout de temps ecouler a la structure partie
    else if (temps ecouler > x ){
        p->pacman.etat.fuite=0;
    }*/

    for (int i =0;i!=NBFANTOMES;i++){
        // Détection de la collision entre fantome et pacman
        if ((p->pacman.pos.l == p->fantomes[i].pos.l) && (p->pacman.pos.c == p->fantomes[i].pos.c)){
            if (!(p->fantomes[i].etat.fuite)){
                p->pacman.etat.nb_vie-=1;
                if (p->pacman.etat.nb_vie == 0) {
                    terminer_partie(p);
                    return;
                }

                p->pacman.pos = p->pacman.pos_init;
                for (int b =0;b!=NBFANTOMES;b++ ){
                    p->fantomes[b].pos=p->fantomes[b].pos_init;
                    p->fantomes[b].pos_cible=(Posf){0,0};
                }
            }
            else {
                p->fantomes[i].pos=p->fantomes[i].pos_init;
                p->fantomes[i].pos_cible=(Posf){0,0};
            }
        }
    }
}


void actualiser_partie(Partie *p, Timer *timer, SDL_Joystick *manette) {
    p->pacman.etat.prochaine_direction = deplacement(manette, p->pacman.etat.prochaine_direction);

    bouger_pacman(p, timer->dt);
    bouger_fantomes(p, timer->dt);
    maj_etat(p);

    if (p->nbbonus == 0) {
        printf("Niveau terminé, chargement du niveau suivant\n");
        *p = charge_plan("data/test.txt");
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
                    afficher_surface(p->pacman.sprite[0][0], pos);
                }
                else if (type == 'F') {
                    afficher_surface(p->fantomes[0].sprite[0][0], pos);
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
        touche = attendre_touche();
        if (touche == SDLK_RIGHT)
            index = (index + 1) % 5;
        else if (touche == SDLK_LEFT) {
            index = (index - 1);
            if (index < 0) index = 5;
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
