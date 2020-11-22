#include <math.h>
#include "fantome.h"
#include "main.h"

// Sprites des fantômes
#define SPRITE_FANTOME_COUNT 7
const char *fantome_sprite_path[SPRITE_FANTOME_COUNT][4][2] = {

    {
        //sprite de fantomes en mode normale R pour fantome red

        {"data/sprites/fantomeR00.bmp","data/sprites/fantomeR01.bmp"},
        {"data/sprites/fantomeR10.bmp","data/sprites/fantomeR11.bmp"},
        {"data/sprites/fantomeR20.bmp","data/sprites/fantomeR21.bmp"},
        {"data/sprites/fantomeR30.bmp","data/sprites/fantomeR31.bmp"},
    },
    {
        //sprite de fantomes en mode normale O pour fantome orange

        {"data/sprites/fantomeO00.bmp","data/sprites/fantomeO01.bmp"},
        {"data/sprites/fantomeO10.bmp","data/sprites/fantomeO11.bmp"},
        {"data/sprites/fantomeO20.bmp","data/sprites/fantomeO21.bmp"},
        {"data/sprites/fantomeO30.bmp","data/sprites/fantomeO31.bmp"},
    },
    {
        //sprite de fantomes en mode normale P pour fantome pink

        {"data/sprites/fantomeP00.bmp","data/sprites/fantomeP01.bmp"},
        {"data/sprites/fantomeP10.bmp","data/sprites/fantomeP11.bmp"},
        {"data/sprites/fantomeP20.bmp","data/sprites/fantomeP21.bmp"},
        {"data/sprites/fantomeP30.bmp","data/sprites/fantomeP31.bmp"},
    },
    {
        //sprite de fantomes en mode normale B pour fantome orange

        {"data/sprites/fantomeB00.bmp","data/sprites/fantomeB01.bmp"},
        {"data/sprites/fantomeB10.bmp","data/sprites/fantomeB11.bmp"},
        {"data/sprites/fantomeB20.bmp","data/sprites/fantomeB21.bmp"},
        {"data/sprites/fantomeB30.bmp","data/sprites/fantomeB31.bmp"},
    },
    {
        //sprites fantome mode fuite avec tmp_fuite <5000ms
        {"data/sprites/fantome_fuite_bleu0.bmp","data/sprites/fantome_fuite_bleu1.bmp"},
        {"data/sprites/fantome_fuite_bleu0.bmp","data/sprites/fantome_fuite_bleu1.bmp"},
        {"data/sprites/fantome_fuite_bleu0.bmp","data/sprites/fantome_fuite_bleu1.bmp"},
        {"data/sprites/fantome_fuite_bleu0.bmp","data/sprites/fantome_fuite_bleu1.bmp"},
    },
    {
        //sprite fantome mode fuite avec tmp > 5000 ms
        //alerne entre celui la et le precedant a une frequence de plus en plus rapite jusqu'a tmp_fuite >=10000
        {"data/sprites/fantome_fuite_blanc0.bmp","data/sprites/fantome_fuite_blanc1.bmp"},
        {"data/sprites/fantome_fuite_blanc0.bmp","data/sprites/fantome_fuite_blanc1.bmp"},
        {"data/sprites/fantome_fuite_blanc0.bmp","data/sprites/fantome_fuite_blanc1.bmp"},
        {"data/sprites/fantome_fuite_blanc0.bmp","data/sprites/fantome_fuite_blanc1.bmp"},
    },
    {
        //sprite fantome mode manger
        {"data/sprites/fantome_manger0.bmp", "data/sprites/fantome_manger0.bmp"},
        {"data/sprites/fantome_manger1.bmp", "data/sprites/fantome_manger1.bmp"},
        {"data/sprites/fantome_manger2.bmp", "data/sprites/fantome_manger2.bmp"},
        {"data/sprites/fantome_manger3.bmp", "data/sprites/fantome_manger3.bmp"},
    }
};
SDL_Surface *sprites_fantomes[SPRITE_FANTOME_COUNT][4][2];

