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

#define DROITS	0666
//------------------------------------------------------------------------
// Rôle de la tâche <Appli>
//
//	Tache mere du Carrefour, initialise l'application, attends la fin de l'application et ferme les taches en cours.
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE

//------------------------------------------------------------------ Types

//	Durée des Feux
//	dureeNS : axe Nord/Sud
//	dureeEO : axe Est/Ouest
struct Duree
{
    int dureeNS;
    int dureeEO;
};

//	Etats des feux NS et EO
//	vrai : vert, faux : orange ou rouge
//	feuxNS : axe Nord/Sud
//	feuxEO : axe Est/Ouest

struct EtatFeux
{
    bool feuxNS;	
    bool feuxEO;
};


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

int main();


#endif // Appli_H

