#include "ea.h"

int EvolutionEngine::nbParent = 0;
int EvolutionEngine::nbGeneration = 0;
int EvolutionEngine::nbCubeMax = 0;
Colony* EvolutionEngine::bestColony = NULL;
EvolutionEngine* EvolutionEngine::engine = NULL;
std::string EvolutionEngine::fileName = "";

/*
 *comparason pour deux colony, on ne peut pas utiliser 
 *l'operation sort des list dans la fonction select
 *car ceux sont des listes de pointeurs sur Colony
 *(redefinir operator< ne sert donc a rien puisque
 *sort() va utiliser la comparaison entre les pointeurs)
 *retourne vrai si c1 a une plus petite eval que c2
 */
bool compareColony(Colony *c1, Colony *c2)
{
  return (c1->getEval() < c2->getEval());
}

/*
 *cette methode selectionne deux colony qui remporte un tournoi
 *pour quelles se reproduisent
 */
std::list<Colony*> EvolutionEngine::selectColony()
{
  //les deux parents qui seront renvoyes
  std::list<Colony*> vectColony;
  //les deux listes dont sortiront les parents
  std::list<Colony*> tournoi1 = listColony;
  std::list<Colony*> tournoi2;
  //iterators pour selection
  std::list<Colony*>::iterator itFirst;
  int sizeTournoi2 = 0;
  int idx = 0;

  //on remplit les deux listes en enlevant certains elements de la premiere pour les mettre dans la second
  //la taille de tournoi2 = tournoi1/2
  sizeTournoi2 = (int)(tournoi1.size()/2);   
 
  for(int i = 0 ; i < sizeTournoi2 ; i++)
    {
      //on place l'ite au debut
      itFirst = tournoi1.begin();
      //on tire un element aleatoirement dans tournoi1 et on le fait passe dans tournoi2
      idx = (int)(((double)rand() / ((double)RAND_MAX + 1)) * (tournoi1.size()-1));
      advance(itFirst,idx);
      tournoi2.splice(tournoi2.begin(),tournoi1,itFirst);
    }

  //on effectue un tournoi a n pour la premiere liste, le gagnant est ajoute a vectColony pour croisement
  tournoi1.sort(compareColony);
  vectColony.push_back(tournoi1.back());

  //idem pour la deuxieme list
  tournoi2.sort(compareColony);
  vectColony.push_back(tournoi2.back());
 
  return vectColony;
};

/*
 *les deux parents contenus dans le vecteur sont croises pour retourner l'enfant
 */
Colony* EvolutionEngine::crossColony(std::list<Colony*> listColony)
{
  Colony* colony = NULL;
  Colony *dad = listColony.front();
  Colony *mom = listColony.back();
  double **w;
  double r;
  int iSize = dad->getInSize();
  int oSize = dad->getOutSize();
  
  w = new double*[iSize];
  for(int i = 0 ; i < iSize ; i++)
    {
      w[i] = new double[oSize];
      for(int j = 0 ; j < oSize ; j++)
	{
	  r = (double)(((double)rand() / ((double)RAND_MAX + 1)));
	  w[i][j] = (dad->getWeightMatrix()[i][j] * r) + (mom->getWeightMatrix()[i][j] * (1-r));
	}
    }

  colony = new Colony(iSize, oSize, w, dad->getNbCube());

  for(int i = 0 ; i < iSize ; i++)
    {
      delete w[i];
    }
      
  delete [] w;

  return colony;
};

/*
 *la colony passee en parametre est mutee (par mutation des coeff de sa matrice)
 */
void EvolutionEngine::mutateColony(Colony *colony)
{
 //chance d'un coeff d'etre mute
  double tauxMutation = 0.2f;
  //taux de mutation d'un coeff pour savoir s'il faut ou non le muter
  double tauxMutationCoeff = 0.0f;

  /*
   *si un coeff est mute il sera pris dans l'intervalle [minCoeff,maxCoeff]
   *avec minCoeff = coeff * 0.75 et maxCoeff = coeff * 1.25
   *minCoeff et maxCoeff sont bornes a [0,1]
   */
  double minCoeff = 0.0f;
  double maxCoeff = 0.0f;

  for(int i = 0 ; i < colony->getInSize() ; i++)
    {
      for(int j = 0 ; j < colony->getOutSize() ; j++)
	{
	  tauxMutationCoeff =  (double)(((double)rand() / ((double)RAND_MAX + 1))); 
	  
	  if(tauxMutationCoeff < tauxMutation)
	    {
	      minCoeff = colony->getWeightMatrix()[i][j] * 0.75f;
	      maxCoeff = colony->getWeightMatrix()[i][j] * 1.25f;

	      if(maxCoeff>1)maxCoeff = 1.0f;

	      colony->getWeightMatrix()[i][j] = minCoeff + (double)((double)(((maxCoeff-minCoeff))*rand()) / ((double)RAND_MAX + 1));
	    }
	}
    }
};

/*
 *la colony passee en parametre est ajoutee a la liste si elle est meilleure
 *qu'une des autres colony dans la liste
 */
