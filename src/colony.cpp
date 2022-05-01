#include "colony.h"

Colony::Colony(const Colony &c): inSize(c.inSize),outSize(c.outSize),nbCube(c.nbCube)
{
  eval = c.eval;
  weightMatrix = new double*[inSize];
  //allocation et recopie de la matrice de poids
  for(int i = 0 ; i < inSize ; i++)
    {
      weightMatrix[i] = new double[outSize];
      for(int j = 0 ; j < outSize ; j++)
	{
	  weightMatrix[i][j] = c.weightMatrix[i][j];
	}
    } 
};

Colony::Colony(int iSize, int oSize, double** w, int c)
{
  //set les attributs de la matrice
  inSize = iSize;
  outSize = oSize;

  eval = 0;

  //set le nombre de cube sur lequel porte l'eval
  nbCube = c;

  weightMatrix = new double*[inSize];
  //allocation et recopie de la matrice de poids
  for(int i = 0 ; i < inSize ; i++)
    {
      weightMatrix[i] = new double[outSize];
      for(int j = 0 ; j < outSize ; j++)
	{
	  weightMatrix[i][j] = w[i][j];
	}
    }
};


void Colony::saveEvalMatrix()
{
  //creation du nom de fichier
  std::time_t rawtime;
  struct tm * timeinfo;
  std::stringstream s;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  
  //dans le dossier sculpt
  s<<"./sculpt/";
  //jour et mois de l'annee
  s<<"eval_"<<timeinfo->tm_mday<<"_"<<timeinfo->tm_mon+1<<"_";
  //heure de l'enregistrement
  s<<timeinfo->tm_hour<<"_"<<timeinfo->tm_min<<"_"<<timeinfo->tm_sec<<"_";
  //le type alpha et nb ant
  s<<nbCube<<".mat";
  //on flush le stream
  unitbuf(s);
  
  //creation du flux + creation du fichier en mode ecriture
  std::fstream fs(s.str().c_str(),std::fstream::out);

  //on ecrit la "note" de la matrice et la taille de la matrice
  fs<<eval<<" "<<inSize<<" "<<outSize<<std::endl;

  for(int i = 0 ; i < inSize ; i++)
    {
      for(int j = 0 ; j < outSize ; j++)
	{
	  fs<<weightMatrix[i][j]<<" ";
	}
      fs<<std::endl;
    }
  

  //force toutes operations que n'ont pas encore etaient faite
  fs.flush();
  //fermeture du flux
  fs.close();
  
  std::cout<<"SAVEGARDE : "<<s.str()<<std::endl;  
}


double Colony::evalMedianHeight()
{
  std::list<Cube*>::iterator itCube;
  double sumHeight = 0;
  double nbCubeStruct = 0;

  eval = 0;

  for(itCube = cubeList.begin() ; itCube != cubeList.end() ; itCube++)
    {
      //si ce n'est pas un cube de la structure
      if((*itCube)->isSubjectToEvap())
	{
	  sumHeight += (*itCube)->getCoord().y;
	  nbCubeStruct++;
	}
    }

  eval = (double)((double)sumHeight/(double)nbCubeStruct);
  
  return eval;
}