// Vérifie que le fantôme n'a pas traversé de mur pendant son déplacement, (peut arriver lorsque les FPS sont bas)
static Posf traverse_mur(Partie *p, Entite *fantome, Posf precf, Posf currf) {
    Pos prec = ecran_vers_grille(precf);
    Pos curr = ecran_vers_grille(currf);

    // Si le fantome n'a pas changé de case, pas besoin de vérifier s'il a traversé un mur
    if (prec.l == curr.l && prec.c == curr.c) {
        return currf;
    }

    // Refait le chemin entre la position précédente et la nouvelle position du fantome
    while (prec.l != curr.l || prec.c != curr.c) {
        Pos prochain = prec;
        switch (fantome->etat.direction) {
            case DIR_HAUT:   prochain.l--; break;
            case DIR_BAS:    prochain.l++; break;
            case DIR_GAUCHE: prochain.c--; break;
            case DIR_DROITE: prochain.c++; break;
            default: break;
        }
        // Si le fantome a traversé un mur ou est sorti du niveau alors le faire reculer avant le mur
        char type_case = on_grid(p, prochain.l, prochain.c);
        if (type_case == '*' || type_case == 'x') {
            return (Posf) {prec.l * CASE, prec.c * CASE};
        }
        prec = prochain;
    }
    return currf;
}

// Charge les sprites des fantômes au lancement du programme
int charger_sprites_fantomes() {
    for (int i = 0; i < SPRITE_FANTOME_COUNT; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 2; k++) {
                SDL_Surface *sprite = SDL_LoadBMP(fantome_sprite_path[i][j][k]);
                if (sprite == NULL) {
                    fprintf(stderr, "Erreur lors du chargement du sprite %s\n", fantome_sprite_path[i][j][k]);
                    return -1;
                }
                SDL_SetColorKey(sprite, SDL_SRCCOLORKEY, noir);
                sprites_fantomes[i][j][k] = sprite;
            }
        }
    }
    return 0;
}

SDL_Surface *sprite_fantome(TypeEntite t, int dir, int frame) {
    return sprites_fantomes[t][dir][frame];
}

// Temps avant que chaque fantome quitte la base
float timer_fantomes[4];
void reset_timer_fantomes() {
    timer_fantomes[0] = 1.f;
    timer_fantomes[1] = 1.f;
    timer_fantomes[2] = 15.f;
    timer_fantomes[3] = 20.f;
}

void bouger_fantomes(Partie *p, float dt) {
    for (int i = 0; i < p->nbf; i++) {

        // Descend le timer avec que le fantôme sorte de la grotte
        if (timer_fantomes[i] > 0) {
            timer_fantomes[i] -= dt;
            continue;
        }

        // Lors du premier niveau, seulement 2 fantomes (Orange et rose) se déplacent
        // Tous les fantômes sont ensuite présent à partir du premier redémarrage
        if (p->niveau == 0 && i >= 2) {
            continue;
        }

        Entite *fantome = &p->fantomes[i];

        if (fantome->etat.fuite > 0) {
            fantome->etat.fuite -= dt;
        }

        Pos current_pos = ecran_vers_grille(fantome->pos);
        Pos pos_init_f= ecran_vers_grille(fantome->pos_init);

        // Si le fantome est rentré à la base
        if (current_pos.l == pos_init_f.l && current_pos.c == pos_init_f.c
                && (fantome->etat.mange || fantome->etat.fuite > 0)){
            revivre(fantome);
        }

        determiner_cible(p, fantome);
        // Trouve le chemin le plus cours entre le fantome et sa case cible
        find_path(p,current_pos,fantome->pos_cible,fantome);

        // Change la direction du fantôme ssi il est aligné à la grille pour éviter qu'il traverse les murs
        if (aligne_grille(p, fantome->pos)) {
            fantome->etat.direction = determiner_direction(fantome);
        }

        // Détermine la prochaine case où le fantome est censé ce trouver,
        // On ne change pas directement la position car cela risquerait de faire travers le mur
        Posf prochaine_pos = fantome->pos;
        switch (fantome->etat.direction) {
            case DIR_HAUT:   prochaine_pos.l -= dt * fantome->vitesse; break;
            case DIR_BAS:    prochaine_pos.l += dt * fantome->vitesse; break;
            case DIR_GAUCHE: prochaine_pos.c -= dt * fantome->vitesse; break;
            case DIR_DROITE: prochaine_pos.c += dt * fantome->vitesse; break;
        }

        fantome->animation_time += dt * VITESSE_ANIMATION;
        if (fantome->animation_time >= 2) {
            fantome->animation_time = 0;
        }

        // Renvoie la première position entre la position précédente du fantome et la nouvelle où
        // il n'y a pas de mur (évite aux fantomes de traverser les murs lorsque les FPS sont bas)
        fantome->pos = traverse_mur(p, fantome, fantome->pos, prochaine_pos);
    }
}

