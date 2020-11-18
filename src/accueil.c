#include <dirent.h>
#include "../lib/libgraphique.h"
#include "main.h"
#include "accueil.h"
#include "Bouton.h"
#include "entrer.h"
#include "affichage.h"
#include "pacman.h"
#include "fantome.h"
#include "leaderboard.h"
#include "editeur.h"
#include "partie.h"
#include "audio.h"

#define NOMBRE_BOUTONS 4
static BoutonAccueil boutons[NOMBRE_BOUTONS];
static int bouton_selectionne = 0;

void charger_boutons() {
    boutons[0] = nouveau_bouton((Point){ECRAN_W / 2, 200}, blanc, "Jouer", 26);
    boutons[1] = nouveau_bouton((Point){ECRAN_W / 2, 250}, blanc, "Classement", 26);
    boutons[2] = nouveau_bouton((Point){ECRAN_W / 2, 300}, blanc, "Editeur", 26);
    boutons[3] = nouveau_bouton((Point){ECRAN_W / 2, 350}, blanc, "Quitter", 26);
}

void actualiser_accueil(Partie *p, Timer *t) {
    int touche = nouvelle_touche();
    if (touche == SDLK_DOWN) {
        bouton_selectionne = (bouton_selectionne + 1) % NOMBRE_BOUTONS;
    }
    else if (touche == SDLK_UP) {
        bouton_selectionne--;
        if (bouton_selectionne < 0)
            bouton_selectionne = NOMBRE_BOUTONS - 1;
    }
    else if (touche == SDLK_RETURN) {
        activer_bouton(p, t);
    }
}

void activer_bouton(Partie *p, Timer *t) {
    // Animation de pacman qui mange le bouton avant de changer de menu
    manger_bouton();
    // Transition écran noir
    switch(bouton_selectionne) {
    case 0: {
        char *chemin = selectionner_niveau();
        if (chemin == NULL) {
            break;
        }
        // Charge le niveau
        if (charger_niveau(p, chemin)) {
            printf("Erreur lors du chargement du niveau\n");
            free(chemin);
            break;
        }
        changer_scene(SCENE_NIVEAU);
        // Arrete la musique de l'accueil
        stop_son(0);
        printf("%d\n", charger_fichier_audio(1));
        free(chemin);
        break;
    }
    case 1: afficher_leaderboard(); break;
    case 2: {
        char *chemin = selectionner_niveau();
        if (chemin == NULL) {
            break;
        }
        lancer_editeur(chemin);
        free(chemin);
        break;
    }
    case 3: exit(0); break;
    }
}

void dessiner_accueil() {
    dessiner_rectangle((Point){0, 0}, 800, ECRAN_W, noir);
    // TODO: Dessiner un logo au lieu d'un texte
    afficher_texte("Pacman", 46, (Point){800 / 2 - 26 * 7, 50}, jaune);
    for (int i = 0; i < NOMBRE_BOUTONS; i++) {
        BoutonAccueil b = boutons[i];
        afficher_texte(b.texte, b.taille, (Point){b.rect.x, b.rect.y}, b.c);
        // Si le bouton en cours d'affichage est le bouton selectionné
        if (i == bouton_selectionne) {
            afficher_image("data/sprites/pacman30.bmp", (Point){b.rect.x - 30, b.rect.y + 7});
        }
    }
    afficher_bouton_selectionner();
    actualiser();
}

void manger_bouton() {
    SDL_Surface *images[] = {
        sprite_pacman(3, 0),
        sprite_pacman(3, 1),
    };
    BoutonAccueil b = boutons[bouton_selectionne];
    Point p = {b.rect.x - 30, b.rect.y + 7};
    Point debut = p;
    int cible = b.rect.x + b.rect.w;
    while (p.x < cible) {
        dessiner_rectangle(debut, (p.x - debut.x + 10), 25, noir);
        p.x += 1;
        // Change d'image toutes les 15 frames
        afficher_surface(images[p.x % 30 / 15], p);
        attente(5);
        actualiser();
    }
}

int charger_niveau(Partie *p, char *chemin) {
    /* Chargement du plan à partir du fichier fourni en paramètre                 */
    printf("Chargement du plan...\n");
    char chemin_complet[100];
    sprintf(chemin_complet, "data/maps/%s", chemin);
    vider_partie(p);
    int err = charger_plan(chemin_complet, p);
    if (err == -1) {
        charger_accueil();
        return 1;
    }
    else {
        calculer_voisins(p);
        reset_timer_fantomes();
    }

#ifdef DEBUG
    /* Affichage du plan lu                                                       */
    int i, j;
    printf("Affichage du plan...\n");
    for (i = 0; i != p->L; i++) {
        for (j = 0; j != p->C; j++)
            printf("%c", p->plateau[i][j]);
        printf("\n");
    }
    printf("Bonbons et bonus : %d\n", p->nbbonus);
#endif
    return 0;
}

char* selectionner_niveau() {
    // Chargement des fichiers
    char *niveaux[100]; // TODO: Probleme de taille fixe
    int nb = 0; // nombre de niveaux dans la liste
    DIR *dossier;
    struct dirent *fichier;
    dossier = opendir("data/maps");
    if (dossier != NULL) {
        while ((fichier = readdir(dossier)) != NULL) {
            if (fichier->d_type == DT_REG) { // Si fichier (et non dossier)
                niveaux[nb] = strdup(fichier->d_name);
                nb++;
            }
        }
        closedir(dossier);
    }
    else {
        return NULL;
    }

    int curseur = 0; // Quel niveau est selectionné
    while (1) {
        traiter_evenements();
        int touche = nouvelle_touche();
        if (touche == SDLK_DOWN) {
            curseur = (curseur + 1) % nb;
        }
        else if (touche == SDLK_UP) {
            curseur--;
            if (curseur < 0) curseur = nb - 1;
        }
        else if (touche == SDLK_RETURN) {
            // Garde en mémoire le niveau selectionne pour pas qu'il se fasse free
            char *selectionne = strdup(niveaux[curseur]);
            // libère la mémoire alloué pour les noms de fichier
            for (int i = 0; i < nb; i++) {
                free(niveaux[i]);
            }
            return selectionne;
        }
        else if (touche == SDLK_q) {
            for (int i = 0; i < nb; i++) {
                free(niveaux[i]);
            }
            return NULL;
        }

        afficher_liste_niveaux(niveaux, nb, curseur);
        afficher_bouton_selectionner();
        afficher_bouton_retour();
        actualiser();
        reinitialiser_evenements();
    }
}

void afficher_liste_niveaux(char **liste, int n, int curseur) {
    const int font = 26;
    dessiner_rectangle((Point){0,0}, ECRAN_W, ECRAN_H, noir);
    for (int i = 0; i < n; i++) {
        Point p = {0, font * i + 10};
        Couleur c = i == curseur ? rouge : blanc;
        afficher_texte(liste[i], font, p, c);
    }
}

void charger_accueil() {
     bouton_selectionne = 0;
}
