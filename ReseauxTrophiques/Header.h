#ifndef ReseauxTrophiques_Header
#define ReseauxTrophiques_Header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure pour un sommet
typedef struct {
    int id;             // ID unique du sommet
    char nom[50];       // Nom de l'espece ou ressource
    char type[20];      // Type : producteur, consommateur, decomposeur
    int biomasse;       // Taille de la biomasse en kg
} Sommet;

// Structure pour un arc
typedef struct {
    int source;         // ID du sommet source
    int cible;          // ID du sommet cible
    float poids;        // Poids de la relation (proportion ou influence)
} Arc;

// Structure du reseau trophique
typedef struct {
    Sommet *sommets;    // Liste des sommets
    Arc *arcs;          // Liste des arcs
    int nbSommets;      // Nombre total de sommets
    int nbArcs;         // Nombre total d'arcs
} ReseauTrophique;

// Fonctions
ReseauTrophique* chargerReseau(const char *nomFichier);
void afficherReseau(ReseauTrophique *reseau);
void sucpre(ReseauTrophique *reseau);
void rechercherMaillons(ReseauTrophique *reseau, const char *type);
void unpre(ReseauTrophique *reseau);
void dfsinverse(ReseauTrophique *reseau, int sommetActuel, int cible, int chemin[], int index);
void toutpre(ReseauTrophique *reseau);
void toutpre(ReseauTrophique *reseau);
void libererReseau(ReseauTrophique *reseau);
void afficherComplexite(ReseauTrophique *reseau);
int calculerHauteurTrophique(ReseauTrophique *reseau);
double calculerDensite(ReseauTrophique *reseau);
void distributionDesDegres(ReseauTrophique *reseau);


#endif
