#include "ant.h"

//la target de la colonie
Coord Ant::target(SIZE_ENV/2,SIZE_ENV/2,SIZE_ENV/2);

//le RNA de la colonie
SimpleRNA* Ant::rna = NULL;

Ant::Ant()
{
  /*placement a l'origine par defaut*/
  pos.x = 0;
  pos.y = 0;
  pos.z = 0;

  alpha = (double)(((double)rand() / ((double)RAND_MAX + 1)));
 
  alphaPos = (double)(((double)rand() / ((double)RAND_MAX + 1)));
  betaPos = (double)(((double)rand() / ((double)RAND_MAX + 1))); 
  gammaPos = (double)(((double)rand() / ((double)RAND_MAX + 1)));

  /*representee par une couleur au hasard*/
  color[0] = (float)(((double)rand() / ((double)RAND_MAX + 1)));
  color[1] = (float)(((double)rand() / ((double)RAND_MAX + 1)));
  color[2] = (float)(((double)rand() / ((double)RAND_MAX + 1)));
};


Ant::Ant(float r, float g, float b, Coord c)
{
  pos = c;

  alpha = (double)(((double)rand() / ((double)RAND_MAX + 1)));
  
  alphaPos = (double)(((double)rand() / ((double)RAND_MAX + 1)));
  betaPos = (double)(((double)rand() / ((double)RAND_MAX + 1))); 
  gammaPos = (double)(((double)rand() / ((double)RAND_MAX + 1)));
  
  color[0] = r;
  color[1] = g;
  color[2] = b;
}

Ant::Ant(float r, float g, float b, Coord c, double a)
{
  pos = c;
  
  alpha = a;
  
  alphaPos = (double)(((double)rand() / ((double)RAND_MAX + 1)));
  betaPos = (double)(((double)rand() / ((double)RAND_MAX + 1))); 
  gammaPos = (double)(((double)rand() / ((double)RAND_MAX + 1)));
  
  
  color[0] = r;
  color[1] = g;
  color[2] = b;
};

Ant::Ant(const Ant &a)
{
  pos = a.pos;

  alpha = a.alpha;
  
  alphaPos = a.alphaPos;
  betaPos = a.betaPos;
  gammaPos = a.gammaPos;
  

  for(int i = 0 ; i < 3 ; i++)
    {
      this->color[i] = a.color[i];
    }
};


void Ant::setColor(GLfloat c[3])
{
  color[0] = c[0];
  color[1] = c[1];
  color[2] = c[2];
}

/*
 *return true si au moins un des 6 voisins est un cube
 *permet de savoir si la case de coord(i,j,k) est 
 *une position possible
 */
bool Ant::gotNeighbor(int i, int j, int k)
{
  /*true = la case a au moins un cube comme voisin pas la peine de continuer la recherche*/
  bool foundNeighbor = false;

  /*recherche si la position a un cube comme voisin on s'arrete des que l'on trouve un cube adjacent*/
  for(int ii = (i - 1) ; ii < (i + 1) +1 && !foundNeighbor; ii++)
    {
      for(int jj = (j - 1) ; jj < (j + 1) +1 && !foundNeighbor; jj++)
	{
	  for(int kk = (k - 1) ; kk < (k + 1) +1 && !foundNeighbor; kk++)
	    {
	      /*on ne cherche pas les voisins en diago <=> chgnt /1 seule coord*/
	      /*    recherche/k          recherche/j        recherche/i    */
	      /*on ne peut pas choisir la case sur laquelle on est situe*/
	      /*ni celle dont on cherche le voisinage*/
	      /*on reste dans les limites de l'env*/
	      if(((ii==i && jj==j) || (ii==i && kk==k) || (jj==j && kk==k)) 
		 && ((ii != pos.x) || (jj != pos.y) || (kk != pos.z))
		 && ((ii != i) || (jj != j) || (kk != k))
		 && ((ii>=0) && (jj>=0) && (kk>=0) && (ii<SIZE_ENV) && (jj<SIZE_ENV) && (kk<SIZE_ENV)))
		{
		  /*s'il le voisinage de i,j,k contient un cube on peut y aller*/
		  if(env[ii][jj][kk] != NULL)
		    {
		      foundNeighbor = true;
		    }
		}
	    }
	}
    }   

  return foundNeighbor;	
}


