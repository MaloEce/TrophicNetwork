#include "Header.h"

//ici ce trouve tout les programmes en dehors de la simulation

// Charger le reseau trophique depuis un fichier
ReseauTrophique* chargerReseau(const char *nomFichier) {
    FILE *fichier = fopen(nomFichier, "r");
    if (!fichier) {
        perror("Erreur d'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    ReseauTrophique *reseau = malloc(sizeof(ReseauTrophique));
    reseau->nbSommets = 0;
    reseau->nbArcs = 0;

    char ligne[256];
    // Compter le nombre de sommets et d'arcs
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == '/' && ligne[1] == '/') {
            continue; // Ignorer les lignes de commentaire
        } else if (strchr(ligne, '"')) {
            reseau->nbSommets++;
        } else if (ligne[0]) {
            reseau->nbArcs++;
        }
    }

    // Allouer de la memoire pour les sommets et les arcs
    reseau->sommets = malloc(reseau->nbSommets * sizeof(Sommet));
    reseau->arcs = malloc(reseau->nbArcs * sizeof(Arc));

    rewind(fichier); // Revenir au debut du fichier pour avoir les autre donnees

    int sommetIndex = 0, arcIndex = 0;


    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == '/' && ligne[1] == '/') {
            continue;
        }

        if (strchr(ligne, '"')) {
            // Lire un sommet
            sscanf(ligne, "%d \"%[^\"]\" %s %d",
                   &reseau->sommets[sommetIndex].id,
                   reseau->sommets[sommetIndex].nom,
                   reseau->sommets[sommetIndex].type,
                   &reseau->sommets[sommetIndex].biomasse);
            sommetIndex++;
        } else if (ligne[0]) {
            // Lire un arc
            sscanf(ligne, "%d %d %f",
                   &reseau->arcs[arcIndex].source,
                   &reseau->arcs[arcIndex].cible,
                   &reseau->arcs[arcIndex].poids);
            arcIndex++;
        }
    }

    fclose(fichier);
    return reseau;
}
void dfsComposante(ReseauTrophique *reseau, int sommet, int composanteId, int *composantes) { //dfs pour connaitre la connexite du graphe
    composantes[sommet] = composanteId;

    for (int i = 0; i < reseau->nbArcs; i++) {
        if (reseau->arcs[i].source == sommet + 1 && composantes[reseau->arcs[i].cible - 1] == -1) {
            dfsComposante(reseau, reseau->arcs[i].cible - 1, composanteId, composantes);
        }
        if (reseau->arcs[i].cible == sommet + 1 && composantes[reseau->arcs[i].source - 1] == -1) {
            dfsComposante(reseau, reseau->arcs[i].source - 1, composanteId, composantes);
        }
    }
}

int identifierComposantes(ReseauTrophique *reseau, int *composantes) {
    int composanteId = 0;
    for (int i = 0; i < reseau->nbSommets; i++) {
        composantes[i] = -1; // Initialisation : aucun sommet visite
    }

    for (int i = 0; i < reseau->nbSommets; i++) {
        if (composantes[i] == -1) { // Trouver une nouvelle composante
            dfsComposante(reseau, i, composanteId, composantes);
            composanteId++;
        }
    }
    return composanteId;
}

