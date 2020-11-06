/******************************************************************************/
/* MAIN.c                                                                     */
/******************************************************************************/
#include "./main.h"
#include "../lib/libgraphique.h"
#include "timer.h"
#include "leaderboard.h"

/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

SCENE scene_active = SCENE_ACCUEIL;

int main(int argc, char **argv) {
    // TODO: Devra être affiché sur l'écran d'accueil
    afficher_leaderboard();

    ouvrir_fenetre(600, 540);
    Timer timer = nouveau_timer();
    Partie p;
    while(1) {
        tick_timer(&timer);
#ifdef DEBUG
        print_fps(&timer);
#endif
        traiter_evenements();
        //actualiser_partie(&p, &timer);
        //dessiner_partie(&p);
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
        actualiser();
        break;
    case SCENE_NIVEAU:
        dessiner_partie(p);
        break;
    }
}

void actualiser_accueil(Partie *p, Timer *t) {
    if (touche_a_ete_pressee(SDLK_j)) {
        if (charger_niveau(p)) {
            printf("Erreur lors du chargement du niveau\n");
            exit(1);
        }
        scene_active = SCENE_NIVEAU;
        actualiser_partie(p, t);
    }
}

int charger_niveau(Partie *p) {
    /* Chargement du plan à partir du fichier fourni en paramètre                 */
    printf("Chargement du plan...\n");
    // TODO: Charger le niveau dynamiquement
    *p = charge_plan("data/test.txt");
    /* Si problème lors du chargement du plan...                                  */
    if(p->plateau == NULL)
        return 1;
    if (charger_sprites() == -1)
        return 1;
    calculer_voisins(p);

#ifdef DEBUG
    /* Affichage du plan lu                                                       */
    int i,j;
    printf("Affichage du plan...\n");
    for(i=0;i!=p->L;i++) {
        for(j=0;j!=p->C;j++)
            printf("%c",p->plateau[i][j]);
        printf("\n");
    }
    printf("Bonbons et bonus : %d\n", p->nbbonus);
#endif
    return 0;
}