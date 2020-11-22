# Projet Pac-man
## Groupe : Evann Berthou, Benjamin Pellieux

# Compte rendu
## Présentation
Pac-man est un jeu dans lequel nous contrôlons un petite boule jaune dans un labyrinthe rempli
de Pac-gomme. Sauf que Pac-man n'est pas seul, des fantômes sont aussi présents. Les fantômes
partent à la poursuite de Pac-man et s'il se fait toucher trop de fois, la partie se termine.

## Utilisation
### Compilation
```
make : compile le jeu en mode normal
make debug : compile le jeu en mode debug, permet l'utilisation de gdb, plus de print
make windows : compile le jeu pour windows
./resultat : lancer le jeu
```

### En jeu
Lorsque le jeu se lance, le joueur se retrouve sur un écran d'accueil, sur lequel il a le choix
entre 4 options

- Jouer : Lancer le premier niveau
- Classement : Affiche le classement du meilleur score des joueurs
- Editeur : Lance l'éditeur de niveau
- Quitter : Quitte le jeu

### Déroulement d'une partie
Lorsque le joueur lance une partie, il dout déplacer Pac-man à l'aide des flèches du clavier,
afin de ramasser les Pac-gomme qui augmenteront son score. Pendant ce temps, les fantômes partent à la poursuite de pacman en utilisant l'algorithme A\* afin de déterminer le chemin le plus court pour l'atteindre. Cependant la taille du plateau n'étant pas particulièrement grande, nous avons legerement modifier le code pour obtenir un Breadth-first search (BFS).

En cas de collision entre un fantôme et Pac-man, deux évènements peuvent se produire.
Si Pac-man n'a pas mangé de super Pac-gomme, alors il perd une vie et retourne à sa position d'origine.
La partie reprend s'il lui reste des vies (3 au lancement du niveau), sinon elle se termine.
Si Pac-man à mangé un super Pac-gomme récemment, alors le fantôme se fait manger et il retourne à sa position
d'origine.

Une fois que pacman à mangé toutes les Pac-gommes, le niveau se relance avec une difficulté accrue. Lors du premier niveau seulement 2 fantômes sont présent. Une fois le premier niveau terminé, les 4 fantômes sont maintenant présents sur le plateau. A chaque relancement, la vitesse de tous les fantômes augmente.

Quand pacman a épuisé ses 3 vies, la partie est terminée, le joueur a alors la possibilité d'entrer un pseudo à 5 lettres à l'aide des flèches du clavier ou de la manette. Ce score est ensuite envoyé dans le classement.

### Classement
Le classement est un moyen qui permet de donner envie aux joueurs de s'améliorer afin de battre le score des autres joueurs. Lorsque la partie se termine et que le joueur a selectioné son pseudo, son score et son pseudo sont envoyés vers un serveur à l'aide d'une requête HTTP. Se serveur va ensuite traiter la requête reçu et déterminer si le score doit être ajouté. Le score est ajouté au classement seulement si le nouveau score est supérieur au score. Le jeu ne disposant pas de compte, n'importe qui peut choisir le score de quelqu'un d'autre lors d'une fin de partie.

Les scores étant stockés sur un serveur externe au jeu, tous les joueurs voient le même classement, pourvu qu'ils disposent d'une connexion internet.

Le serveur est un serveur NodeJS, hébergé sur le plan gratuit d'Heroku et disponible à l'adresse
<https://pacman-leaderboard.herokuapp.com>

Ce système ne possède aucune sécurité, il est tout à fait possible d'envoyer une requête manuellement afin d'ajouter n'importe quel score. Nous n'avons pas passé beaucoup de temps sur cette partie car nous ne la jugions pas prioritaire par rapport aux jeu.

### Editeur
L'éditeur permet à tout le monde de fabriquer son propre niveau. En choisisant le type de case
qu'il veut poser à l'aide des touches A à Y, il peut cliquer et modifier chaque case
de la grille. Son type de case selectioné s'affiche en rouge sur la droite. Une fois la
création du niveau finie, appuyer sur S permet de sauvegarder. La touche Q permet de quitter l'éditeur.

# Difficultés
La plus grosse difficulté lors de ce projet à été le système de pathfinding. On ne voulait pas utiliser l'algorithme proposé dans le sujet car on trouvait sa théorie trop simple. On s'est donc dirigé vers l'algorithme A\* pour plus de difficulté. Comme prévu, cela nous a posé quelques problèmes notamment au niveau des cas particuliers.

# Fonctionnalités
## Sujet
- Afficher le niveau en mode graphique
- Afficher pacman et les fantômes
- Gérer les déplacements de pacman/fantômes en temps réel
- Bonbons/bonus
- Tunnels pour passer d'un côté du plateau à l'autre
- IA des fantômes
- Fantômes fuient quand pacman a mangé un bonus
- Fantômes qui disparaissent quand pacman les mange en mode bonus
- Réapparition des fantômes
- Différents niveaux

## Idées originales
- Classement en ligne (fait des requêtes à un serveur)
- Ecran d'accueil
- Utilisation d'images
- Editeur de niveaux
- Régulation de la vitesse du jeu afin d'assurer aucun changement dans les déplacements peu importe la vitesse d'exécution du programme.
- Déplacement lisse des entités (pas de téléportation entre les cases)
- Animations
- Support manette
- Musique/sons
- Support natif de Windows
- Nombre de fantomes dynamique
- Augmentation du score pour chaque fantôme mangé avec le même Pacgum
- Taille de plateau dynamique
- Support du konami code !

## Pas implémentés
- [ ] Création d'un nouveau fichier depuis l'éditeur (on est obligé de créer manuellement un fichier vide dans data/maps puis ensuite de le charger en jeu)
- [ ] Le jeu à des soucis lorsque les FPS sont très bas ( > 10) : certains fantômes n'arrivent pas à sortir de la grotte


# Crédits

- [Sprites fantomes et pacman](https://www.spriters-resource.com/arcade/pacman/)
- Sprites des murs faits à la main
- [Sons](https://www.classicgaming.cc/classics/pac-man/sounds)
