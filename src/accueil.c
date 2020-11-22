// Include différent en fonction de la platforme car les API sont pas les mêmes
#ifndef __WIN32
#include <dirent.h>
#else
#include <windows.h>
#endif

#include "../lib/libgraphique.h"
#include "main.h"
#include "accueil.h"
#include "bouton.h"
#include "entrer.h"
#include "affichage.h"
#include "pacman.h"
#include "fantome.h"
#include "classement.h"
#include "editeur.h"
#include "partie.h"
#include "audio.h"

#define NOMBRE_BOUTONS 5
static BoutonAccueil boutons[NOMBRE_BOUTONS];
static int bouton_selectionne = 0;

void charger_boutons() {
    boutons[0] = nouveau_bouton((Point){ecran_w() / 2, 150}, blanc, "Jouer", 26);
    boutons[1] = nouveau_bouton((Point){ecran_w() / 2, 200}, blanc, "Classement", 26);
    boutons[2] = nouveau_bouton((Point){ecran_w() / 2, 250}, blanc, "Editeur", 26);
    boutons[3] = nouveau_bouton((Point){ecran_w() / 2, 300}, blanc, "Instructions", 26);
    boutons[4] = nouveau_bouton((Point){ecran_w() / 2, 350}, blanc, "Quitter", 26);
}

// Fonctions appelée à chaque frame
void actualiser_accueil(Partie *p, Timer *t) {
    int touche = nouvelle_touche();
    toggle_volume(touche);
    // Change le bouton selectionné lorsqu'une flèche est pressée
    if (touche == SDLK_DOWN) {
        bouton_selectionne = (bouton_selectionne + 1) % NOMBRE_BOUTONS;
    }
    else if (touche == SDLK_UP) {
        bouton_selectionne--;
        if (bouton_selectionne < 0)
            bouton_selectionne = NOMBRE_BOUTONS - 1;
    }
    // Si touche entrer, activer le bouton
    else if (touche == SDLK_RETURN) {
        activer_bouton(p, t);
    }
}

void activer_bouton(Partie *p, Timer *t) {
    // Animation de pacman qui mange le bouton avant de changer de menu
    manger_bouton();
    // Détermine l'action a faire en fonction du bouton pressé
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
        int w = p->C * CASE + 180;
        int h = p->L * CASE;
        changer_taille_fenetre(w, h);
        changer_scene(SCENE_NIVEAU);
        // Arrete la musique de l'accueil
        stop_son(0);
        // Charge le son de pacman mais ne le joue pas directement
        p->son_pacman = charger_fichier_audio(1);
        pause_son(p->son_pacman, 1);
        free(chemin);
        // Joue la musique d'intro
        jouer_intro(p, t);
        break;
    }
    // Classement
    case 1: afficher_classement(); break;
    // Editeur
    case 2: {
        char *chemin = selectionner_niveau();
        if (chemin == NULL) {
            break;
        }
        stop_son(0);
        lancer_editeur(chemin);
        free(chemin);
        break;
    }
    // Instructions
    case 3: afficher_instructions(); break;
    // Quitter
    case 4: exit(0); break;
    }
}

void dessiner_accueil() {
    dessiner_rectangle((Point){0, 0}, ecran_w(), ecran_h(), BG_COLOR);
    // Affiche le logo
    afficher_surface(sprite_index(0),(Point){ecran_w() / 2 - 100, 50});
    for (int i = 0; i < NOMBRE_BOUTONS; i++) {
        BoutonAccueil b = boutons[i];
        // Affiche le texte du bouton
        afficher_texte(b.texte, b.taille, (Point){b.rect.x, b.rect.y}, b.c);
        // Si le bouton en cours d'affichage est le bouton selectionné
        if (i == bouton_selectionne) {
            afficher_surface(sprite_pacman(3,0), (Point){b.rect.x - 30, b.rect.y + 7});
        }
    }
    afficher_bouton_selectionner();
}

void manger_bouton() {
    // Animations de pacman bouge ouverte/fermer
    SDL_Surface *images[] = {
        sprite_pacman(3, 0),
        sprite_pacman(3, 1),
    };
    BoutonAccueil b = boutons[bouton_selectionne];
    // Point de départ de pacman
    Point p = {b.rect.x - 30, b.rect.y + 7};
    Point debut = p;
    // Position en x d'arrivé de pacman une fois le bouton mangé
    int cible = b.rect.x + b.rect.w;
    // Déplacement de pacman entre debut et cible
    while (p.x < cible) {
        // Efface le passage de pacman
        dessiner_rectangle(debut, (p.x - debut.x + 10), 25, BG_COLOR);
        p.x += 1;
        // Change d'image toutes les 15 frames
        afficher_surface(images[p.x % 30 / 15], p);
        attente(5);
        actualiser();
    }
}

