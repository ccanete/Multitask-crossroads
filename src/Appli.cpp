/*************************************************************************
                           Appli  -  Gestion de l'application 
                             -------------------
    début                : Appli
    copyright            : (C) Cyril Canete
    e-mail               : cyril.canete@insa-lyon.fr
*************************************************************************/

//---------- Réalisation de la tâche <Appli> (fichier Appli.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <unistd.h>		//  sleep
#include <sys/wait.h>	//  waitpid

//------------------------------------------------------ Include personnel
#include "Appli.h"

#include "Outils.h"
#include "Heure.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées
int main() {
// Mode d'emploi :
//	main function
//	
// Contrat :
//	x32 OS
// 
//	Algorithme :
//	- Masquer les signaux SIGUSR2 et SIGCHILD
//	- Mettre en place boite aux lettre, ajout_voiture : communication Generateur, Clavier et Voie
//  - Mettre en place memoires partagees, etat_feux, duree_feux
//	- Mettre en place les semaphores, 
//	- Création de l'application
//	- Création de la tache Heure
//	- Création de la tache Generateur
//	- Création de la tache Feux
//	- Création des taches Voie (4)
//	- Création de la tache Clavier
//  - Attente fin de tache Clavier
//	- Detruire dans l'ordre inverse 

	//  =====  Initialisation  ===== //

	pid_t pidHeure;
	pid_t pidGenerateur;
	pid_t pidVoie[4];
	pid_t pidFeux;
	pid_t pidClavier;

	//  initialiser application
	InitialiserApplication ( XTERM );
  	
	//  initialiser tache Heure
  	pidHeure = CreerEtActiverHeure ();
	
  	//  =====  Moteur (test)  ----- //
  	sleep(10);

  	//  =====  Destruction  -----  //

  	//  detruire tache heure
	kill(pidHeure, SIGUSR2);
	waitpid(pidHeure, NULL, 0); // wait for end of the process TacheHeure

	//  detruire application
	TerminerApplication();	
}
