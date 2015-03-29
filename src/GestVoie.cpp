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
#include "GestClavier.h"

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
// Mode d'emploi :
// Supprime la voiture (son pid) du vecteur voituresDeplacement quand la voiture est sortie du Carrefour
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
// Mode d'emploi :
// Parcours la liste des voiture en déplacement et tue chaque tache voiture avant de tuer la tache gestVoie.
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
void GestVoie( TypeVoie voie, int idEtatFeux, int idFileVoiture )
// Mode d'emploi :
//	fonction GestVoie, gere le passage des voitures aux feux
//	
// Contrat :
//	x32 OS
// 
//	Algorithme :
//	- Mise en place des HANDLER finTache et finDeplacement
//	- Attachement memoire partagee
//	- Gestion des voitures au feu 
{

	//  =====  Initialisation  ===== //

    dirVoie = voie;
    fileVoiture = idFileVoiture ;
    struct MsgVoiture msg;


    //	-- Mise en place HANDLER finDeplacement et finTache
    struct sigaction actionFinDeplacement;
    struct sigaction actionFinTache;

    actionFinDeplacement.sa_handler = handlerFinDeplacement;
    actionFinDeplacement.sa_flags = 0;
    sigaction (SIGCHLD, &actionFinDeplacement, NULL);

    actionFinTache.sa_handler = handlerFinTache;
    actionFinTache.sa_flags = 0;
    sigaction (SIGUSR2, &actionFinTache, NULL);

    //	-- Attachement memoire partagee
    etatFeux = (EtatFeux *) shmat(idEtatFeux, NULL, 0);
  	

  	//  =====  Moteur ===== //

    for (;;)
    {
      //	attend qu'une nouvelle voiture soit prete a etre traitee
      if(msgrcv(fileVoiture, &msg, TAILLE_MSG_VOITURE, dirVoie, 1)!=-1)
      {
        //	affiche la voiture au feu 
        DessinerVoitureFeu(msg.uneVoiture.numero, msg.uneVoiture.entree, msg.uneVoiture.sortie);
        OperationVoie (MOINS, dirVoie);

        if (dirVoie == NORD || dirVoie == SUD)
        {
          //	On est civilise, on attend que le feu passe au vert
          while (!etatFeux->feuxNS)
          {
            sleep(1);
          }
          
          //	le feu est vert, on y va : on cree la tache voiture et on fait deplacer la voiture sur l'interface
          pid_t nouvelleVoiture = DeplacerVoiture(msg.uneVoiture.numero, msg.uneVoiture.entree, msg.uneVoiture.sortie);
          voituresDeplacement.push_back(nouvelleVoiture);
        }
        else
        {
          //	On est civilise, on attend que le feu passe au vert
          while (!etatFeux->feuxEO)
          {
            sleep(1);
          }

          //	le feu est vert, on y va : on cree la tache voiture et on fait deplacer la voiture sur l'interface
          pid_t nouvelleVoiture =DeplacerVoiture(msg.uneVoiture.numero, msg.uneVoiture.entree, msg.uneVoiture.sortie);
          voituresDeplacement.push_back(nouvelleVoiture);
        }
      }

    }
  } //----- fin de Voie






