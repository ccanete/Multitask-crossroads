/*************************************************************************
                           		 GestClavier
                             -------------------
    début                : 17/04/2015
    copyright            : (C) Andrea Accardo
*************************************************************************/

//---------- Réalisation de la tâche <GestClavier> (fichier GestClavier.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
//------------------------------------------------------ Include personnel
#include "GestClavier.h"

#include "Generateur.h"
#include "Voiture.h"
#include "Menu.h"
#include "Outils.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static pid_t pidGen;
static int vBAL;
static int semDuree;

static bool genActif;
static unsigned int numVoiture;

//static Duree * dureeFeux;

// struct utilisée par le sémaphore
static struct sembuf reserver = {0, -1, 0};
static struct sembuf liberer = {0, 1, 0};

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void GestClavier (pid_t generateur, int idBAL, int idSem, int idMemDuree) {
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
// Algorithme : 
//	- Sauvegarde de l'adresse de la boîte aux lettres (idBAL)
//	- Sauvegarde de l'adresse du sémaphore (idSem)
//	- Attachement à la mémoire partagée en utilisant son id (idMemDuree)
//	- Sauvegarde du pid du générateur
// 	- Initialisation numéro voiture
//	- Exécution de la procedure Menu (BOUCLE INFINIE)

	//						\\
		//				\\
			//		\\

		//INITIALISATION\\

			//		\\
		//				\\
	//						\\

	vBAL = idBAL;
	semDuree = idSem;
	//dureeFeux = (Duree *) shmat(idMemDuree, SHM_WRONLY, 0);
 	pidGen = generateur;
 	genActif = false;
 	numVoiture = ONE;

	//						\\
		//				\\
			//		\\

			//MOTEUR\\
		
			//		\\
		//				\\
	//						\\

 	Menu();

}	//----- fin de GestClavier

void Commande (char code) {
// Mode d'emploi :
//  code  : Paramètre en majuscule reçu par menu
//  - Permet le traitement des codes 'F' et 'G'
// Contrat :
//  -  Le paramètre en entrée doit être rendu majuscule
//	   par la procedure menu 
// Algorithme : Selon si code == 'G' ou 'Q' respectivement
//				elle met en pause/marche le générateur
//				ou sort de l'application 
//				(avec conséquent nettoyage du contexte)

	if (code == 'Q') {

		//						\\
			//				\\
				//		\\

			  //DESTRUCTION\\
				
				//		\\
			//				\\
		//						\\

		//	Affichage message de sortie
		
		Effacer(MESSAGE);
  		Afficher(MESSAGE, "Fermeture");

		//	- Détachement de la mémoire partagée
		
 		//shmdt(dureeFeux);

		// Nettoyage du contexte d'exécution et déstruction de la tâche

 		exit (0);

	} else if (code == 'G') {

		switch (genActif){

			case true:

				// Envoi du signal de stop
				
				kill (pidGen, SIGSTOP);

				// Mise à jour de l'affichage 
				
				Effacer(ETAT_GENERATEUR);
				Afficher(ETAT_GENERATEUR, "OFF");
				Effacer(MESSAGE);
				Afficher(MESSAGE, "Arret du generateur");
				
				// Changement du valeur de la variable genActif
				
				genActif = false;
				
				break;

			case false:

				// Envoi du signal de reprise
				
				kill (pidGen, SIGCONT);

				// Mise à jour de l'affichage 
				
				Effacer(ETAT_GENERATEUR);
				Afficher(ETAT_GENERATEUR, "ON");
				Effacer(MESSAGE);
				Afficher(MESSAGE, "Activation du generateur");
				
				// Changement du valeur de la variable genActif
				
				genActif = true;
				
				break;
		}

	}

}	//----- fin de GestClavier

void Commande (TypeVoie entree, TypeVoie sortie) {
// Mode d'emploi :
//  entree  : Voie par laquelle la voiture entre dans le carrefour
//  sortie  : Voie par laquelle la voiture sort du carrefour
//  -  Permet de créer une voiture manuellement
// Algorithme :
//	- Modification du numéro voiture (simple)
//	- Création d'un message contenant toutes les 
//	  données pour la prochaine voiture à entrer
//	  dans le carrefour
//	- Dépôt du message dans la boîte aux lettres
// 	- Affichage d'un message confirmant du correct ajoute de la voiture
//	- Mise à jour de l'interface

	if (numVoiture < MAX_VOITURE) {

		numVoiture++;

	} else {

		numVoiture = ONE ;

	}

	// Creation des structures :

	//  - Voiture :
	struct Voiture aVoiture = {entree, sortie, numVoiture};
	//  - MsgVoiture : 
	struct MsgVoiture msg = {static_cast<long>(entree), aVoiture};

	// Envoie du message (msg) à la boîte aux lettres

	msgsnd(vBAL, &msg, TAILLE_MSG_VOITURE, 0);

	// Affichage

	Effacer(MESSAGE);
	Afficher(MESSAGE, "Voiture ajoute correctement a la liste");

	// Ajout de la voiture dans l'interface grâce à la 
	// méthode fournie dans Outils.h

	OperationVoie(PLUS, entree);

}	//----- fin de GestClavier

void Commande (TypeVoie voie, unsigned int duree) {
// Mode d'emploi :
//  voie   : Voie dont on souhaite modifier le temps du feux
//  duree  : Durée d'attente dans l'êtat vert des feux choisis 
//  -  Modification du temps des feux dans l'état vert
// Algorithme : 
//	- Contrôle de l'axe à modifier
//	- Modification de la mémoire partagée à travers l'utilisation du sémaphore
//	- Affichage d'un message confirmant la correcte modification du sémaphore
//	Modification des données contenues dans 
//				la mémoire partagée concernant la durée 
//				d'attente dans l'état vert des feux
//				de la voie choisie

	if (voie == NORD || voie == SUD) {

		semop(semDuree, &reserver, 1);
		//dureeFeux->nS = duree;		// Mémoire partagée --> résource critique
		semop(semDuree, &liberer, 1);

		// Affichage

		Effacer(MESSAGE);
		Afficher(MESSAGE, "Duree du feux de l'axe N-S modifie (attendre le prochain changement)");

	} else if (voie == OUEST || voie == EST) {

		semop(semDuree, &reserver, 1);
		//dureeFeux->eO = duree;		// Mémoire partagée --> résource critique
		semop(semDuree, &liberer, 1);

		// Affichage

		Effacer(MESSAGE);
		Afficher(MESSAGE, "Duree du feux de l'axe E-O modifie (attendre le prochain changement)");
	
	}

}	//----- fin de GestClavier