void determiner_cible(Partie *p, Entite *fantome) {
    // Si le fantome est en mode fuite, le faire se déplacer vers sa base
    if (fantome->etat.fuite > 0 || fantome->etat.mange) {
        fantome->pos_cible = ecran_vers_grille(fantome->pos_init);
    }
    else {
        // Ici on gère les différentes IA de chaque fantome car ils ont une case cible differente
        fantome->pos_cible = ecran_vers_grille(p->pacman.pos);

        // fantome rose Pinky
        if (fantome->type == ENTITE_FANTOME_P) {
            IA_PINK(p, fantome);
        }
        else if (fantome->type == ENTITE_FANTOME_C) {
            IA_CYAN(fantome);
        }
        else if (fantome->type == ENTITE_FANTOME_O) {
            IA_ORANGE(p, fantome);
        }
    }
}

// Lorsque le fantome orange est trop près de pacman, il se déplace vers le coin le plus loin
void IA_ORANGE(Partie *p, Entite *fantome) {
    if (distance_pac(p->pacman.pos, fantome->pos) < 80.f) {
        select_coin(fantome);
    }
}

// Le fantome rose esssaie de se déplacer 2 cases en face de pacman
void IA_PINK(Partie *p, Entite *fantome) {
    for (int delta = 3; delta > 0; delta--){
        Pos prochaine_case = fantome->pos_cible;
        switch (p->pacman.etat.direction){
            case DIR_HAUT:   prochaine_case.l -= delta; break;
            case DIR_BAS:    prochaine_case.l += delta; break;
            case DIR_GAUCHE: prochaine_case.c -= delta; break;
            case DIR_DROITE: prochaine_case.c += delta; break;
        }
        char prochaine_case_type = on_grid(p,fantome->pos_cible.l,fantome->pos_cible.c);
        if (prochaine_case_type != '*' || prochaine_case_type != 'x') {
            break;
        }
        fantome->pos_cible = prochaine_case;
    }
}

void IA_CYAN(Entite *fantome) {
    if (entier_aleatoire(10)>7){
        Pos liste_coin[4]={{1,1},{1,20},{26,1},{26,21}};
        fantome->pos_cible=liste_coin[entier_aleatoire(4)];
    }
}

// Détermine dans quelle direction est la prochaine case du déplacement du fantôme
DirEntite determiner_direction(Entite *fantome) {
    Pos current_pos = ecran_vers_grille(fantome->pos);
    DirEntite direction = fantome->etat.direction;
    if (fantome->prochain_noeud != NULL) {
        if (current_pos.l > fantome->prochain_noeud->pos.l) {
            direction = DIR_HAUT;
        }
        else if (current_pos.l < fantome->prochain_noeud->pos.l) {
            direction = DIR_BAS;
        }
        else if (current_pos.c > fantome->prochain_noeud->pos.c) {
            direction = DIR_GAUCHE;
        }
        else if (current_pos.c < fantome->prochain_noeud->pos.c) {
            direction = DIR_DROITE;
        }
    }
    return direction;
}