// Liste les fichiers dans un dossier
#ifndef __WIN32
// VERSION LINUX
int liste_fichiers(char **resultat) {
    int nb = 0; // nombre de niveaux dans la liste
    DIR *dossier;
    struct dirent *fichier;
    dossier = opendir("data/maps");
    if (dossier != NULL) {
        while ((fichier = readdir(dossier)) != NULL) {
            if (fichier->d_type == DT_REG) { // Si fichier (et non dossier)
                resultat[nb] = strdup(fichier->d_name);
                nb++;
            }
        }
        closedir(dossier);
        return nb;
    }
    return -1;
}
// VERSION WINDOWS
#else
int liste_fichiers(char **resultat) {
    int nb = 0;
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    WCHAR szDir[] = L"data/maps";
    size_t length_of_arg;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;

    hFind = FindFirstFile("data\\maps\\*.*", &ffd);

    do {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            resultat[nb++] = strdup(ffd.cFileName);
        }
    } while (FindNextFile(hFind, &ffd) != 0);
    FindClose(hFind);

    // Si aucu fichier trouver
    if (nb == 0) {
        return -1;
    }
    return nb;
}
#endif

// Menu pour selectionner quel niveau charger
char* selectionner_niveau() {
    // Chargement des fichiers
    char *niveaux[100]; // TODO: Probleme de taille fixe
    int nb;
    nb = liste_fichiers(niveaux);
    // Si aucun fichier dans le dossier data/maps
    if (nb == -1) {
        return NULL;
    }

    int curseur = 0; // Quel niveau est selectionné
    while (1) {
        traiter_evenements();
        int touche = nouvelle_touche();
        toggle_volume(touche);
        // Déplacement du curseur avec looping
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
            // Libère tous les noms 
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

// Affiche le nom des fichiers sur un interval 
void afficher_liste_niveaux(char **liste, int n, int curseur) {
    const int limit = 15;
    const int font = 26;
    dessiner_rectangle((Point){0,0}, ecran_w(), ecran_h(), BG_COLOR);

    // Par défaut on affiche depuis le début
    int debut = 0;
    // Si la curseur est plus loin que la limit affichable
    if (curseur >= limit) {
        // Décalé le début pour afficher la suite
        debut = curseur - limit + 1;
    }

    // Affiche les nom entre liste[debut:debut+10]
    for (int i = 0; i < limit; i++) {
        if (debut + i >= n) break;
        Point p = {0, font * i + 10};
        Couleur c = (debut + i) == curseur ? rouge : blanc; // Rouge si selectionnné, sinon blanc
        afficher_texte(liste[debut + i], font, p, c);
    }
}

void charger_accueil() {
    bouton_selectionne = 0;
    pause_son(0, 1);
    changer_taille_fenetre(600, 540);
}

// Affiche le menu d'instructions
void afficher_instructions() {
    dessiner_rectangle((Point){0,0}, ecran_w(), ecran_h(), BG_COLOR);
    const int taille_titre = 36;
    const int taille_texte = 18;
    // Position du texte à afficher en y
    int avance_y = 0;

    char *texte_instructions = "Instructions";
    avance_y += 20;
    // Affiche le titre centré
    afficher_texte(texte_instructions, taille_titre,
            centrer_texte(texte_instructions, (Point){ecran_w() / 2, avance_y}, taille_titre),
            blanc);
    
    char *texte_regles[] = {
        "Pac-man se retrouve dans un labyrinthe, avec des fantômes.",
        "Manger tous les bonus du niveau avant de se faire attraper.",
        "Vous disposez de 3 vies pour remplir cette mission",
    };

    avance_y += 20;

    for (int i = 0; i < 3; i++) {
        avance_y += taille_texte;
        afficher_texte(texte_regles[i], taille_texte, (Point){0, avance_y}, blanc);
    }

    // Affiche les controles
    avance_y += 40;
    afficher_texte("Jeu", taille_titre, (Point){0, avance_y}, blanc);
    afficher_texte("Editeur", taille_titre, (Point){ecran_w() / 2, avance_y}, blanc);

    avance_y += 40;

    char *texte_touches[] = {
        "Deplacement : Flèches",
        "Retour : Q",
        "Confirmer : Entrer",
        "(Dés)activer le son : M",
    };

    char *texte_touches_editeur[] = {
        "Mur : A",
        "Bille : Z",
        "Bonbon : E",
        "Vide : R",
        "Pacman : T",
        "Fantome : Y",
    };

    // Garde en mémoire avance_y avant l'affichage des controles pour ensuite mettre les controles dans
    // l'éditeur à côté
    int pre_touches = avance_y;
    for (int i = 0; i < 4; i++) {
        avance_y += taille_texte;
        afficher_texte(texte_touches[i], taille_texte, (Point){0, avance_y}, blanc);
    }

    avance_y = pre_touches;
    for (int i = 0; i < 6; i++) {
        avance_y += taille_texte;
        afficher_texte(texte_touches_editeur[i], taille_texte, (Point){ecran_w() / 2, avance_y}, blanc);
    }

    // Attendre que l'utilisateur est fini pour continuer
    afficher_bouton_retour();
    actualiser();
    attendre_sortie();
}