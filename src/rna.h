#ifndef RNA_H
#define RNA_H

#include <math.h>

#include <iostream>
#include <vector>
#include <list>

/*
 *Cette classe simule un RNA, elle est designer selon le pattern Singleton
 *Son seul but est d'evaluer un vecteur donne aux fonctions (en utilisant la matrice de poids de la classe 
 *ou celle passee en parametre)
 *Avant de recuperer une instance (via getInstance()) il faut setter les variables nbIn et nbOut
 *qui correspondent aux nombres de neurones en entree et en sortie.
 *La matrice de poids de la classe est initialisee aleatoirement avec des poids € [0,1]
 */
class SimpleRNA
{
 private:
  //pointeur que l'on renvoie lors de l'appel de getInstance()
  static SimpleRNA* rna;
 protected :         
  //nombre de neurones en entree et en sortie
  static int nbIn;
  static int nbOut;
 
  double firstAction;
  double secondAction;
    
  //vecteur de valeur d'entree et de sortie
  std::vector<double> inValue;
  std::vector<double> outValue;   

  //matrice de poids
  double **weight;   
              
  //initialisation aleatoire du RNA
  SimpleRNA()
    {
      //RAZ des vecteurs d'entree/sortie + resize
      inValue.resize(nbIn , 0.0) ;
      outValue.resize(nbOut , 0.0);
         
      //creation + init aleatoire de la matrice des poids [0,1]
      weight = new double*[inValue.size()];
          
      for(unsigned int i = 0 ; i < inValue.size() ; i++)
	{
          weight[i] = new double[inValue.size()];
          
          for(unsigned int j = 0 ; j < outValue.size() ; j++)
	    {
	      //weight[i][j] = -1 + (float)(((double)rand() / ((double)RAND_MAX + 1)) * 2);        
	      // les valeurs sont tirees entre 0 et 1 pour simplifier le tirage par roulette
	      weight[i][j] = (double)(((double)rand() / ((double)RAND_MAX + 1)));      
	    }
	}
      
      //initialisation de l'histogramme
      histogramme.push_back(0.5);
      for(int i = 1 ; i < 27 ; i++)
	{
	  histogramme.push_back(histogramme[i-1]+ ((double)1/(double)(pow(2,(i+1)))));
	} 
    };   
 
 public :
      
  std::vector<double> histogramme;
      
  /*
   *permet de recuperer le pointeur pour acceder aux methodes
   */
  static SimpleRNA* getInstance()
  {
    if(rna == NULL)
      {
	rna = new SimpleRNA();
      }        
         
    return rna;  
  }
        
  /*
   *evaluation du vecteur d'entree
   */
  std::vector<double> evaluate(std::vector<double> inVal)
    {
      //copie des valeurs d'entree
      inValue.assign(inVal.begin(),inVal.end());
      //RAZ des valeurs de sortie
      outValue.assign(outValue.size(), 0.0);
         
      //calcul des valeurs de sortie
      for(unsigned int i = 0 ; i < inValue.size() ; i++)
	{
          for(unsigned int j = 0 ; j < outValue.size() ; j++)
	    {
	      outValue[j] += inValue[i] * weight[i][j];
	    }
	}
         
      return outValue;
    }
        
  /*
   *evaluation du vecteur d'entree en specifiant la matrice de poids
   */        
  std::vector<double> evaluate(std::vector<double> inVal, double **w)
    {
      //copie des valeurs d'entree
      inValue.assign(inVal.begin(),inVal.end());
      //RAZ des valeurs de sortie
      outValue.assign(outValue.size(), 0.0);
         
      //calcul des valeurs de sortie
      for(unsigned int i = 0 ; i < inValue.size() ; i++)
	{
          for(unsigned int j = 0 ; j < outValue.size() ; j++)
	    {
	      outValue[j] += inValue[i] * w[i][j];
	    }
	}
         
      return outValue;
    }       
    
