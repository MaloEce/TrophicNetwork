#ifndef ReseauxTrophiques_Header
#define ReseauxTrophiques_Header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include <math.h>

//ici nous definissons nos structures, nous avons les structures sommet comprenant les informations sur une espece
//une struct Arcs, avec la cible et la source (oriente de la source a la cible) et sa ponderation, notre coefficient dinteraction
typedef struct {
    int id;
    char nom[50];
    char type[20];
    int biomasse;
} Sommet;


typedef struct {
    int source;
    int cible;
    float poids;
} Arc;

//Ici nous avpons notre structure reseau, qui est defini par les structures vu precedement mais aussi le nombre total darcs et de sommet, pour ne pas avoir a passer ces variable partout, jai decider de la mettre dans le resau directement

typedef struct {
    Sommet *sommets;
    Arc *arcs;
    int nbSommets;
    int nbArcs;
} ReseauTrophique;

// Fonctions sauf pour la simulation
ReseauTrophique* chargerReseau(const char *nomFichier);
void dfsComposante(ReseauTrophique *reseau, int sommet,  int composanteId, int *composantes);
void connexite(ReseauTrophique **reseau);
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
void simuler_dynamique(ReseauTrophique *reseau, int iterations, float r);


#endif
