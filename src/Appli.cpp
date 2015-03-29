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
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include <sys/shm.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/sem.h>

//------------------------------------------------------ Include personnel
#include "Appli.h"
#include "GestClavier.h"

#include "Outils.h"
#include "Heure.h"
#include "Generateur.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées

int ouvrirBAT( key_t keyval )
{
        int     qid;
        
        if((qid = msgget( keyval, IPC_CREAT | 0660 )) == -1)
        {
                return(-1);
        }
        
        return(qid);
}

static void detruireBAL(int idBAT)
// Mode d'emploi :
// Détruit les boites aux lettres
// <fileVoitures> : id de la BAL contenant les voitures
//
// Algorithme :
// Trivial
{
    msgctl(idBAT, IPC_RMID, 0);
} //----- fin de detruireBAL

int creerMemoire(size_t taille, int droits)
// Mode d'emploi :
// Créé une zone de memoire partagee de taille definie
// <idMemoire> : id de la zone mémoire partagée
//
{
    // creer zone memoire
	int idMemoire = shmget(IPC_PRIVATE, taille, droits | IPC_CREAT);

	return idMemoire;
} //----- fin de creerMemoire

void * attacherMemoire (int idMemoire)
{
	return shmat (idMemoire, NULL, 0);
}

void detacherMemoire (const void * zone)
{
	shmdt(zone);
}

void detruireMemoire (int idMemoire)
{
	shmctl (idMemoire, IPC_RMID, 0);
}


int CreerSemaphore ( int valInit, int valMax, int droits)
// Mode d'emploi :
//  - Cree un semaphore au nom défini et renvoie un id de semaphore
//  - On devra préciser sa valeur initiale et sa valeur maximale
//  - On précisera également les options de lecture/écriture
{
  int idSem = semget ( IPC_PRIVATE, valMax, IPC_CREAT | droits);
  semctl ( idSem, 0, SETVAL, valInit );
  return idSem;
} // Fin de CreerSemaphore

void DetruireSemaphore ( int idSem )
// Mode d'emploi :
// Détruis un semaphore avec son id
{
  semctl ( idSem, 0, IPC_RMID, 0 );
} // Fin de DetruireSemaphore

int main() {
// Mode d'emploi :
//	main function
//	
// Contrat :
//	x32 OS
// 
//	Algorithme :
//	- Masquage les signaux SIGCHILD et SIGUSR2
//	- Mise en place boite aux lettre, ajout_voiture : communication Generateur, Clavier et Voie
//  - Mise en place memoires partagees, etat_feux, duree_feux
//	- Mise en place les semaphores, 
//	- Initialisation de l'application
//	- Création de la tache Heure
//	- Création de la tache Generateur
//	- Création de la tache Feux
//	- Création des taches Voie (4)
//	- Création de la tache Clavier
//  - Attente fin de tache Clavier
//	- Detruire dans l'ordre inverse 

	//  =====  Initialisation  ===== //

	int boiteLettresVoitures;
	int idMemDureeFeux;
	int idMemEtatFeux;
	int idSemDureeFeux;
	int idSemEtatFeux;

	EtatFeux* etat_feux;
	Duree* duree_feux;

	pid_t pidHeure;
	pid_t pidGenerateur;
	// pid_t pidVoie[4];
	// pid_t pidFeux;
	pid_t pidClavier;


	//	-- masquer les signaux SIGCHILD et SIGUSR2
	struct sigaction masqueSIGCHLD;
	struct sigaction masqueSIGUSR2;

	masqueSIGCHLD.sa_handler = SIG_IGN;
	sigemptyset(&masqueSIGCHLD.sa_mask);
	masqueSIGCHLD.sa_flags = 0;
	sigaction(SIGCHLD,&masqueSIGCHLD, NULL);

	masqueSIGUSR2.sa_handler = SIG_IGN;
	sigemptyset(&masqueSIGUSR2.sa_mask);
	masqueSIGUSR2.sa_flags = 0;
	sigaction(SIGUSR2,&masqueSIGUSR2, NULL);
	//	fin masquage

	//	-- mise en place boite aux lettres
	boiteLettresVoitures = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

	//	-- mise en place memoire partagee
	idMemEtatFeux = creerMemoire ( sizeof(EtatFeux), 0666);
	idMemDureeFeux = creerMemoire ( sizeof(Duree), 0666);

	//	initialiser memoires
	duree_feux = (struct Duree *) attacherMemoire ( idMemDureeFeux );
	etat_feux = (struct EtatFeux *) attacherMemoire ( idMemEtatFeux );

	duree_feux->eO = 12;
	duree_feux->nS = 18;

	etat_feux->eO = false;
	etat_feux->nS = false;

	detacherMemoire (duree_feux);
	detacherMemoire (etat_feux);
	//	fin initialisation memoires

	// -- mise en place des semaphores
	idSemDureeFeux = CreerSemaphore (1, 1, 0666);	
	idSemEtatFeux = CreerSemaphore (1, 1, 0666);


	//  -- initialiser application
	InitialiserApplication ( XTERM );
  	
	//  -- creer tache Heure
  	pidHeure = CreerEtActiverHeure ();

  	//	-- creer tache generateur
  	pidGenerateur = CreerEtActiverGenerateur(0, boiteLettresVoitures);

  	//	-- crer GestClavier
  	if( ( pidClavier = fork ( ) ) == 0 )
    {
      GestClavier (pidGenerateur, boiteLettresVoitures, idSemDureeFeux, idMemDureeFeux);
    }


  	//  =====  Destruction  =====  //
    waitpid(pidClavier, NULL, 0);

  	//	-- detruire tache Generateur
	kill(pidGenerateur, SIGCONT);
  	kill(pidGenerateur, SIGUSR2);
  	waitpid(pidGenerateur, NULL, 0);

  	//	-- detruire tache heure
	kill(pidHeure, SIGUSR2);
	waitpid(pidHeure, NULL, 0);

	//	-- detruire semaphores
	DetruireSemaphore(idSemDureeFeux);
	DetruireSemaphore(idSemEtatFeux);

	//	-- detruire memoires partagees
	detruireMemoire (idMemDureeFeux);
	detruireMemoire (idMemEtatFeux);

	//	-- detruire boite aux lettres
	detruireBAL(boiteLettresVoitures);

	//  detruire application
	TerminerApplication();	
}