/*
 *On renvoi la liste de toutes les cases ou 
 *la fourmi peut se deplacer/ou construire (ie case vide, au
 *sol ou en contact avec un cube)
 */
std::list<Coord> Ant::getPosiblePosition()
{
  /*liste des position possibles (ie vide ou en contact avec un cube*/
  std::list<Coord> nextMove;
  /*nb de case du deplacement des fourmis*/
  int motion = 1;
  
  for(int i = (pos.x - motion) ; i < (pos.x + motion) +1 ; i++)
  {
    for(int j = (pos.y - motion) ; j < (pos.y + motion) +1 ; j++)
    {
      for(int k = (pos.z - motion) ; k < (pos.z + motion) +1; k++)
      {
        /*on ne peut pas choisir la case sur laquelle on est situe && on reste dans les limites de l'env*/
        if(((i != pos.x) || (j != pos.y) || (k != pos.z)) && ((i>=0) 
        && (j>=0) && (k>=0) && (i<SIZE_ENV) && (j<SIZE_ENV) && (k<SIZE_ENV)))
        {
          /*s'il n'y a pas de cube a cet endroit*/
          if(env[i][j][k] == NULL)
          {
            /*on peut se deplacer sur le sol (evite une recherche des voisins)*/
            if(j == 0)
            {
             nextMove.push_back(Coord(i,j,k));
            }
            else
            {
              /*s'il le voisinage de i,j,k contient au moins un cube alors on peut y aller*/
              if(gotNeighbor(i,j,k))
              {
               nextMove.push_back(Coord(i,j,k));
              }
            }
          }
        }
      }
    }
  }
  
  return nextMove;
}

/*
 *recherche dans le vecteur de coordonnees
 *celles qui ont le voisinage avec la + forte odeur
 *de phero de construction, renvoi les positions
 *ayant ce meilleur voisinage
 */
std::list<Coord> Ant::getBestBuild(std::list<Coord> listCoord,float *bestColorSoFar)
{
  /*coordonnees correspondante a la meilleure couleur*/
  std::list<Coord> bestCoordSoFar;
  /*iterator de parcours de la liste*/
  std::list<Coord>::iterator itCoord;

  for(itCoord = listCoord.begin(); itCoord != listCoord.end(); itCoord++)
    {
      Coord c = *itCoord;
      for(int ii = (c.x - 1) ; ii < (c.x + 1) +1; ii++)
	{
	  for(int jj = (c.y - 1) ; jj < (c.y + 1) +1; jj++)
	    {
	      for(int kk = (c.z - 1) ; kk < (c.z + 1) +1; kk++)
		{
		  /*on ne cherche pas les voisins en diago <=> chgnt /1 seule coord*/
		  /*    recherche/k          recherche/j        recherche/i    */
		  /*ni celle dont on cherche le voisinage*/
		  /*on reste dans les limites de l'env*/
		  if(((ii==c.x && jj==c.y) || (ii==c.x && kk==c.z) || (jj==c.y && kk==c.z)) 
		     && ((ii != c.x) || (jj != c.y) || (kk != c.z))
		     && ((ii>=0) && (jj>=0) && (kk>=0) && (ii<SIZE_ENV) && (jj<SIZE_ENV) && (kk<SIZE_ENV)))
		    {
		      /*s'il le voisinage de i,j,k contient un cube on test son odeur de construction*/
		      if(env[ii][jj][kk] != NULL)
			{
			  float pheroConst  = env[ii][jj][kk]->getPheroConst();
			  if(pheroConst > *bestColorSoFar)
			    {/*si on trouve une meilleure odeur on nettoye la liste et on set la nouvelle meilleure couleur*/
			      *bestColorSoFar = pheroConst;
			      bestCoordSoFar.clear();
			    }
			  /*on ajoute la nouvelle coord a la liste des meilleure si son odeur est egale*/
			  if(pheroConst == *bestColorSoFar)
			    {
			      bestCoordSoFar.push_back(c);
			    }
			}
		    }
		}
	    }
	}
    }

  return bestCoordSoFar;
}

