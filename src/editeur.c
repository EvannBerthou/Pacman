#include <sys/stat.h>
#include <stdio.h>
#include "main.h"
#include "editeur.h"

char selected_char = '*';
int pacman_place = 0;
int nb_fantomes = 0;
char chemin_fichier[100];

const char bouton_editeurs[] = {'*', '.', 'B', ' ', 'P', 'F'};
const char *boutons_editeur_textes[] = {"Mur", "Bille", "Bonbon", "Vide", "Pacman", "Fantome"};
const int n = sizeof(bouton_editeurs) / sizeof(bouton_editeurs[0]);

static Partie partie_vide() {
    Partie p;
    p.L = 27;
    p.C = 21;

    p.plateau = malloc(p.L * sizeof(char *));
    if(p.plateau == NULL) {
        fprintf(stderr, "Allocation dynamique impossible\n");
        exit(1);
    }

    for (int l = 0; l != p.L; l++) {
        p.plateau[l] = malloc(p.C * sizeof(char));
        if(p.plateau[l] == NULL) {
            fprintf(stderr, "Allocation dynamique impossible\n");
            exit(1);
        }
    // Met toute la colonne à 0 pour éviter d'avoir des valeurs inattendus
        memset(p.plateau[l], 0, p.C * sizeof(char));
    }
    return p;
}
// Très fortement inspiré du charger_plan fournis
Partie charger_plan_editeur(char *chemin) {
    Partie p;
    FILE *f = fopen(chemin, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur dans l'ouverture de %s", chemin);
        return partie_vide();
    }

    int res = fscanf(f,"%d %d\n", &p.L, &p.C); // Lecture de deux entiers

    // Si on n'a pas pu lire deux entiers ou s'ils sont incorrects
    if (res != 2 || p.C < 2 || p.L < 2 || p.C > 800 || p.L > 600) {
        fprintf(stderr, "Dimensions du tableau lues dans '%s' incorrectes\n", chemin);
        fclose(f);
        return partie_vide();
    }

    /* ALLOCATION DYNAMIQUE                                                       */
    /* Allocation du tableau de *L pointeurs sur lignes                           */
    p.plateau = malloc(p.L * sizeof(char *));
    if(p.plateau == NULL) {
        fprintf(stderr, "Allocation dynamique impossible\n");
        fclose(f);
        exit(1);
    }
/* Allocation des tableaux de *C caractères                                   */
    for (int l = 0; l != p.L; l++) {
        p.plateau[l] = malloc(p.C * sizeof(char));
        if(p.plateau[l] == NULL) {
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

            if(c>p.C) { // Si trop de colonnes...
                fprintf(stderr, "Ligne %d colonne %d: trop de colonnes\n", l, c);
                fclose(f);
                return partie_vide();
            }

            if(c==p.C) { // Si fin de ligne supposée...
                if(ch=='\n') { // Si fin de ligne réelle, on quitte la boucle
                    break;
                }
                else { // Sinon trop de caractères
                    fprintf(stderr, "Ligne %d: trop de caractères\n", l);
                    fclose(f);
                    return partie_vide();
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
                return partie_vide();
            }
            else if (ch == 'P') {
                if (pacman_place == 1) { 
                    c++;
                    continue;
                }
                pacman_place = 1;
            }
            else if (ch == 'F') {
                if (nb_fantomes >= 4) {
                    fprintf(stderr, "Ligne %d:  un fantôme de trop!\n", l);
                    c++;
                    continue;
                }
                nb_fantomes++;
            }

            p.plateau[l][c] = ch; // Ecriture dans le plan

            c++; // caractère suivant
        }
        l++; // ligne suivante
    }

    fclose(f); // Fermeture du flux de lecture du fichier

    /* Si à la lecture de EOF on n'est pas sur la *V+1 ème ligne...               */
    if(l != p.L+1) {
        fprintf(stderr, "Ligne %d: nb de lignes incorrect\n", l);
        return partie_vide();
    }
    return p;
}

Partie charger_editeur() {
    nb_fantomes = 0;
    pacman_place = 0;
    return charger_plan_editeur(chemin_fichier);
}

int sauvegarder_niveau(Partie *p) {
    printf("Début de la sauvegarde\n");
    // Ouvre le fichier
    FILE *f = fopen(chemin_fichier, "w");
    if (f == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier\n");
        return 1;
    }

    // Ne pas sauvegarder si pacman n'est pas placé sur la grille
    if (pacman_place == 0) {
        printf("Pacman n'est pas sur la grille\n");
        return 1;
    }

    if (nb_fantomes < 4) {
        printf("Il manque des fantomes\n");
        return 1;
    }

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

void lancer_editeur(char *chemin) {
    pacman_place = 0;
    nb_fantomes = 0;
    sprintf(chemin_fichier, "data/maps/%s", chemin);
    Partie p = charger_editeur(chemin_fichier);
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
*/
