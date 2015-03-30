/*************************************************************************
                           GestVoie  -  description
                             -------------------
    copyright            : (C) GestVoie par Cyril Canete
    e-mail               : cyril.canete@insa-lyon.fr
*************************************************************************/

//---------- Interface de la tâche <GestVoie> (fichier GestVoie.h) -------
#if ! defined ( GestVoie_H )
#define GestVoie_H

//------------------------------------------------------------------------
// Rôle de la tâche <GestVoie>
//
// Gère l'affichage et le déplacement des voitures sur les voies
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
#include "Outils.h"
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void GestVoie( TypeVoie voie, int idEtatFeux, int idSemEtat, int idFileVoiture );
// Mode d'emploi :
//	<numVoie>	: Choix de la voie creee
//	<etatFeux>		: id zone mémoire etatFeux
//	<fileVoiture>	: id boite au lettre fileVoiture
// - Réception des messages et traitement des voitures
// Contrat :
// Aucun

#endif // GestVoie_H