ReseauTrophique *extraireComposante(ReseauTrophique *reseau, int *composantes, int cibleComposante) {
    // Compter les sommets et les arcs appartenant a la composante cible
    int nbSommets = 0, nbArcs = 0;
    for (int i = 0; i < reseau->nbSommets; i++) {
        if (composantes[i] == cibleComposante) {
            nbSommets++;
        }
    }
    for (int i = 0; i < reseau->nbArcs; i++) {
        if (composantes[reseau->arcs[i].source - 1] == cibleComposante &&
            composantes[reseau->arcs[i].cible - 1] == cibleComposante) {
            nbArcs++;
        }
    }

    // Allouer le nouveau reseau
    ReseauTrophique *nouveauReseau = malloc(sizeof(ReseauTrophique));
    nouveauReseau->sommets = malloc(nbSommets * sizeof(Sommet));
    nouveauReseau->arcs = malloc(nbArcs * sizeof(Arc));
    nouveauReseau->nbSommets = nbSommets;
    nouveauReseau->nbArcs = nbArcs;

    // Copier les sommets

    int sommetIndex = 0;
    int *idMapping = malloc(reseau->nbSommets * sizeof(int)); // Table de correspondance
    for (int i = 0; i < reseau->nbSommets; i++) {
        if (composantes[i] == cibleComposante) {
            nouveauReseau->sommets[sommetIndex] = reseau->sommets[i];
            idMapping[reseau->sommets[i].id - 1] = sommetIndex + 1; // Correspondance ancien -> nouveau ID
            nouveauReseau->sommets[sommetIndex].id = sommetIndex + 1; // Nouveau ID
            sommetIndex++;
        }
    }

    // Copier les arcs avec mise a jour des IDs
    int arcIndex = 0;
    for (int i = 0; i < reseau->nbArcs; i++) {
        if (composantes[reseau->arcs[i].source - 1] == cibleComposante &&
            composantes[reseau->arcs[i].cible - 1] == cibleComposante) {
            nouveauReseau->arcs[arcIndex].source = idMapping[reseau->arcs[i].source - 1];
            nouveauReseau->arcs[arcIndex].cible = idMapping[reseau->arcs[i].cible - 1];
            nouveauReseau->arcs[arcIndex].poids = reseau->arcs[i].poids;
            arcIndex++;
        }
    }

    free(idMapping); // Liberer la table de correspondance
    return nouveauReseau;

    return nouveauReseau;
}



void connexite(ReseauTrophique **reseau) {
    int *composantes = malloc((*reseau)->nbSommets * sizeof(int));  // Allocation correcte
    int nbComposantes = identifierComposantes(*reseau, composantes);

    if (nbComposantes == 1) {
        printf("\nLe graphe est connexe.\n");
    } else {
        printf("\nLe graphe n'est pas connexe. Il y a %d composantes connexes.\n", nbComposantes);

        for (int i = 0; i < nbComposantes; i++) {
            printf("Composante %d :\n", i + 1);
            for (int j = 0; j < (*reseau)->nbSommets; j++) {
                if (composantes[j] == i) {
                    printf("  [%d] %s (%s)\n", (*reseau)->sommets[j].id, (*reseau)->sommets[j].nom, (*reseau)->sommets[j].type);
                }
            }
        }

        // Demander a l'utilisateur s'il veut etudier une composante
        int choix;
        printf("\nEntrez le numero de la composante a etudier (0 pour quitter) : ");
        scanf("%d", &choix);

        if (choix > 0 && choix <= nbComposantes) {
            // Extraire la composante choisie
            ReseauTrophique *nouveauReseau = extraireComposante(*reseau, composantes, choix - 1);
            printf("\nVous avez choisi d'etudier la composante %d :\n", choix);
            // Liberer l'ancien réseau et mettre à jour le pointeur
            free(*reseau);
            *reseau = nouveauReseau;  // Remplacement du reseau principal par le nuveau reseau extrait
        } else {
            printf("\nAucune action effectuee.\n");
        }
    }

    free(composantes);  // Liberer la memoire allouee pour les composantes
}