/*
 *methode qui recupere la moyenne des phero (const et move) du voisinage de la coord c
 */
void Ant::getNeighborInfo(Coord c, double* newPheroMove, double* newPheroConst)
{
  int nbVoisin = 0;
  *newPheroMove = 0;
  *newPheroConst = 0;
  
  for(int ii = (c.x - 1) ; ii < (c.x + 1) +1; ii++)
    {
      for(int jj = (c.y - 1) ; jj < (c.y + 1) +1; jj++)
	{
	  for(int kk = (c.z - 1) ; kk < (c.z + 1) +1; kk++)
	    {
	      /*ni celle dont on cherche le voisinage*/
	      /*on reste dans les limites de l'env*/
	      if(((ii != c.x) || (jj != c.y) || (kk != c.z))
		 && ((ii>=0) && (jj>=0) && (kk>=0) && (ii<SIZE_ENV) && (jj<SIZE_ENV) && (kk<SIZE_ENV)))
		{
		  /*s'il y a un cube dans le voisinage on en tient compte pour calculer les new phero*/
		  if(env[ii][jj][kk] != NULL)
		    {
		      nbVoisin++;
		      *newPheroMove += env[ii][jj][kk]->getPheroMove();
		      *newPheroConst += env[ii][jj][kk]->getPheroConst();
		    }
		}
	    }
	}
    }

  /*on fait la moyenne pour chacune des pheros*/
  if(nbVoisin != 0)
    {
      *newPheroMove = (double)((double)*newPheroMove/(double)nbVoisin);
      *newPheroConst = (double)((double)*newPheroConst/(double)nbVoisin);
    }
  else
    {
      *newPheroMove = 0;
      *newPheroConst = 0;
    }
}

/*
 *recherche dans le vecteur de coordonnees
 *celles qui ont le voisinage avec la + forte odeur
 *de phero de deplacement, renvoi les positions
 *ayant ce meilleur voisinage
 */
std::list<Coord> Ant::getBestPositions(std::list<Coord> listCoord,float *bestSmellSoFar)
{
   /*coordonnees correspondante a la meilleure odeur*/
  std::list<Coord> bestCoordSoFar;
  /*iterator de parcours de la liste*/
  std::list<Coord>::iterator itCoord;

  for(itCoord = listCoord.begin(); itCoord != listCoord.end(); itCoord++)
    {
      Coord c = *itCoord;
      for(int ii = (c.x - 1) ; ii < (c.x + 1) +1; ii++)
	{
	  for(int jj = (c.y - 1) ; jj < (c.y + 1) +1; jj++)
	    {
	      for(int kk = (c.z - 1) ; kk < (c.z + 1) +1; kk++)
		{
		  /*on ne cherche pas les voisins en diago <=> chgnt /1 seule coord*/
		  /*    recherche/k          recherche/j        recherche/i    */
		  /*ni celle dont on cherche le voisinage*/
		  /*on reste dans les limites de l'env*/
		  if(((ii==c.x && jj==c.y) || (ii==c.x && kk==c.z) || (jj==c.y && kk==c.z)) 
		     && ((ii != c.x) || (jj != c.y) || (kk != c.z))
		     && ((ii>=0) && (jj>=0) && (kk>=0) && (ii<SIZE_ENV) && (jj<SIZE_ENV) && (kk<SIZE_ENV)))
		    {
		      /*s'il le voisinage de i,j,k contient un cube on test son odeur de deplacement*/
		      if(env[ii][jj][kk] != NULL)
			{
			  float pheroMove = env[ii][jj][kk]->getPheroMove();
			  if(pheroMove > *bestSmellSoFar)
			    {/*si on trouve une meilleure odeur on nettoye la liste et on set la nouvelle meilleure odeur*/
			      *bestSmellSoFar = pheroMove;
			      bestCoordSoFar.clear();
			    }
			  /*on ajoute la nouvelle coord a la liste des meilleure si sa dist est egale*/
			  if(pheroMove == *bestSmellSoFar)
			    {
			      bestCoordSoFar.push_back(c);
			    }
			}
		    }
		}
	    }
	}
    }

  return bestCoordSoFar;                 
}

/*
 *return une position prise au hasard dans 
 *le vecteur passe en parametre (choix uniforme)
 */