  /*
   *on evalue et on somme les valeurs (juste les 27 dernieres)
   *facilite le tirage par roulette (on ne fait qu'une fois le parcours pour eval + sum)
   *on fera donc appel a cette fonction et ensuite on recuperera le vecteur de sortie via
   *getOutValues()
   */
  double evaluateAntGetSum(std::vector<double> inVal)
  {
    double sum = 0;

    //copie des valeurs d'entree
    inValue.assign(inVal.begin(),inVal.end());
    //RAZ des valeurs de sortie
    outValue.assign(outValue.size(), 0.0);
         
    //calcul des valeurs de sortie
    for(unsigned int i = 0 ; i < inValue.size() ; i++)
      {
	for(unsigned int j = 0 ; j < outValue.size() ; j++)
	  {
	    outValue[j] += inValue[i] * weight[i][j];
	    //pour le dernier passage on calcul la somme
	    if((i == inValue.size()-1) && (j > 1))
	      {
		//std::cout<<"SUM :"<<sum<<" + "<<outValue[j];
		sum += outValue[j];
		//std::cout<<"="<<sum<<std::endl;
	      }
	  }
      }
    
    //std::cout<<"SUM RNA :"<<sum<<std::endl;
    return sum;
  }
 
        
  /*
   *retourne la plus grande valeur de sortie en specifiant la matrice de poids
   */
  double evaluateMax(std::vector<double> inVal, double **w)
  {
    double maxOutVal;
          
    evaluate(inVal,w);
          
    maxOutVal = outValue[0];
          
    for(unsigned int i = 1 ; i < outValue.size() ; i++)
      {
	if(outValue[i] >= maxOutVal)
	  {
            maxOutVal = outValue[i];
	  }        
      }
          
    return maxOutVal;       
  }
        
  /*
   *retourne l'indice de la + grande valeur de sortie en specifiant la matrice de poids
   */
  int evaluateMaxIndex(std::vector<double> inVal, double **w)
  {
    double maxOutVal;
    int idx = 0;
         
    evaluate(inVal,w);
          
    maxOutVal = outValue[0];
          
    for(unsigned int i = 1 ; i < outValue.size() ; i++)
      {
	if(outValue[i] >= maxOutVal)
	  {
            maxOutVal = outValue[i];
            idx = i;
	  }        
      }
          
    return idx;
  }        
  /*
   *retourne la plus grande valeur de sortie
   */
  double evaluateMax(std::vector<double> inVal)
  {
    double maxOutVal;
          
    evaluate(inVal);
          
    maxOutVal = outValue[0];
          
    for(unsigned int i = 1 ; i < outValue.size() ; i++)
      {
	if(outValue[i] >= maxOutVal)
	  {
            maxOutVal = outValue[i];
	  }        
      }
          
    return maxOutVal;
  }

  /*
   *retourne l'indice de la + grande valeur de sortie
   */
  int evaluateMaxIndex(std::vector<double> inVal)
  {
    double maxOutVal;
    int idx = 0;
         
    evaluate(inVal);
          
    maxOutVal = outValue[0];
          
    for(unsigned int i = 1 ; i < outValue.size() ; i++)
      {
	if(outValue[i] >= maxOutVal)
	  {
            maxOutVal = outValue[i];
            idx = i;
	  }        
      }
          
    return idx;
  }

  double getFirstAction() 
  {
    firstAction = outValue[0];
    return firstAction;
  };

  double getSecondAction() 
  {
    secondAction = outValue[1];
    return secondAction;
  };

  /*
   *retourne la liste des valeurs de sortie triee par ordre decroisant en supprimant les deux premieres
   *cette methode ne doit etre appelee qu'une seul fois par evaluation
   */
  std::list<double> getOutValueSorted()
    {
      std::list<double> outValueSorted(outValue.begin(),outValue.end());
      
      //on supprime les deux premieres valeurs
      outValueSorted.pop_front();
      outValueSorted.pop_front();

      outValueSorted.sort();
      outValueSorted.reverse();

      return outValueSorted;
    }
        
  /*
   *recuperation des valeurs de sortie
   */       
  std::vector<double> getOutValues()
    {
      return outValue;                  
    }
        
  /*
   *destructeur de la class
   */
  ~SimpleRNA()
    {
      delete rna;
      for(unsigned int i = 0 ; i < inValue.size() ; i++)
	{
	  delete weight[i];
	}
          
      delete [] weight;
    };
       
  double** getWeightMatrix()
  {
    return weight;
  }
 
  /*
   *set le nombre de neurones en entree
   */
  static void setNbIn(int in)
  {
    nbIn = in;       
  }

  /*
   *set le nombre de neurones en sortie
   */        
  static void setNbOut(int out)
  {
    nbOut = out;       
  }

  /*
   *get le nombre de neurones en sortie
   */        
  static int getNbOut()
  {
    return nbOut;       
  }

  /*
   *get le nombre de neurones en sortie
   */        
  static int getNbIn()
  {
    return nbIn;       
  }

  static void resetRNA()
  {
    rna = NULL;
  }

  void setMatrix(double **w)
  {
    memcpy(weight,w,nbIn*nbOut*sizeof(double));
  };
};


#endif