// Afficher le reseau trophique
void sucpre(ReseauTrophique *reseau) {
    printf("\n--- Relations d'adjacence ---\n");
    for (int i = 0; i < reseau->nbSommets; i++) {
        printf("\nSommet [%d] %s (%s) :\n", reseau->sommets[i].id, reseau->sommets[i].nom, reseau->sommets[i].type);
        printf("  Successeur : ");
        int aucunSuccesseur = 1;
        for (int j = 0; j < reseau->nbArcs; j++) {
            if (reseau->arcs[j].source == reseau->sommets[i].id) {
                printf("[%d] %s, ", reseau->arcs[j].cible, reseau->sommets[reseau->arcs[j].cible - 1].nom);
                aucunSuccesseur = 0;
            }
        }
        if (aucunSuccesseur) printf("Aucun");
        printf("\n  Predecesseurs : ");
        int aucunPredecesseur = 1;
        for (int j = 0; j < reseau->nbArcs; j++) {
            if (reseau->arcs[j].cible == reseau->sommets[i].id) {
                printf("[%d] %s, ", reseau->arcs[j].source, reseau->sommets[reseau->arcs[j].source - 1].nom);
                aucunPredecesseur = 0;
            }
        }
        if (aucunPredecesseur) printf("Aucun");
        printf("\n");
    }
}
// les sommet avec un ppredecesseurs
void unpre(ReseauTrophique *reseau) {
    printf("\n--- Especes n'ayant qu'une source d'alimentation ---\n");

    for (int i = 0; i < reseau->nbSommets; i++) {
        // Compter le nombre de predecesseurs du sommet actuel
        int nbPredecesseurs = 0;

        for (int j = 0; j < reseau->nbArcs; j++) {
            if (reseau->arcs[j].cible == reseau->sommets[i].id) {
                nbPredecesseurs++;
            }
        }

        // Afficher uniquement les sommets ayant exactement un predecesseur
        if (nbPredecesseurs == 1) {
            printf("\nSommet [%d] %s (%s) :\n", reseau->sommets[i].id, reseau->sommets[i].nom, reseau->sommets[i].type);
            printf("  Predecesseur : ");

            for (int j = 0; j < reseau->nbArcs; j++) {
                if (reseau->arcs[j].cible == reseau->sommets[i].id) {
                    printf("[%d] %s\n",
                           reseau->arcs[j].source,
                           reseau->sommets[reseau->arcs[j].source - 1].nom
                    );
                    break; // On affiche le seul predecesseur trouve
                }
            }
        }
    }
}

// Fonction DFS inversee pour explorer tous les chemins menant jusqu'a la cible
void dfsInverse(ReseauTrophique *reseau, int sommetActuel, int chemin[], int index) {
    chemin[index] = sommetActuel; // Enregistrer le sommet actuel dans le chemin

    // Verifier s'il existe des arcs menant a ce sommet
    int trouvePred = 0;
    for (int i = 0; i < reseau->nbArcs; i++) {
        if (reseau->arcs[i].cible == sommetActuel) {
            trouvePred = 1; // Si un predecesseur sortir
            dfsInverse(reseau, reseau->arcs[i].source, chemin, index + 1); // Continuer la recherche
        }
    }

    // Afficher le chemin lorsque l'on remonte dans la recursion
    if (!trouvePred) { // On est arrive a une source sans predecesseur
        printf("\nChemin trouve : ");
        for (int i = index; i >= 0; i--) {
            printf("%s", reseau->sommets[chemin[i] - 1].nom);
            if (i > 0) printf(" -> ");
        }
        printf("\n");
    }
}

// Afficher tous les predecesseurs
void toutpre(ReseauTrophique *reseau) {
    int sommetRecherche;
    int chemin[100]; // Tableau pour sauvegarder le chemin actuel
    int aDesPred = 0;

    // Afficher la liste des sommets disponibles
    printf("\nSommets disponibles dans le reseau trophique :\n");
    for (int i = 0; i < reseau->nbSommets; i++) {
        printf("[%d] %s (%s)\n", reseau->sommets[i].id, reseau->sommets[i].nom, reseau->sommets[i].type);
    }

    // Demander un sommet cible
    printf("\nEntrez l'ID du sommet que vous souhaitez explorer : ");
    scanf("%d", &sommetRecherche);
    // Verifier si le sommet a des predecesseurs
    for (int i = 0; i < reseau->nbArcs; i++) {

        if (reseau->arcs[i].cible == sommetRecherche) {
            aDesPred = 1;
            break;
        }
    }

    if (aDesPred==0) {
        printf("\nLe sommet %s n'a aucun predecesseur dans le reseau trophique.\n");
    }
    if (aDesPred==1) {
        // Lancer la recherche inversee avec DFS, on utilise le dfs inverse car on veut remonter depuis cette espece, cible a source
        printf("\nExploration des chemins menant jusqu'a %s\n", reseau->sommets[sommetRecherche - 1].nom);
        dfsInverse(reseau, sommetRecherche, chemin, 0);
    }
}