Coord Ant::setRandomPosition(std::list<Coord> listCoord)
{
  std::list<Coord>::iterator itCoord = listCoord.begin();

  /*supprime les doublons*/
  listCoord.unique();
  /*choix aleatoire de l'indice a retourner*/
  int idx = (int)(((double)rand() / ((double)RAND_MAX + 1)*listCoord.size()));

  //for(int i = 0 ; i < idx ; i++){itCoord++;}
  //sans doute plus rapide que la methode avec un for
  advance(itCoord, idx-1);

  return *itCoord;
}


SimpleRNA* Ant::getRNA()
{
  if(Ant::rna == NULL)
    {
      //set les proprietes du RNA
      //pour chaque voisin on recupere 4 informations
      SimpleRNA::setNbIn(27*4);
      //2: les actions
      //27: les 27 voisins possedent une "note"
      SimpleRNA::setNbOut( 2 + 27);
      Ant::rna = SimpleRNA::getInstance();    
    }
 
  return Ant::rna;
};

/*
 *Initialise le vecteur d'entree du RNA avec pour chaque voisins
 *la qte de phero de deplacement
 *la qte de phero de construction
 *sa distance a la cible de la colonie
 *si cet endroit est occupe par un cube ou non
 */
std::vector<double> Ant::initInValue()
{
  std::vector<double> inValue;
  double pheroMove = 0;
  double pheroConst = 0;
  double influenceDist = 0;
  double isCube = 0;
  //on parcours le voisinage 3D
  for(int i = (pos.x - 1) ; i < (pos.x + 1) +1; i++)
    {
      for(int j = (pos.y - 1) ; j < (pos.y + 1) +1; j++)
	{
	  for(int k = (pos.z - 1) ; k < (pos.z + 1) +1; k++)
	    {
	      pheroMove = 0;
	      pheroConst = 0;
	      influenceDist = 0;
	      isCube = 0;
	       
	      if((i >=0) && (j>=0) && (k>=0) && (i<SIZE_ENV) && (j<SIZE_ENV) && (k<SIZE_ENV))
		{
		  //si on est dans l'env
		  Coord c(i,j,k);
		  
		  if(env[i][j][k] != NULL)
		    {//s'il y a un cube on prend ces valeurs de phero et on met isCube a 1
		      pheroMove = env[i][j][k]->getPheroMove();
		      pheroConst = env[i][j][k]->getPheroConst();
		      isCube = 1;
		    }
		  else
		    {//s'il n'y a pas de cube on recupere la moyenne des pheromones du voisinage et on met isCube a 0
		      getNeighborInfo(c,&pheroMove,&pheroConst);
		      isCube = 0;
		    }

		  //dans tous les cas on calcul la distance de la coord a la target 
		  influenceDist = c.influenceDistance(Ant::target);
		}
	     
	      inValue.push_back(pheroMove);
	      inValue.push_back(pheroConst);
	      inValue.push_back(influenceDist);
	      inValue.push_back(isCube);
	    }
	}
    }
  

  return inValue;
}

/*
 *fonction utilitaire qui retourne la coordonnee correspondante (cible de l'action)
 *grace a l'indice dans le vecteur de sortie du RNA 
 */
Coord Ant::getCoordByIndex(int idxOut)
{ 
  int x = pos.x;
  int y = pos.y;
  int z = pos.z;
  
  if(idxOut < 9)
    {
      x--;
      if(idxOut < 3)
	{
	  y--; 
	  if(idxOut == 0)z--;
	  else if(idxOut == 2)z++;
	}
      else if(idxOut >= 6)
	{
	  y++;
	  if(idxOut == 6)z--;
	  else if(idxOut == 8)z++;
	}
      else
	{
	  if(idxOut == 3)z--;
	  else if(idxOut == 5)z++; 
	}
    }
  else if(idxOut >= 18)
    {
      x++;
      if(idxOut < 21)
	{
	  y--;
	  if(idxOut == 18)z--;
	  else if(idxOut == 20)z++;
	}
      else if(idxOut >= 24)
	{
	  y++;
	  if(idxOut == 24)z--;
	  else if(idxOut == 26)z++;
	}
      else
	{
	  if(idxOut == 21)z--;
	  else if(idxOut == 23)z++; 
	}
    }
  else
    {
      if(idxOut < 12)
	{
	  y--;
	  if(idxOut == 12)z--;
	  else if(idxOut == 14)z++;
	}
      else if(idxOut >= 15)
	{
	  y++;
	  if(idxOut == 15)z--;
	  else if(idxOut == 17)z++;
	}
      else
	{
	  if(idxOut == 12)z--;
	  else if(idxOut == 14)z++; 
	} 
    }

  return Coord(x,y,z);
}


