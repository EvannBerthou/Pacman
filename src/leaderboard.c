/*
Code de socket inspiré de ce gist pour plus de portabilité
https://gist.github.com/FedericoPonzi/2a37799b6c601cce6c1b
*/

#ifdef __WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "leaderboard.h"
#include "entrer.h"
#include "affichage.h"

/*
Ces macros servent a quitter la fonction envoyer_requete en cas d'erreur dans la requête
elles permettent de direction renvoyer NULL en cas d'erreur et diminue la quantité de code répété
pour la vérification de chaque erreur dans la fonction
*/

#define ERR(x, msg, r) do { \
  int retval = (x); \
  if (retval < 0) { \
    fprintf(stderr, msg); \
    close(sockfd); \
    CleanSocket(); \
    return NULL /* or throw or whatever */; \
  } \
  if (r != NULL) { \
    (*r) = retval; \
  } \
} while (0)

#define ERRP(x, msg, r) do { \
  void *retval = (x); \
  if (retval == NULL) { \
    fprintf(stderr, msg); \
    close(sockfd); \
    CleanSocket(); \
    return NULL /* or throw or whatever */; \
  } \
  (*r) = retval; \
} while (0)

static void CleanSocket() {
#ifdef __WIN32
    WSACleanup();
#endif
}

// Extrait le code de statut de la réponse (code 200 = ok)
int status(char *reponse) {
    char *statut = strstr(reponse, " "); // Trouve le premier espace
    int res = 0;
    sscanf(statut, "%d", &res);
    return res;
}
// Envoie une requete à un serveur et renvoie la réponse du serveur
char *envoyer_requete(const char *host, int port, const char *req) {
#if defined WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2 ,2), &wsaData) != 0) {
        printf("Erreur dans WSAStartup\n");
        return NULL;
    }
#endif

    // Création du socket
    int sockfd;
    ERR(socket(AF_INET, SOCK_STREAM, 0), "Erreur lors de la création du socket", &sockfd);
    // Trouver l'ip du serveur par rapport à son host
    struct hostent *server;
    ERRP(gethostbyname(host), "Serveur non trouvé", &server);

    // Initialisation du socket pour qu'il se connecte au serveur
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr.s_addr,server->h_addr,server->h_length);

    // Connection au serveur
    int res;
    ERR(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr)), "Erreur lors de la connection", &res);

    // Envoie de la requete tout en gérant les éventuelles paquets perdus
    int total = strlen(req);
    int envoye = 0;
    do {
        int bytes;
        ERR(send(sockfd, req + envoye, total - envoye, 0), "Erreur lors de l'envoie", &bytes);
        if (bytes == 0) break; // Connection au serveur perdue
        envoye+=bytes;
    } while (envoye < total);

    // Réponse du serveur
    total = 4096;
    char *reponse = calloc(total, sizeof(char)); // Memory leak, reponse n'est pas free en cas d'erreur dans recv
    int recu = 0;
    do {
        int bytes;
        ERR(recv(sockfd, reponse + recu, total - recu, 0), "Erreur lors de la lecture", &bytes);
        if (bytes == 0) break; // Connection au serveur perdue
        recu += bytes;
    } while (recu < total);

    // Fermeture du socket
    closesocket(sockfd);

#ifdef DEBUG
    printf("Envoyé : %s\nReçu : %s\n", req, reponse);
#endif

    if (status(reponse) != 200) {
        free(reponse);
        return NULL;
    }

    return reponse;
}

Point centrer_texte(char *texte, Point centre, int taille) {
    Point t = taille_texte(texte, taille);
    return (Point) {centre.x - (t.x / 2), centre.y - (t.y / 2)};
}

void afficher_leaderboard() {
    afficher_message_leaderboard("Chargement du classement", 26);
    const char *get_req =
            "GET / HTTP/1.0\r\n"
            "Host: pacman-leaderboard.herokuapp.com\r\n"
            "Content-Type: application/x-www-form-urlencoded"
            "\r\n\r\n";

    //char *reponse = envoyer_requete("localhost", 3000, get_req);
    char *reponse = envoyer_requete("pacman-leaderboard.herokuapp.com", 80, get_req);
    // En cas d'erreur dans la requête
    if (reponse == NULL) {
        afficher_message_leaderboard("Erreur lors du chargement du classement", 26);
        afficher_bouton_retour();
        actualiser();
        attendre_sortie();
        return;
    }

    // Récupère que le corps de la réponse sans l'en-tête
    char *body = strstr(reponse, "\r\n\r\n");

    // Efface l'écran
    dessiner_rectangle((Point){0,0}, ECRAN_W, ECRAN_H, noir);

    const int taille_titre = 46;
    afficher_texte("Joueur", taille_titre, centrer_texte("Joueur", (Point){ECRAN_W / 4, 20}, taille_titre), blanc);
    afficher_texte("Score", taille_titre, centrer_texte("Score", (Point){ECRAN_W / 2 + 300 / 2, 20}, taille_titre), blanc);
    // Point de départ du tableau du classement
    int y = 75;
    // Sépare chaque ligne du corps
    char *state;
    char *line = strtok_r(body, "\r\n", &state);
    while (line != NULL) {
        char *partie;
        // Sépare le nom du joueur et son score
        char *joueur = strtok_r(line, ":", &partie);
        char *points = strtok_r(NULL, ":", &partie);
        // Affiche la ligne
        afficher_ligne(joueur, points, y);
        line = strtok_r(NULL, "\r\n", &state);
        y += 45;
    }
    afficher_bouton_retour();
    actualiser();
    attendre_sortie();
    free(reponse);
}

void afficher_ligne(char *joueur, char *score, int y) {
#ifdef DEBUG
    printf("joueur : %s score : %s\n", joueur, score);
#endif
    const int taille_font = 26;
    afficher_texte(joueur, taille_font, centrer_texte(joueur, (Point){ECRAN_W / 4, y}, taille_font), blanc);
    afficher_texte(score, taille_font, centrer_texte(score, (Point){ECRAN_W / 2 + 300 / 2, y}, taille_font), blanc);
}

void afficher_message_leaderboard(char *message, int font) {
    dessiner_rectangle((Point){0,0}, ECRAN_W, ECRAN_H, noir);
    afficher_texte(message, font, centrer_texte(message, (Point){ECRAN_W / 2, ECRAN_H / 2}, font), blanc);
}

void envoyer_score(Partie *p) {
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