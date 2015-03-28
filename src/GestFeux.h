/*************************************************************************
                           		 GestFeux
                             -------------------
    début                : 17/04/2015
    copyright            : (C) Andrea Accardo
*************************************************************************/
    
//-------- Réalisation de la tâche <GestFeux> (fichier GestFeux.h) -------
#if ! defined ( GestSignalisation_H )
#define GestSignalisation_H

//------------------------------------------------------------------------
// Rôle de la tâche <GestSignalisation>
// Gère les changement de la durée et des couleurs des feux 
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes
const unsigned int DUREE_AXE_NS_DEFAULT = 20;
const unsigned int DUREE_AXE_EO_DEFAULT = 10;
const char VERT[] = "Vert  \0";
const char ORANGE[] = "Orange\0";
const char ROUGE[] = "Rouge \0";
const unsigned int DUREE_ORANGE = 3;
const unsigned int DUREE_ROUGE = 2;
const unsigned int DUREE_ARRET = DUREE_ORANGE + DUREE_ROUGE;

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void GestFeux (int idSemEtat, int idMemEtat, int idSemDuree, int idMemDuree);
// Mode d'emploi :
//	idSemEtat	: ID du sémaphore d'exclusion mutuelle liée à la zone de 
//				  mémoire état
//	idMemEtat	: ID de la zone mémoire partagée contenant l'état des feux
//	idSemDuree	: ID du sémaphore d'exclusion mutuelle liée à la zone de 
//				  mémoire durée
//	idMemDuree	: ID de la zone mémoire partagée contenant la durée des 
//				  feux
//	- Initialise les feux
//	- Gère la démarche des feux
// Contrat :
// - les zones mémoires doivent être créée avant l'exécution de la tâche GestFeux

#endif // GestSignalisation_H