void dessiner_fantomes(Partie *p) {
    // Dessine les fantomes dans l'ordre inverse pour pas que le premier fantome
    // a sortir de la base soit dessiné sous les autres
    for (int i = p->nbf - 1; i >= 0; i--) {
        Entite *fantome = &p->fantomes[i];
        Point pos = {fantome->pos.c, fantome->pos.l};
        int dir = fantome->etat.direction;
        int frame = (int)fantome->animation_time;
        // Regarde vers le bas par défaut
        if (fantome->etat.direction == DIR_INCONNUE)  {
            afficher_surface(sprite_fantome(fantome->type, 1, 0), pos);
        }
        // Si le fantome s'est fait manger
        else if (fantome->etat.mange) {
            afficher_surface(sprites_fantomes[6][dir][frame], pos);
        }
        // Si le fantome fuit
        else if (fantome->etat.fuite > 0) {
            // Change d'image toutes les 0.5s
            double integral;
            double rem = modf(fantome->etat.fuite, &integral);
            int sprite = rem > 0.5f ? 4 : 5;
            afficher_surface(sprites_fantomes[sprite][dir][frame], pos);
        }
        // Fantome en mode poursuite
        else {
            SDL_Surface *sprite = sprite_fantome(fantome->type, dir, frame);
            afficher_surface(sprite, pos);
        }
    }
}



#define CARRE(x) ((x) * (x))
float distance_pac(Posf A,Posf B){
    return CARRE(A.c - B.c) + CARRE(A.l - B.l);
}

// Selectionne le coin le plus éloigné d'un fantôme
void select_coin(Entite *fantome){
    Pos liste_coin[4] = {{1,1}, {1,20}, {26,1}, {26,21}};
    int pos_coin = 0;
    float dist_max = 0;
    for (int i = 0; i < 4; i++){
        float distance_i = distance_pac((Posf){liste_coin[i].l,liste_coin[i].c}, fantome->pos);
        if (distance_i > dist_max){
            pos_coin = i;
            dist_max = distance_i;
        }
    }
    fantome->pos_cible = liste_coin[pos_coin];
}

// Passe le fantôme en mode fuite
void fuite_fantome(Entite *fantome) {
    fantome->etat.fuite = TEMPS_FUITE;
    fantome->vitesse=35;
}

// Passe le fantôme en mode mangé
void a_ete_mange(Entite *fantome) {
    fantome->etat.mange = 1;
    fantome->vitesse = 70;
}

// Faire revivre le fantôme
void revivre(Entite *fantome) {
    fantome->etat.fuite = 0;
    fantome->etat.mange = 0;
    fantome->vitesse = 50;
}

// Calcul la vitesse d'un fantôme à un niveau donné
static int formule_vitesse(int niv) {
    return CARRE(niv) + niv + VITESSE_FANTOME; // x^2 + x + vf
}

// Calcul la vitesse de chaque fantôme
void calculer_vitesse_niveau(Partie *p) {
    for (int i = 0; i < p->nbf; i++) {
        Entite *fantome = &p->fantomes[i];
        switch (fantome->type) {
        case ENTITE_FANTOME_O:
        case ENTITE_FANTOME_P:
        case ENTITE_FANTOME_C: {
            fantome->vitesse = formule_vitesse(p->niveau);
            break;
        }
        case ENTITE_FANTOME_R: {
            // Le fantôme rouge à un niveau d'avance sur sa vitesse
            fantome->vitesse = formule_vitesse(p->niveau + 1); 
            break;
        }
        default: break;
        }
    }
}