void afficherReseau(ReseauTrophique *reseau) {
    printf("\n--- Reseau Trophique ---\n");
    printf("\nSommets :\n");
    for (int i = 0; i < reseau->nbSommets; i++) {
        printf("[%d] %s (%s) - Biomasse : %d\n", reseau->sommets[i].id, reseau->sommets[i].nom, reseau->sommets[i].type, reseau->sommets[i].biomasse);
    }
    printf("\nArcs :\n");
    for (int i = 0; i < reseau->nbArcs; i++) {
        printf("[%d -> %d] Poids : %.2f\n", reseau->arcs[i].source, reseau->arcs[i].cible, reseau->arcs[i].poids);
    }
    sucpre(reseau);
}

// Rechercher des maillons
void rechercherMaillons(ReseauTrophique *reseau, const char *type) {
    printf("\nRecherche des %s :\n", type);
    for (int i = 0; i < reseau->nbSommets; i++) {
        if (strcmp(reseau->sommets[i].type, type) == 0) {
            printf("[%d] %s - Biomasse : %d\n",
                   reseau->sommets[i].id,
                   reseau->sommets[i].nom,
                   reseau->sommets[i].biomasse);
        }
    }
}
// On compte
void distributionDesDegres(ReseauTrophique *reseau) {
    int *degresEntrants = calloc(reseau->nbSommets, sizeof(int));
    int *degresSortants = calloc(reseau->nbSommets, sizeof(int));

    for (int i = 0; i < reseau->nbArcs; i++) {
        if (reseau->arcs[i].source >= 1 && reseau->arcs[i].source <= reseau->nbSommets) {
            degresSortants[reseau->arcs[i].source - 1]++;
        }
        if (reseau->arcs[i].cible >= 1 && reseau->arcs[i].cible <= reseau->nbSommets) {
            degresEntrants[reseau->arcs[i].cible - 1]++;
        }
    }
    printf("\nDistribution des degres :\n");
    for (int i = 0; i < reseau->nbSommets; i++) {
        printf("[%d] %s - Degre entrant : %d, Degre sortant : %d\n",
               reseau->sommets[i].id,
               reseau->sommets[i].nom,
               degresEntrants[i],
               degresSortants[i]);
    }

    free(degresEntrants);
    free(degresSortants);
}

// Pour calculer la hauteur cest juste un dfs ou on se souvient de la hauteur la plus haute rencontree
int calculerHauteurTrophiqueRec(ReseauTrophique *reseau, int sommetId, int *visites) {
    visites[sommetId] = 1;
    int maxHauteur = 0;

    for (int i = 0; i < reseau->nbArcs; i++) {
        if (reseau->arcs[i].source == sommetId + 1 && !visites[reseau->arcs[i].cible - 1]) {
            int hauteur = calculerHauteurTrophiqueRec(reseau, reseau->arcs[i].cible - 1, visites);
            if (hauteur > maxHauteur) {
                maxHauteur = hauteur;
            }
        }
    }

    visites[sommetId] = 0;
    return maxHauteur + 1;
}

int calculerHauteurTrophique(ReseauTrophique *reseau) {
    int *visites = calloc(reseau->nbSommets, sizeof(int));
    int maxHauteur = 0;

    for (int i = 0; i < reseau->nbSommets; i++) {
        int hauteur = calculerHauteurTrophiqueRec(reseau, i, visites);
        if (hauteur > maxHauteur) {
            maxHauteur = hauteur;
        }
    }

    free(visites);
    return maxHauteur;
}
// NbArcs/Nb maximum darcs , or le max arcs cest que chaque sommet est connect a tous sommet (sauf lui) donc nbsommet -1
double calculerDensite(ReseauTrophique *reseau) {
    int nbMaxArcs = reseau->nbSommets * (reseau->nbSommets - 1);
    return (double)reseau->nbArcs / nbMaxArcs;
}
// Faire tout les programmes precedent
void afficherComplexite(ReseauTrophique *reseau) {
    printf("\n--- Complexite du reseau ---\n");
    printf("Nombre d'especes : %d\n", reseau->nbSommets);

    int hauteurTrophique = calculerHauteurTrophique(reseau);
    printf("Hauteur trophique (nombre de niveaux) : %d\n", hauteurTrophique);

    double densite = calculerDensite(reseau);
    printf("Densite de liaison : %.2f\n", densite);

    distributionDesDegres(reseau);
}

// Liberer la memoire allouee
void libererReseau(ReseauTrophique *reseau) {
    free(reseau->sommets);
    free(reseau->arcs);
    free(reseau);
}
