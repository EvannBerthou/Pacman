#include <sys/stat.h>
#include <stdio.h>
#include "main.h"
#include "editeur.h"

char selected_char = '*';
int pacman_place = 0;
int nb_fantomes = 0;

const char bouton_editeurs[] = {'*', '.', 'B', ' ', 'P', 'F'};
const char *boutons_editeur_textes[] = {"Mur", "Bille", "Bonbon", "Vide", "Pacman", "Fantome"};
const int n = sizeof(bouton_editeurs) / sizeof(bouton_editeurs[0]);

int sauvegarder_niveau(Partie *p) {
    printf("Début de la sauvegarde\n");
    // Ouvre le fichier
    FILE *f = fopen("data/maps/tmp", "w");
    if (f == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier\n");
        return 1;
    }

    // Ne pas sauvegarder si pacman n'est pas placé sur la grille
    if (pacman_place == 0) {
        printf("Pacman n'est pas sur la grille\n");
        return 1;
    }

    /*if (niveau->nb_fantomes < 4) {
        printf("Il manque des fantomes\n");
        return 1;
    }*/

    // Ecris la taille de la carte
    fprintf(f, "%d %d\n", p->L, p->C);

    // Ecris chaque ligne dans le fichier
    for (int h = 0; h < p->L; h++) {
        char ligne[p->L];
        snprintf(ligne, p->L, "%s", p->plateau[h]);
        fprintf(f, "%s\n", ligne);
    }

    fclose(f);
    printf("Sauvegarde terminée\n");
    return 0;
}

Partie charger_editeur(char *chemin) {
    Partie p = charge_plan(chemin);

    // Détermine si pacman est placé et le nombre de fantomes
    for (int i = 0; i < p.L; i++) {
        for (int j = 0; j < p.C; j++) {
            char c = p.plateau[i][j];
            if (c == 'P') {
                if (pacman_place == 0) {
                    pacman_place = 1;
                }
                else {
                    p.plateau[i][j] = ' ';
                    printf("Un pacman est déjà placé sur la grille");
                }
            }
            if (c == 'F') {
                if (nb_fantomes < 4) {
                    nb_fantomes++;
                }
                else {
                    p.plateau[i][j] = ' ';
                    printf("Trop de fantomes sur la carte\n");
                }
            }
        }
    }
    return p;
}

static void dessiner_boutons() {
    const int font_size = 26;
    // Efface l'écran
    dessiner_rectangle((Point){420, 0}, ECRAN_W - 420, ECRAN_H, noir);
    for (int i = 0; i < n; i++) {
        // Texte du bouton à afficher
        const char *texte = boutons_editeur_textes[i];
        // Rouge si selectioné sinon blanc
        Couleur couleur = bouton_editeurs[i] == selected_char ? rouge : blanc;
        afficher_texte((char *)texte, font_size, (Point){430, i * font_size + 10}, couleur);
    }
}

static void placer_case(Partie *p, Point grille) {
    char celulle = p->plateau[grille.y][grille.x];
    if (selected_char == 'P') {
        if (pacman_place) return;
        pacman_place = 1;
    }
    if (celulle == 'P' && selected_char != 'P') {
        pacman_place = 0;
    }

    if (selected_char == 'F') {
        if (nb_fantomes == 4) return;
        nb_fantomes++;
    }
    if (celulle == 'F' && selected_char != 'F') {
        nb_fantomes--;
    }

    p->plateau[grille.y][grille.x] = selected_char;
    calculer_voisins(p);
}

static Point point_vers_grille(Point p) {
    return (Point){
        p.x / CASE,
        p.y / CASE
    };
}

static void entourner_case(Point souris) {
    Point grille = point_vers_grille(souris);
    grille.x *= CASE;
    grille.y *= CASE;
    // HAUT
    dessiner_ligne((Point){grille.x, grille.y}, (Point){grille.x + CASE, grille.y}, blanc);
    // BAS
    dessiner_ligne((Point){grille.x, grille.y + CASE}, (Point){grille.x + CASE, grille.y + CASE}, blanc);
    // GAUCHE
    dessiner_ligne((Point){grille.x, grille.y}, (Point){grille.x, grille.y + CASE}, blanc);
    // DROITE
    dessiner_ligne((Point){grille.x + CASE, grille.y}, (Point){grille.x + CASE, grille.y + CASE}, blanc);
}


void lancer_editeur() {
    pacman_place = 0;
    nb_fantomes = 0;
    Partie p = charger_editeur("data/maps/test.txt");
    calculer_voisins(&p);

    int en_cours = 1;
    while (en_cours) {
        traiter_evenements();

        Point souris = deplacement_souris_a_eu_lieu();
        if (souris.x <= 420) {
            Point clic = clic_a_eu_lieu();
            if (clic.x != -1 && clic.y != -1) {
                Point grille = point_vers_grille(clic);
                placer_case(&p, grille);
            }
        }

        if (touche_a_ete_pressee(SDLK_a))
            selected_char = bouton_editeurs[0];
        else if (touche_a_ete_pressee(SDLK_z))
            selected_char = bouton_editeurs[1];
        else if (touche_a_ete_pressee(SDLK_e))
            selected_char = bouton_editeurs[2];
        else if (touche_a_ete_pressee(SDLK_r))
            selected_char = bouton_editeurs[3];
        else if (touche_a_ete_pressee(SDLK_t))
            selected_char = bouton_editeurs[4];
        else if (touche_a_ete_pressee(SDLK_y))
            selected_char = bouton_editeurs[5];

        else if (touche_a_ete_pressee(SDLK_s))
            sauvegarder_niveau(&p);
        else if (touche_a_ete_pressee(SDLK_q))
            en_cours = 0;

        dessiner_grille(&p, 1);
        entourner_case(souris); // Permet de montrer quelle case est survolée
        dessiner_boutons();
        actualiser();
        reinitialiser_evenements();
        attente(16); // Limite la vitesse du programme pour éviter une surutilisation du CPU
    }
}

/*
TODO :
    - Message d'information lors de la sauvegarde en dessous du type selectionné
    - Sauvegarde dans le fichier chargé
*/
