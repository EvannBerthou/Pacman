#include <sys/stat.h>
#include <stdio.h>
#include "main.h"
#include "editeur.h"

char selected_char = '*';
char chemin_fichier[100];

const char bouton_editeurs[] = {'*', '.', 'B', ' ', 'P', 'F'};
const char *boutons_editeur_textes[] = {"Mur (A)", "Bille (Z)", "Bonbon (E)",
                                        "Vide (R)", "Pacman (T)", "Fantome (Y)"};
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

Partie charger_editeur() {
    Partie p;
    int err = charger_plan(chemin_fichier, &p);
    if (err == -1) {
        return partie_vide();
    }
    return p;
}

int sauvegarder_niveau(Partie *p) {
    static int en_sauvegarde = 0;
    if (en_sauvegarde) {
        return 1;
    }

    en_sauvegarde = 1;
    printf("Début de la sauvegarde\n");
    // Ouvre le fichier
    FILE *f = fopen(chemin_fichier, "w");
    if (f == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier\n");
        en_sauvegarde = 0;
        return 1;
    }

    // Ne pas sauvegarder si pacman n'est pas placé sur la grille
    if (p->pacman_place == 0) {
        printf("Pacman n'est pas sur la grille\n");
        fclose(f);
        en_sauvegarde = 0;
        return 1;
    }

    if (p->nbf < 4) {
        printf("Il manque des fantomes\n");
        fclose(f);
        en_sauvegarde = 0;
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
    en_sauvegarde = 0;
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
        if (p->pacman_place) return;
        p->pacman_place = 1;
    }
    if (celulle == 'P' && selected_char != 'P') {
        p->pacman_place = 0;
    }

    if (selected_char == 'F') {
        if (p->nbf == 4) return;
        p->nbf++;
    }
    if (celulle == 'F' && selected_char != 'F') {
        p->nbf--;
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
    sprintf(chemin_fichier, "data/maps/%s", chemin);
    Partie p = charger_editeur(chemin_fichier);
    calculer_voisins(&p);

    while (1) {
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
        else if (touche_a_ete_pressee(SDLK_q)) {
            changer_scene(SCENE_ACCUEIL);
            return;
        }

        dessiner_grille(&p, 1);
        entourner_case(souris); // Permet de montrer quelle case est survolée
        dessiner_boutons();
        actualiser();
        reinitialiser_evenements();
        attente(16); // Limite la vitesse du programme pour éviter une surutilisation du CPU
    }
}