int Ant::idxByRouletteOnIdx(std::vector<double> inValue)
{
  int idxOut = 0;
  bool foundTarget = false;
  std::vector<double> outValue = Ant::rna->evaluate(inValue);
  std::list<double>::iterator itOutSorted;
  std::vector<double>::iterator itTarget;
  
  double probaTarget = (double)(((double)rand() / ((double)RAND_MAX + 1)));  

  //27 valeurs de sortie du RNA triee dans l'ordre decroissant
  std::list<double> outValueList = Ant::rna->getOutValueSorted();

  //on met l'iterateur au debut pour avoir un pointeur sur la bonne case
  itOutSorted = outValueList.begin();
    
  //tirage de la cible via une roulette/histogramme
  for(itTarget = Ant::rna->histogramme.begin() ; itTarget != Ant::rna->histogramme.end() && !foundTarget ; itTarget++)
    {
      if(probaTarget < (*itTarget))
	{
	  foundTarget = true;
	}
      itOutSorted++;
    }

  //retrouver l'indice dans la liste non triee grace a la valeur (outValueList[idxListTriee] == inValue[i])
  foundTarget = false;
  
  //on commence a 2 car les 2 premieres valeurs sont celles qui designe l'action
  for(unsigned int i = 2 ; i < outValue.size() && !foundTarget ; i++)
    {
      if((*itOutSorted) == outValue[i])
	{
	  foundTarget = true;
	}
      idxOut++;
    }

  return idxOut;
}

int Ant::idxByRouletteOnValue(std::vector<double> inValue)
{
  int idxOut = 0;
  bool foundTarget = false;
  std::vector<double>::iterator itOut;
  //effectue l'evaluation du vecteur de sortie et retourne la somme des 27 dernieres sorties
  double sum =Ant::rna->evaluateAntGetSum(inValue);
  //recupere le vecteur de sortie
  std::vector<double> outValue = Ant::rna->getOutValues();
  //probaTarget [0,sum]
  double probaTarget = (double)(((double)rand() / ((double)RAND_MAX + 1)) * sum);
  
  //on vire les valeurs pour les actions
  itOut = outValue.begin();
  itOut++; itOut++;
  sum = 0;

  //on tire l'indice par roulette
  for( ; itOut < outValue.end() && !foundTarget ; itOut++)
    {
      sum += (*itOut);
      if(sum > probaTarget)
	{
	  foundTarget = true; 
	}
      idxOut++; 
    }
  
  return idxOut;
}


/*
 *on parcours l'ensemble du voisinage pour remplir le vecteur d'entree du RNA
 *on evalue ce vecteur d'entree et on decide de l'action et de la cible de cette action
 *pour chaque position du voisinage on a 
 *1: la qte de pheromone de deplacement (0 si hors env)
 *2: la qte de pheromone de construction (0 si hors env)
 *3: la distance par rapport a Ant::target (0 si hors env)
 *4: cette position est-elle un cube ? (0 si vide 1 si cube)
 */
void Ant::actionRNA()
{
  double maxProba;
  double probaAction;
  
  int idxOut = 0;
 
  //on recupere/initialise le rna
  Ant::rna = getRNA();

  //valeurs d'entree du RNA
  std::vector<double> inValue = initInValue();
 
  //on recupere l'indice de la cible apres evaluation par le RNA
  //idxOut = idxByRouletteOnIdx(inValue);
  idxOut = idxByRouletteOnValue(inValue);

  //on recupere la coordonnee correspondante
  Coord target = getCoordByIndex(idxOut);

  maxProba = Ant::rna->getFirstAction() + Ant::rna->getSecondAction();
  probaAction  =  (double)(((double)rand() / ((double)RAND_MAX + 1)) * maxProba);
 
  //tirage par roulette de l'action a effectuer
  if(probaAction  > Ant::rna->getFirstAction())
    {
      //std::cout<<"MOVE TO "<<target<<std::endl;
      moveToPosition(target);
    }
  else
    {
      //std::cout<<"CONST TO "<<target<<std::endl;
      if(!constructToPosition(target))
	{
	  moveToPosition(target);
	}
    }
}


