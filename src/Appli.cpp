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
#include "GestVoie.h"
#include "GestFeux.h"


#include "Outils.h"
#include "Heure.h"
#include "Generateur.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------ Fonctions privées

int creerBAL (int droits)
// Mode d'emploi :
// Cree une boite aux lettres
// <droits> : droits de la tache sur la BAT
// renvoie l'id de la BAT
{
	return = msgget(IPC_PRIVATE, droits | IPC_CREAT);
} //----- fin de creerBAL

void detruireBAL(int idBAT)
// Mode d'emploi :
// Detruit une boite aux lettres
// <idBAT> : id de la BAT à détruire
{
    msgctl(idBAT, IPC_RMID, 0);
} //----- fin de detruireBAL

int creerMemoire(size_t taille, int droits)
// Mode d'emploi :
// Cree une zone memoire partagee
// <taille> : taille de la memoire a allouer
// <droits> : droits de la tache sur la BAT
// renvoie l'id de la memoire creee
{
	int idMemoire = shmget(IPC_PRIVATE, taille, droits | IPC_CREAT);
	return idMemoire;
} //----- fin de creerMemoire

void * attacherMemoire (int idMemoire)
// Mode d'emploi :
// Permet a la tache d'acceder a la memoire
// <idMemoire> : id de la memoire a laquelle acceder
// renvoie la zone memoire
{
	return shmat (idMemoire, NULL, 0);
} //----- fin de attacherMemoire

void detacherMemoire (const void * zone)
// Mode d'emploi :
// Permet a la rendre l'acces a une zone memoire
// <zone> : zone memoire a rendre
{
	shmdt(zone);
} //----- fin de detacherMemoire

void detruireMemoire (int idMemoire)
// Mode d'emploi :
// Detruit une zone memoire
// <idMemoire> : id de la zone memoire a detruire
{
	shmctl (idMemoire, IPC_RMID, 0);
} //----- fin de detruireMemoire


int creerSemaphore ( int valInit, int valMax, int droits)
// Mode d'emploi :
// Cree un Semaphore
// <valInit> : valeur initiale du nombre de jeton
// <valMax> : valeur max de jetons
// <droits> : droits de la tache sur le semaphore
// renvoie l'id du semaphore cree
{
  int idSem = semget ( IPC_PRIVATE, valMax, IPC_CREAT | droits);
  semctl ( idSem, 0, SETVAL, valInit );
  return idSem;
} //----- fin de creerSemaphore

void detruireSemaphore ( int idSem )
// Mode d'emploi :
// Detruit un Semaphore
// <idMemoire> : id du Semaphore a detruire
{
  semctl ( idSem, 0, IPC_RMID, 0 );
} //----- fin de detruireSemaphore

int main() {
// Mode d'emploi :
//	fonction mere, gere l'application
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
	pid_t pidVoie[4];
	pid_t pidFeux;
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
	boiteLettresVoitures = creerBAL(DROITS);

	//	-- mise en place memoire partagee
	idMemEtatFeux = creerMemoire ( sizeof(EtatFeux), DROITS);
	idMemDureeFeux = creerMemoire ( sizeof(Duree), DROITS);

	//	initialiser memoires
	duree_feux = (struct Duree *) attacherMemoire ( idMemDureeFeux );
	etat_feux = (struct EtatFeux *) attacherMemoire ( idMemEtatFeux );

	duree_feux->dureeEO = 10;
	duree_feux->dureeNS = 20;

	etat_feux->feuxEO = false;
	etat_feux->feuxNS = false;

	detacherMemoire (duree_feux);
	detacherMemoire (etat_feux);
	//	fin initialisation memoires

	// -- mise en place des semaphores
	idSemDureeFeux = creerSemaphore (1, 1, DROITS);	
	idSemEtatFeux = creerSemaphore (1, 1, DROITS);


	//  -- initialiser application
	InitialiserApplication ( XTERM );
  	
	//	== Creation des taches ==

	//  -- creer tache Heure
  	pidHeure = CreerEtActiverHeure ();

  	//	-- creer tache generateur
  	pidGenerateur = CreerEtActiverGenerateur(0, boiteLettresVoitures);

  	//	-- creer tache gestion de Feux
  	if( ( pidFeux = fork ( ) ) == 0 )
    {
      GestFeux (idSemEtatFeux, idMemEtatFeux, idSemDureeFeux, idMemDureeFeux);
    }

  	//	-- creer taches gestion de voies
    if( ( pidVoie[0] = fork ( ) ) == 0 )
    {
      GestVoie (SUD, idMemDureeFeux, boiteLettresVoitures);
    }
    if( ( pidVoie[1] = fork ( ) ) == 0 )
    {
      GestVoie (OUEST, idMemDureeFeux, boiteLettresVoitures);
    }
    if( ( pidVoie[2] = fork ( ) ) == 0 )
    {
      GestVoie (NORD, idMemDureeFeux, boiteLettresVoitures);
    }
    if( ( pidVoie[3] = fork ( ) ) == 0 )
    {
      GestVoie (EST, idMemDureeFeux, boiteLettresVoitures);
    }

  	//	-- creer GestClavier
  	if( ( pidClavier = fork ( ) ) == 0 )
    {
      GestClavier (pidGenerateur, boiteLettresVoitures, idSemDureeFeux, idMemDureeFeux);
    }



  	//  =====  Destruction  =====  //
    waitpid(pidClavier, NULL, 0);

  	//	-- detruire taches Voies
	int i;
	for(i = 0; i < 4; i++)
	{
		kill(pidVoie[i], SIGUSR2);
		waitpid(pidVoie[i], NULL, 0);
	}

  	//	-- detruire tache Feux
	kill(pidFeux, SIGUSR2);
	waitpid(pidFeux, NULL, 0);
	
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

	return 0;

} //----- fin de main
