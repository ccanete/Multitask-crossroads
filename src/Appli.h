/*************************************************************************
                           Appli  -  Gestion de l'application 
                             -------------------
    début                : Appli
    copyright            : (C) Cyril Canete
    e-mail               : cyril.canete@insa-lyon.fr
*************************************************************************/

//---------- Interface de la tâche <Appli> (fichier Appli.h) -------
#if ! defined ( Appli_H )
#define Appli_H

//------------------------------------------------------------------------
// Rôle de la tâche <Appli>
//
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

// Etats des Feux
// état :
//	- faux : feu rouge/orange
//	- vrai : feu vert
//
// eO : axe Est-Ouest
// nS : axe Nord-Sud
struct EtatFeux
{
    bool eO;
    bool nS;
};


// Durée des Feux
// eO : axe Est-Ouest
// nS : axe Nord-Sud
struct Duree
{
    int eO;
    int nS;
};

int main();

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
// type Nom ( liste de paramètres );
// Mode d'emploi :
//
// Contrat :
//

#endif // Appli_H