void Ant::moveToPosition(Coord c)
{
  bool contains = false;
  std::list<Coord>::iterator it;
  Coord tmp = pos;

  //si on est au sol ou si on a des voisins directs
  if(gotNeighbor(pos.x,pos.y,pos.z) || pos.y==0)
    {
      /*on recupere tout les voisins possibles*/
      std::list<Coord> nextMove = getPosiblePosition();
     
      /*si on a des voisins directs (ie on est pas bloque)*/
      if(nextMove.size() != 0)
	{  
	  /*si la position choisie par le RNA fait partie de ces voisins possibles*/
	  for(it = nextMove.begin() ; it != nextMove.end() && !contains; it++)
	    {
	      if((*it) == c)
		{
		  contains = true;
		}
	    }
	      
	  /*si la coordonnee ou on veut aller est dans les mouvements possibles et si elle n'est pas occupee par un cube on y va */
	  if(contains && (env[c.x][c.y][c.z] == NULL))
	    {
	      pos = c;
	    }
	  /* 
	     else
	     {
	     constructToPosition(c); 
	     }
	  */      
	}
      else
	{
	  //si la fourmi est bloquee on la replace aleatoirement dans l'env
	  Coord c((int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),
		  (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),
		  (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))));
	  pos = c;
	}
    }
  else //si on a pas de voisin <=> on est en l'air => on tombe
    {
      //si la fourmi n'a pas de voisinage on la fait tomber jusqu'Ã  trouver un voisinage
      pos.y--;  
      //si elle passe en y<0 (ie le sol est autorise pour les deplacement mais n'est pas considerer comme un voisin)
      //on recommence la procedure
      if(pos.y<0)
	{
	  Coord c((int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),
		  (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),
		  (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))));
	  pos = c;
	}    
    }  
}


bool Ant::constructToPosition(Coord c)
{
  //valeur de retour
  bool res = false;
  
  /*qte phero donnee au cube construit (fonction de son voisinage)*/
  double pMoveNeighbor = 0;
  double pConstNeighbor = 0;

  /*qte phero donnee au cube construit (fonction de sa position)*/
  double pMovePos = 0;
  double pConstPos = 0;

  /*qte finale de phero donne au cube*/
  double pMove = 0;
  double pConst = 0;
  
  /*on construit s'il n'y a rien a cet endroit et si cette coord a un voisin direct*/
  if(gotNeighbor(c.x,c.y,c.z) && (env[c.x][c.y][c.z] == NULL))
    {
      getNeighborInfo(c,&pMoveNeighbor,&pConstNeighbor);
	  
      //attirance par un point (ie la target de la colonie)
      pMovePos = pConstPos = c.influenceDistance(Ant::target);
      
      //calcul des valeurs de pheromones
      pMove = (1-alpha)*pMoveNeighbor + alpha*(pMovePos);
      pConst = (1-alpha)*pConstNeighbor + alpha*(pConstPos);
      
      //ajout du nouveau cube dans la liste
      Cube *cube = new Cube((float)((double)c.y/(double)(SIZE_ENV)),c,pMove,pConst);
      cubeList.push_back(cube); 
     
      res = true;
    }

  return res;
}


void Ant::actionSimpleRNA()
{
  Ant::rna = getRNA();
     
  //quantite de pheromones de deplacement et de construction
  double pheroMove;
  double pheroConst;
 
  //indice de la plus grande valeur de sortie du RNA
  int idxRNA;
 
  //valeurs d'entree du RNA
  std::vector<double> inValue;

  //on recupere la moyenne des pheros du voisinage courant
  getNeighborInfo(pos,&pheroMove,&pheroConst);
 
  inValue.push_back(pheroMove);
  inValue.push_back(pheroConst);
 
  idxRNA = Ant::rna->evaluateMaxIndex(inValue);
 
  if(idxRNA == 0)
    {
      move();     
    }
  else
    {
      if(!construct())
	{
	  move();                
	}
    }
 
};

