/*************************************************************************
                           GestVoie  -  description
                             -------------------
    copyright            : (C) GestVoie par Cyril Canete
    e-mail               : cyril.canete@insa-lyon.fr
*************************************************************************/

//---------- Réalisation de la tâche <GestVoie> (fichier GestVoie.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <vector>

#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>


//------------------------------------------------------ Include personnel
#include "GestVoie.h"
#include "Appli.h"

#include "Voiture.h"
#include "Outils.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static TypeVoie dirVoie;

static std::vector<pid_t> voituresDeplacement; //liste des voitures en deplacement

static EtatFeux * etatFeux;
int fileVoiture;

//------------------------------------------------------ Fonctions privées

static void handlerFinDeplacement (int typeSignal)
// Recherche du pid de la tache ayant envoyé le signal, et suppression
// du pid de la structure de donnée existante
{
  if (typeSignal == SIGCHLD)
  {
    // Supression dans la liste des deplacement de la voiture ayant finis
    // son déplacement
    int statut;
    pid_t pidTache = wait(&statut);
    std::vector<pid_t>::iterator it;
    it=voituresDeplacement.begin();
    while ((*it)!=pidTache)
    {
      it++;
    }
    voituresDeplacement.erase(it);
  }
}

static void  handlerFinTache (int typeSignal)
// Algorithme :
// Parcourt de la structure de donnée, et on tue toutes les taches de 
// déplacement en cours, avant de quitter la tache
{
  if (typeSignal == SIGUSR2)
  {
    // Suppression des déplacements restants
    std::vector<pid_t>::iterator it;
    it=voituresDeplacement.begin();
    while(it!=voituresDeplacement.end())
    {
      kill((*it),SIGUSR2);
      waitpid((*it),NULL,0);
      it++;
    }

    // Détachement de la mémoire
    shmdt (etatFeux);

    // Fin de la tache
    exit(0);
  }
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Voie( TypeVoie voie, int idEtatFeux, int idFileVoiture )
// Algorithme :
// Phase d'initilisation triviale
// Phase moteur : attente d'une voiture dans la BAL pui affichage de celle - ci
// Une fois affichée, on attend que le feux passe au vert et on déplace la voiture
// après l'avoir ajoutée à la structure de donnée stoquant les voitures en déplacement
{

	//  =====  Initialisation  ===== //

    dirVoie = voie;
    fileVoiture = idFileVoiture ;
    struct MsgVoiture msg;


    //	-- Mise en place HANDLER finDeplacement et finTache
    struct sigaction actionFinDeplacement;
    struct sigaction actionFinTache;

    actionFinTache.sa_handler = handlerFinDeplacement;
    actionFinTache.sa_flags = 0;
    sigaction (SIGCHLD, &actionFinTache, NULL);

    actionFinTache.sa_handler = handlerFinTache;
    actionFinTache.sa_flags = 0;
    sigaction (SIGUSR2, &actionFinTache, NULL);

    //	-- Attachement memoire partagee
    etatFeux = (EtatFeux *) shmat(idEtatFeux, NULL, 0);
  	

  	//  =====  Moteur ===== //

    for (;;)
    {
      // Attente de la prochaine voiture à traiter par la voie en question
      // if(msgrcv(myBAL, &msg, TAILLE_MSG_VOITURE, nVoie, 1)!=-1)
      // {
      //   // Display     
      //   DessinerVoitureFeu(msg.uneVoiture.numero, msg.uneVoiture.entree, msg.uneVoiture.sortie);
      //   OperationVoie (MOINS, nVoie);

      //   if (nVoie == NORD || nVoie == SUD)
      //   {
      //     // Attente du passage du feux au vert
      //     while (!feux->nS)
      //     {
      //       sleep(1);
      //     }
          
      //     // Création de la tache de déplacement de la voiture et ajout à la liste des tache en execution
      //     pid_t voitureBouge =DeplacerVoiture(msg.uneVoiture.numero, msg.uneVoiture.entree, msg.uneVoiture.sortie);
      //     vectDeplacement.push_back(voitureBouge);
      //   }
      //   else
      //   {
      //     // Attente du passage du feux au vert
      //     while (!feux->eO)
      //     {
      //       sleep(1);
      //     }

      //     // Création de la tache de déplacement de la voiture et ajout à la liste des tache en execution
      //     pid_t voitureBouge =DeplacerVoiture(msg.uneVoiture.numero, msg.uneVoiture.entree, msg.uneVoiture.sortie);
      //     vectDeplacement.push_back(voitureBouge);
      //   }
      // }

    }
  } //----- fin de Voie






