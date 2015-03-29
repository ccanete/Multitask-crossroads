/*************************************************************************
                           		 GestClavier
                             -------------------
    début                : 17/04/2015
    copyright            : (C) Andrea Accardo
*************************************************************************/

//----- Interface de la tâche <GestClavier> (fichier GestClavier.h) ------
#if ! defined ( GestClavier_H )
#define GestClavier_H

//------------------------------------------------------------------------
// Rôle de la tâche <GestClavier>
// Gestion des commandes reçues par le menu.
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
#include "Outils.h"

//--------------------------------------------------- Interfaces utilisées


//------------------------------------------------------------- Constantes
const unsigned int MAX_VOITURE = 199 ;
const unsigned int ONE = 1;

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void GestClavier ( pid_t generateur, int idBAL, int idSem, int idMemDuree );
// Mode d'emploi :
//  generateur	: le pid du générateur
//  idBAL   	: L'ID de la boîte aux lettres contenant les 
//				  voitures en attente
//  idSem   	: L'ID du sémaphore protégent la mémoire partagée
//                de la durée des feux
//  idMemDuree 	: L'ID de la memoire partagée contenant la duree
//                des feux
//  - Lancement de la tâche GestClavier par la tâche Appli (mêre)
// Contrat :
//  - Toutes les ressources doivent être déjà crées avant le 
//	  lancement de la tâche GestClavier

void Commande (char code);
// Mode d'emploi :
//  code  : Paramètre en majuscule reçu par menu
//  - Permet le traitement des codes 'F' et 'G'
// Contrat :
//  -  Le paramètre en entrée doit être rendu majuscule
//	   par la procedure menu 

void Commande (TypeVoie entree, TypeVoie sortie);
// Mode d'emploi :
//  entree  : Voie par laquelle la voiture entre dans le carrefour
//  sortie  : Voie par laquelle la voiture sort du carrefour
//  -  Permet de créer une voiture manuellement


void Commande (TypeVoie voie, unsigned int duree);
// Mode d'emploi :
//  voie   : Voie dont on souhaite modifier le temps du feux
//  duree  : Durée d'attente dans l'êtat vert des feux choisis 
//  -  Modification du temps des feux dans l'état vert


#endif // GestClavier_H