void Ant::move()
{
  //qte pheromones de deplacement
  float qtePheroMove = 0;
     
  //si on est au sol ou si on a des voisins directs
  if(gotNeighbor(pos.x,pos.y,pos.z) || pos.y==0)
    {
      /*on recupere tout les voisins possibles*/
      std::list<Coord> nextMove = getPosiblePosition();
    
      /*si on a des voisins directs (ie on est pas bloque)*/
      if(nextMove.size() != 0)
	{  
	  /*on cherche si un de ces voisins a un voisinage attrayant (pheoromone deplacement)*/
	  std::list<Coord> bestMove = getBestPositions(nextMove,&qtePheroMove);
      
	  /*si le voisinage n'est pas assez attrayant on choisit au hasard parmis les voisins possible mais on ne construit pas */
	  if(qtePheroMove == 0 )
	    {
	      pos = setRandomPosition(nextMove);
	    }
	  else
	    {
	      /*on choisit au hasard dans les positions au voisinage le plus attrayant*/
	      pos = setRandomPosition(bestMove); 
	    }          
	}
      else
	{
	  //si la fourmi est bloquee on la replace aleatoirement dans l'env
	  Coord c((int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),
		  (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),
		  (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))));
	  pos = c;
	}
    }
  else //si on a pas de voisin <=> on est en l'air => on tombe
    {
      //si la fourmi n'a pas de voisinage on la fait tomber jusqu'Ã  trouver un voisinage
      pos.y--;  
      //si elle passe en y<0 (ie le sol est autorise pour les deplacement mais n'est pas considerer comme un voisin)
      //on recommence la procedure
      if(pos.y<0)
	{
	  Coord c((int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),
		  (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),
		  (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))));
	  pos = c;
	}    
    }    
};
  
bool Ant::construct()
{
  //valeur de retour
  bool res = false;
     
  /*qte phero donnee au cube construit (fonction de son voisinage)*/
  double pMoveNeighbor = 0;
  double pConstNeighbor = 0;

  /*qte phero donnee au cube construit (fonction de sa position)*/
  double pMovePos = 0;
  double pConstPos = 0;

  /*qte finale de phero donne au cube*/
  double pMove = 0;
  double pConst = 0;

  //qte la plus elevee de pheromone de construction
  float bestConstSoFar = 0;

  std::list<Coord> nextBuild = getPosiblePosition();
  
  // s'il y a des emplacements sur lesquels construire 
  if(nextBuild.size() != 0)
    {
      //on cherche si un de ces voisins a un voisinage attrayant (pheoromone construction)
      std::list<Coord> bestBuild = getBestBuild(nextBuild,&bestConstSoFar);
    
      // s'il y a des emplacements mieux que d'autres
      if(bestBuild.size() != 0 && bestConstSoFar > 0)
	{
	  Coord c = setRandomPosition(bestBuild);
	  getNeighborInfo(c,&pMoveNeighbor,&pConstNeighbor);
	  
	  //attirance par un point
	  pMovePos = pConstPos = c.influenceDistance(Ant::target);
      
	  //calcul des valeurs de pheromones
	  pMove = (1-alpha)*pMoveNeighbor + alpha*(pMovePos);
	  pConst = alpha*pConstNeighbor + (1-alpha)*(pConstPos);
      
	  //ajout du nouveau cube dans la liste
	  Cube *cube = new Cube((float)((double)c.y/(double)(SIZE_ENV)),c,pMove,pConst);
	  cubeList.push_back(cube);
	}  
    
      res = true;               
    }      
  
  return res;
};


/*
 *cette fonction regroupe les actions successives de la fourmi
 */
