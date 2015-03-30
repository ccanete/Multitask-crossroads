/*************************************************************************
                           		 GestFeux
                             -------------------
    début                : 17/04/2015
    copyright            : (C) Andrea Accardo
*************************************************************************/
    
//------ Réalisation de la tâche <GestFeux> (fichier GestFeux.cpp) -------

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>


#include <Outils.h>
//------------------------------------------------------ Include personnel
#include "GestFeux.h"
#include "Appli.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static int semEtat;
static int semDuree;

static EtatFeux * etatFeux;
static Duree * dureeFeux;

static unsigned int dureeNS;
static unsigned int dureeEO;
static struct sembuf reserver = {0, -1, 0};
static struct sembuf liberer = {0, 1, 0};

//------------------------------------------------------ Fonctions privées
static void destruction(int noSignal) {
// Mode d'emploi :
//	- Termine la tâche GestFeux
// Algorithme : 
//	- Détachement des zones de mémoire partagée
//	- Sortie de la tâche

		//						\\
			//				\\
				//		\\

			  //DESTRUCTION\\
						
				//		\\
			//				\\
		//						\\

	// Détachement de etatFeux et semDuree

	shmdt(etatFeux);
	shmdt(dureeFeux);

	// Sortie

	exit(0);

}

static void updateMemDuree() {
// Mode d'emploi : 
//	- Met à jour les durée des feux en prenant 
//	  les nouvelles valeurs de la mémoire partagée
// Algorithme : 
//	- Lecture de la mémoire partagée memDuree à travers
//	  l'utilisation du sémaphore
// 	- Mise à jour de l'affichage de l'interface

	semop(semDuree, &reserver, 1);
	dureeNS = dureeFeux->dureeNS;		// Mémoire partagée --> résource critique
	dureeEO = dureeFeux->dureeEO;		// Mémoire partagée --> résource critique
	semop(semDuree, &liberer, 1);

	// Affichage durées des feux
  	Effacer(DUREE_AXE_NS);
	Afficher(DUREE_AXE_NS, dureeNS, GRAS, NORMALE);
  	Effacer(DUREE_AXE_EO);
  	Afficher(DUREE_AXE_EO, dureeEO, GRAS, NORMALE);

}

static void updateMemEtat(bool etatNS, bool etatEO) {
// Mode d'emploi : 
//	- Met à jour les états des feux dans la mémoire partagée
// Algorithme : 
//	- Ecriture de la mémoire partagée memEtat à travers
//	  l'utilisation du sémaphore

	semop(semEtat, &reserver, 1);
	etatFeux->feuxNS = etatNS;		// Mémoire partagée --> résource critique
	etatFeux->feuxEO = etatEO;		// Mémoire partagée --> résource critique
	semop(semEtat, &liberer, 1);

}

