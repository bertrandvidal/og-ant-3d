#ifndef EVAL_H
#define EVAL_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>

#include <math.h>

#include "ant.h"
#include "rna.h"
#include "util.h"
#include "cube.h"

extern std::list<Cube*> cubeList;

/*
 *cette classe represente une colony par sa matrice et son evaluation
 *le pointeur de fontion permet d'avoir differentes methodes d'evaluation
 */
class Colony
{
 private :
  
  double eval;

  double** weightMatrix;

  int inSize;
  int outSize;

  int nbCube;
  
 public :

  Colony(){};
  Colony(int , int ,double** , int);
  Colony(const Colony &);
  
  ~Colony()
    {
      for(int i = 0 ; i < inSize ; i++)
	{
	  delete weightMatrix[i];
	}
      
      delete [] weightMatrix;
    };

  double evalMedianHeight();

  void saveEvalMatrix();

  double getEval()
  {
    return eval;
  };

  void setEval(double e)
  {
    eval = e;
  };

  double** getWeightMatrix()
  {
    return weightMatrix;
  };

  friend bool operator<(const Colony &c1, const Colony &c2)
  {
    return (c1.eval < c2.eval);
  };

  int getInSize(){return inSize;};
  int getOutSize(){return outSize;};
  int getNbCube(){return nbCube;};
};

#endif