void Ant::action()
{
  /*odeur les + attrayantes pour la fourmi*/
  float bestSmellSoFar = 0;
  float bestConstSoFar = 0;

  /*qte phero donnee au cube construit (fonction de son voisinage)*/
  double pMoveNeighbor = 0;
  double pConstNeighbor = 0;

  /*qte phero donnee au cube construit (fonction de sa position)*/
  double pMovePos = 0;
  double pConstPos = 0;
  
  /*qte finale de phero donne au cube*/
  double pMove = 0;
  double pConst = 0;

  /*si on a pas de voisin <=> on est en l'air => on tombe cf else*/
  if(gotNeighbor(pos.x,pos.y,pos.z) || pos.y==0){
    /*on recupere tout les voisins possible*/
    std::list<Coord> nextMove = getPosiblePosition();

    /*si on a des voisins directs (ie on est pas bloque)*/
    if(nextMove.size() != 0)
      {
    	/*on cherche si un de ces voisins a un voisinage attrayant (pheoromone deplacement)*/
    	std::list<Coord> bestMove = getBestPositions(nextMove,&bestSmellSoFar);
	
    	/*si le voisinage n'est pas assez attrayant on choisit au hasard parmis les voisins possible mais on ne construit pas */
    	if(bestSmellSoFar == 0 )
    	  {
    	    pos = setRandomPosition(nextMove);
    	  }
    	else
    	  {
	    /*on choisit au hasard dans les positions au voisinage le plus attrayant*/
    	    pos = setRandomPosition(bestMove); 
	    /*
	      float probaCube = (float)(((double)rand() / ((double)RAND_MAX + 1)));
      
	      if(probaCube > 0.75)
    	      {
	      Cube *cube = new Cube(constructionColor[0],constructionColor[1],constructionColor[2],pos,pos.y*0.01,pos.x*0.01);
	      cubeList.push_back(cube);
    	      }
    	      
	    */
    	  }
	
	std::list<Coord> nextBuild = getPosiblePosition();

        // s'il y a des emplacements sur lesquels construire 
    	if(nextBuild.size() != 0)
    	  {
    	    //on cherche si un de ces voisins a un voisinage attrayant (pheoromone construction)
    	    std::list<Coord> bestBuild = getBestBuild(nextBuild,&bestConstSoFar);
    
    	    // s'il y a des emplacements mieux que d'autres
    	    if(bestBuild.size() != 0 && bestConstSoFar > 0)
	      {
		Coord c = setRandomPosition(bestBuild);
		getNeighborInfo(c,&pMoveNeighbor,&pConstNeighbor);
		
		/*
		  pMovePos = (alphaPos*abs(c.x-(SIZE_ENV - SIZE_ENV/2)) + betaPos*abs(c.y-SIZE_ENV/2) + gammaPos*abs(c.z-SIZE_ENV/2))/(SIZE_ENV*3);
		  pConstPos = (alphaPos*abs(c.x-SIZE_ENV/2) + betaPos*abs(c.y-SIZE_ENV/2) + gammaPos*abs(c.z-SIZE_ENV/2))/(SIZE_ENV*3);
		*/
         
		//pour le moment on ne pondère pas les coord...
		//alphaPos = gammaPos = betaPos = 1.0;
		//(alphaPos*c.x + betaPos*c.y + gammaPos*c.z)/(SIZE_ENV*3)
        	
		//attirance par un point
		pMovePos = pConstPos = c.influenceDistance(Ant::target);
        
        	
		//calcul des valeurs de pheromones
		pMove = (1-alpha)*pMoveNeighbor + alpha*(pMovePos);
		pConst = (1-alpha)*pConstNeighbor + alpha*(pConstPos);
		//ajout du nouveau cube dans la liste
		Cube *cube = new Cube((float)((double)c.y/(double)(SIZE_ENV)),c,pMove,pConst);
		cubeList.push_back(cube);
	      }                 
    	  }   
      }
    else
      {
	//si la fourmi est bloquee on la replace aleatoirement dans l'env
	Coord c((int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),
		(int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),
		(int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))));
	pos = c;
      }
  }
  else
    {
      //si la fourmi n'a pas de voisinage on la fait tomber jusqu'Ã  trouver un voisinage
      pos.y--;  
      //si elle passe en y<0 (ie le sol est autorise pour les deplacement mais n'est pas considerer comme un voisin)
      //on recommence la procedure
      if(pos.y<0)
	{
	  Coord c((int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),
		  (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),
		  (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))));
	  pos = c;
	}
    }
};

