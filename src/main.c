/******************************************************************************/
/* MAIN.c                                                                     */
/******************************************************************************/
#include "./main.h"
#include "../lib/libgraphique.h"
#include "timer.h"
#include "leaderboard.h"
#include "Bouton.h"
#include "entite.h"

/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

SCENE scene_active = SCENE_ACCUEIL;
#define NOMBRE_BOUTONS 3
BoutonAccueil boutons[NOMBRE_BOUTONS];
int bouton_selectionne = 0;

int main(int argc, char **argv) {
    if (charger_sprites() == -1)
        return 1;

    // Création des boutons
    boutons[0] = nouveau_bouton((Point){600 / 2, 200}, blanc, "Jouer", 26);
    boutons[1] = nouveau_bouton((Point){600 / 2, 250}, blanc, "Classement", 26);
    boutons[2] = nouveau_bouton((Point){600 / 2, 300}, blanc, "Quitter", 26);

    ouvrir_fenetre(600, 540);
    Timer timer = nouveau_timer();
    Partie p;
    while(1) {
        tick_timer(&timer);
#ifdef DEBUG
        print_fps(&timer);
#endif
        traiter_evenements();
        actualiser_jeu(&p, &timer);
        dessiner_jeu(&p);
        reinitialiser_evenements();
    }
    return 0;
}

void actualiser_jeu(Partie *p, Timer *t) {
    switch(scene_active) {
    case SCENE_ACCUEIL:
        actualiser_accueil(p, t);
        break;
    case SCENE_NIVEAU:
        actualiser_partie(p, t);
        break;
    }
}

void dessiner_jeu(Partie *p) {
    switch(scene_active) {
    case SCENE_ACCUEIL:
        dessiner_accueil();
        break;
    case SCENE_NIVEAU:
        dessiner_partie(p);
        break;
    }
}

void actualiser_accueil(Partie *p, Timer *t) {
    // Evite la répétition de touche
    static int derniere_touche = 0;
    int touche = attendre_touche_duree(10);
    if (touche == derniere_touche) return;
    derniere_touche = touche;

    if (touche == SDLK_DOWN) {
        bouton_selectionne = (bouton_selectionne + 1) % NOMBRE_BOUTONS;
    }

    if (touche == SDLK_UP) {
        bouton_selectionne--;
        if (bouton_selectionne < 0)
            bouton_selectionne = NOMBRE_BOUTONS - 1;
    }

    if (touche == SDLK_RETURN) {
        activer_bouton(p, t);
    }
}

void activer_bouton(Partie *p, Timer *t) {
    manger_bouton();
    // Transition écran noir
    switch(bouton_selectionne) {
    case 0:
        if (charger_niveau(p)) {
            printf("Erreur lors du chargement du niveau\n");
            exit(1);
        }
        scene_active = SCENE_NIVEAU;
        actualiser_jeu(p, t);
        break;
    case 1: afficher_leaderboard(); break;
    case 2: exit(0); break;
    }
}

void dessiner_accueil() {
    dessiner_rectangle((Point){0, 0}, 800, 600, noir);
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
    actualiser();
}

void manger_bouton() {
    SDL_Surface *images[] = {
        charger_sprite(ENTITE_PACMAN, 3, 0),
        charger_sprite(ENTITE_PACMAN, 3, 1),
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

int charger_niveau(Partie *p) {
    /* Chargement du plan à partir du fichier fourni en paramètre                 */
    printf("Chargement du plan...\n");
    // TODO: Charger le niveau dynamiquement
    *p = charge_plan("data/test.txt");
    /* Si problème lors du chargement du plan...                                  */
    if (p->plateau == NULL)
        return 1;
    calculer_voisins(p);

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
