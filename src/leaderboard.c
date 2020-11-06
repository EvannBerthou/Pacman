#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <netdb.h>

#include "leaderboard.h"

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
    return NULL /* or throw or whatever */; \
  } \
  (*r) = retval; \
} while (0)


// Envoie une requete à un serveur et renvoie la réponse du serveur
char *envoyer_requete(const char *host, int port, const char *req) {

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
    char *reponse = calloc(total, sizeof(char));
    int recu = 0;
    do {
        int bytes;
        ERR(recv(sockfd, reponse + recu, total - recu, 0), "Erreur lors de la lecture", &bytes);
        if (bytes == 0) break; // Connection au serveur perdue
        recu += bytes;
    } while (recu < total);

    // Fermeture du socket
    close(sockfd);

#ifdef DEBUG
    printf("Envoyé : %s\nReçu : %s\n", req, reponse);
#endif

    return reponse;
}

void afficher_leaderboard() {
    const char *get_req = 
            "GET / HTTP/1.0\r\n"
            "Host: pacman-leaderboard.herokuapp.com\r\n"
            "Content-Type: application/x-www-form-urlencoded"
            "\r\n\r\n";

    //char *reponse = envoyer_requete("localhost", 3000, req);
    char *reponse = envoyer_requete("pacman-leaderboard.herokuapp.com", 80, get_req);
    // En cas d'erreur dans la requête
    if (reponse == NULL) {
        free(reponse);
        return;
    }

    char *body = strstr(reponse, "\r\n\r\n");
    char *state;
    char *line = strtok_r(body, "\r\n", &state);

    Point point_dessin = {0,0};
    while (line != NULL) {
        char *partie;
        char *joueur = strtok_r(line, ":", &partie);
        char *points = strtok_r(NULL, ":", &partie);
        afficher_ligne(joueur, points, &point_dessin);
        line = strtok_r(NULL, "\r\n", &state);
    }
    free(reponse);
}

void afficher_ligne(char *joueur, char *score, Point *point_dessin) {
#ifdef DEBUG
    printf("joueur : %s score : %s\n", joueur, score);
#endif
    afficher_texte(joueur, 26, *point_dessin, blanc);
    point_dessin->x += 150;
    afficher_texte(score, 26, *point_dessin, blanc);
    point_dessin->y += 20;
    point_dessin->x = 0;
    actualiser();
    attendre_clic();
}