void EvolutionEngine::addColony(Colony *colony)
{
  //on recopie et trie la liste de colony pour savoir rapidement si on doit ajouter la colonie
  std::list<Colony*> orderedColony = listColony;
  Colony *badColony = NULL;

  //on trie et recupere la plus mauvaise colony
  orderedColony.sort(compareColony);
  badColony = orderedColony.front();

  //si l'enfant est meilleure que la plus mauvaise des colony elle prend sa place
  if(badColony->getEval() < colony->getEval())
    {
      //on enleve la plus mauvaise
      listColony.remove(badColony);
      //on ajoute l'enfant
      listColony.push_back(colony);

      //on conserve la meilleure colony
      if(colony->getEval() > bestColony->getEval())
	{
	  std::cout<<"NEW BEST COLONY !!"<<std::endl;
	  bestColony = colony;
	}
    }
};

/*
 *cette methode va creer nbEval colonies et les evaluer, les selectionner,
 *les croiser, et muter la descendance jusqu'a l'obtention d'un certain 
 *resultat (oui mais lequel ? on commencera par un nombre fixe de generation)
 */
void EvolutionEngine::startEvolution()
{
  Colony *colony = NULL;
  std::list<Colony*> listColonyParent;
  //initialiser bestColony avec une eval a 0 pour test de la meilleur colony
  bestColony = new Colony();
  bestColony->setEval(0.0);

  //initialisation des parents
  initParent();

  for(int numGene = 0 ; numGene < nbGeneration ; numGene++)
    {
      std::cout<<"---------------- GENERATION n° : "<<numGene<<" -----------------"<<std::endl;
      
      //selection de deux parents
      listColonyParent = selectColony();
      //croisement des deux parents selectionnes
      colony = crossColony(listColonyParent);
      //mutation du descendant
      mutateColony(colony);
      //evaluation du descendant + test bestColony
      evaluationColony(colony);
      //on ajoute le descendant s'il est meilleur qu'un des parents d'origine
      addColony(colony);
      
      std::cout<<"---------------- BEST COLONY EVAL :"<<bestColony->getEval()<<" --------------"<<std::endl<<std::endl;
    }
  
  //on evalue et enregistre les infos de la meilleure colony
  evaluationColony(bestColony,true);
};

/*
 *cette methode genere les parents, les evalue et initialise bestColony 
 *avec le parent de meilleur evaluation
 *en sortie la liste de colony est initialisee 
 */
void EvolutionEngine::initParent()
{
  Colony* colony = NULL;

  //on cree et evalue les colony
  for(int numParent = 0 ; numParent < nbParent ; numParent++)
    {
      std::cout<<"Parent n° : "<<numParent<<std::endl;
	     
      //RAZ des reference sur le RNA
      clearEnv();
      Ant::resetRNA();
      SimpleRNA::resetRNA();

      //on initialise le tout
      extractConfigFile(fileName);

      //on lance la sculpture jusqu'a atteindre nbCube
      colonieAction(nbCubeMax);

      //creation et evaluation de la colonie
      colony = new Colony(SimpleRNA::getNbIn(),SimpleRNA::getNbOut(),Ant::getRNA()->getWeightMatrix(),nbCubeMax);
      colony->evalMedianHeight();
      
      std::cout<<"Resultat : "<<colony->getEval()<<std::endl<<std::endl;
            
      //on conserve la meilleur colony
      if(colony->getEval() > bestColony->getEval())
	{
	  //bestColony = new Colony(*colony);
	  bestColony = colony;
	}

      //on l'ajoute a la liste des colonies
      listColony.push_back(colony);
    } 
};
  
/*
 *cette methode evalue la colony passee en parametre
 *si c'est la bestColony (autrement dit si on est a 
 *la fin de l'algo genetique on la reevalue,
 *on sauvegarde sa matrice et sa sculpture)
 *En la reevaluant on peut obtenir une note moins bonne
 *mais cela permet de ne pas sauvegarder tout a chaque 
 *fois que l'on a une new bestColony
 */
void EvolutionEngine::evaluationColony(Colony *colony, bool isBest)
{
  //std::cout<<"EVAL [IN]"<<std::endl;
  //on set la matrice du RNA avec celle de la colony que l'on veut evaluer
  Ant::getRNA()->setMatrix(colony->getWeightMatrix());

  //nettoyage de l'env avant nouvelle sculpture
  clearEnv();

  //on initialise le tout
  extractConfigFile(fileName);

  //on lance la sculpture jusqu'a atteindre nbCube
  colonieAction(nbCubeMax);
   
  if(isBest)
    {
      sculpToAntFile(1, NB_ANT);
      bestColony->saveEvalMatrix();
    }
  else
    { 
      //evaluation de la colonie
      colony->evalMedianHeight();
      std::cout<<"Evaluation enfant : "<<colony->getEval()<<std::endl;
    }

  //std::cout<<"EVAL [OUT]"<<std::endl;
};


void EvolutionEngine::setFilename(std::string s)
{
  std::string* str = new std::string(s);
  fileName = *str;
};