static void init() {
// Mode d'emploi : 
//	- Initialise l'interface graphique
// Algorithme : 
//	- Mise à jour des durées des feux
//	- Mise à jour de l'affichage dans l'interface concernant la durée et la couleur

  	// Mise à jour des durées des feux

  	updateMemDuree();

  	// Mise à jour des états des feux

  	updateMemEtat(true, false);

  	// Affichage couleurs des feux

  	Effacer(COULEUR_AXE_NS);
  	Afficher(COULEUR_AXE_NS, VERT, GRAS, NORMALE);
  	Effacer(COULEUR_AXE_EO);
  	Afficher(COULEUR_AXE_EO, ROUGE, GRAS, INVERSE);

  	// Affichage des temps initiaux des feux

	Afficher(TEMPS_AXE_NS, dureeNS, GRAS, NORMALE);
	Afficher(TEMPS_AXE_EO, dureeNS + DUREE_ARRET, GRAS, NORMALE);

}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void GestFeux (int idSemEtat, int idMemEtat, int idSemDuree, int idMemDuree) {
// Mode d'emploi :
//
// Contrat :
//
// Algorithme :
//	- Création de l'handler et armement du signal SIGUSR2
//	- Attachement à la mémoire partagée étatFeux
//	- Sauvegarde de l'adresse du sémaphore chargé de étatFeux (semEtat)
//	- Attachement à la mémoire partagée duréeFeux
//	- Sauvegarde de l'adresse du sémaphore chargé de duréeFeux (semDuree)
//	- Initialisation de l'interface graphique
//	- Gestion des feux et de ses temps d'attentes

	//						\\
		//				\\
			//		\\

		//INITIALISATION\\

			//		\\
		//				\\
	//						\\

	// Mise en place du handler de destruction
	
	struct sigaction action;
	action.sa_handler = destruction;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGUSR2, &action, NULL);

	// Attachement aux mémoires partagée et sauvegarde
	// des ID des respectifs sémaphores
	
	etatFeux = (EtatFeux *) shmat(idMemEtat, NULL, 0);
	semEtat = idSemEtat;
	dureeFeux = (Duree *) shmat(idMemDuree, NULL, 0);
	semDuree = idSemDuree;
	
	// Initialisation de l'interface graphique

	init();

	//						\\
		//				\\
			//		\\

			//MOTEUR\\
		
			//		\\
		//				\\
	//						\\

	for( ; ; ) {

		// Mise à jour des durées des feux

		updateMemDuree();

		// Attente feu vert sur l'axe N-S et rouge sur l'axe E-O

		for(int i = dureeNS; i > 0; i--) {

			Effacer(TEMPS_AXE_NS);
			Afficher(TEMPS_AXE_NS, i, GRAS, NORMALE);
			Effacer(TEMPS_AXE_EO);
			Afficher(TEMPS_AXE_EO, i + DUREE_ARRET, GRAS, NORMALE);
			sleep(1);

		}

		// Le feu sur l'axe N-S passe à orange :
		//	- Affichage
		Effacer(COULEUR_AXE_NS);
		Afficher(COULEUR_AXE_NS, ORANGE, GRAS, INVERSE);
		//	- Mise à jour de la mémoire partagée
		updateMemEtat(false, false);

		// Attente feu orange sur l'axe N-S et rouge sur l'axe E-O

		for(int i = DUREE_ORANGE; i > 0; i--) {

			Effacer(TEMPS_AXE_NS);
			Afficher(TEMPS_AXE_NS, i, GRAS, NORMALE);
			Effacer(TEMPS_AXE_EO);
			Afficher(TEMPS_AXE_EO, i + DUREE_ROUGE, GRAS, NORMALE);
			sleep(1);

		}

		// Le feu sur l'axe N-S passe à rouge :
		//	- Affichage
		Effacer(COULEUR_AXE_NS);
		Afficher(COULEUR_AXE_NS, ROUGE, GRAS, INVERSE);

		// Attente feu rouge sur les deux axes

		for(int i = DUREE_ROUGE; i > 0; i--) {

			Effacer(TEMPS_AXE_NS);
			Afficher(TEMPS_AXE_NS, i, GRAS, NORMALE);
			Effacer(TEMPS_AXE_EO);
			Afficher(TEMPS_AXE_EO, i, GRAS, NORMALE);
			sleep(1);

		}

		// Le feu sur l'axe E-O passe à vert :
		//	- Affichage
		Effacer(COULEUR_AXE_EO);
		Afficher(COULEUR_AXE_EO, VERT, GRAS, NORMALE);
		//	- Mise à jour de la mémoire partagée
		updateMemEtat(false, true);
		//	- Mise à jour des durées des feux
		updateMemDuree();

		// Attente feu vert sur l'axe E-O et rouge sur l'axe N-S

		for(int i = dureeEO; i > 0; i--) {

			Effacer(TEMPS_AXE_EO);
			Afficher(TEMPS_AXE_EO, i, GRAS, NORMALE);
			Effacer(TEMPS_AXE_NS);
			Afficher(TEMPS_AXE_NS, i + DUREE_ARRET, GRAS, NORMALE);
			sleep(1);

		}

		// Le feu sur l'axe E-O passe à orange :
		//	- Affichage
		Effacer(COULEUR_AXE_EO);
		Afficher(COULEUR_AXE_EO, ORANGE, GRAS, INVERSE);
		//	- Mise à jour de la mémoire partagée
		updateMemEtat(false, false);

		// Attente feu orange sur l'axe E-O et rouge sur l'axe N-S

		for(int i = DUREE_ORANGE; i > 0; i--) {

			Effacer(TEMPS_AXE_EO);
			Afficher(TEMPS_AXE_EO, i, GRAS, NORMALE);
			Effacer(TEMPS_AXE_NS);
			Afficher(TEMPS_AXE_NS, i + DUREE_ROUGE, GRAS, NORMALE);
			sleep(1);

		}

		// Le feu sur l'axe E-O passe à rouge :
		//	- Affichage
		Effacer(COULEUR_AXE_EO);
		Afficher(COULEUR_AXE_EO, ROUGE, GRAS, INVERSE);

		// Attente feu rouge sur les deux axes

		for(int i = DUREE_ROUGE; i > 0; i--) {

			Effacer(TEMPS_AXE_EO);
			Afficher(TEMPS_AXE_EO, i, GRAS, NORMALE);
			Effacer(TEMPS_AXE_NS);
			Afficher(TEMPS_AXE_NS, i, GRAS, NORMALE);
			sleep(1);

		}

		// Le feu sur l'axe N-S passe à vert :
		//	- Affichage
		Effacer(COULEUR_AXE_NS);
		Afficher(COULEUR_AXE_NS, VERT, GRAS, NORMALE);
		//	- Mise à jour de la mémoire partagée
		updateMemEtat(true, false);
		
	}

} //----- fin de nom

