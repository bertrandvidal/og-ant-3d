#ifndef ANT_H
#define ANT_H

#include <GL/gl.h>
#include <iostream>
#include <list>
#include <vector>
#include <math.h>
#include <climits>

#include "util.h"
#include "cube.h"
#include "rna.h"

extern Cube* env[SIZE_ENV][SIZE_ENV][SIZE_ENV];
extern std::list<Cube*> cubeList;

class Ant
{
 private:
  
  GLfloat color[3];
 
  Coord pos;
  
  double alpha;
  
  double alphaPos;
  double betaPos;
  double gammaPos;
  
  static SimpleRNA* rna;
  
 public :

  static Coord target;

  Ant();
  
  Ant(float r, float g, float b, Coord c);
 
  Ant(float r, float g, float b, Coord c, double alpha);

  Ant(const Ant &a);
  
  GLfloat getColor(int i){return color[i];};
   
  double getAlpha(){return alpha;};
  
  void setAlpha(double a){alpha = a;};
  
  Coord getCoord(){return pos;};

  void setCoord(Coord c){pos=c;};

  void setColor(GLfloat c[3]);

  void action();
  
  void actionRNA();

  void actionSimpleRNA();
  
  void move();

  std::vector<double> initInValue();

  void moveToPosition(Coord);
  
  bool construct();

  bool constructToPosition(Coord);

  std::list<Coord> getPosiblePosition();

  bool gotNeighbor(int, int, int);

  void getNeighborInfo(Coord , double* , double*);

  std::list<Coord> getBestPositions(std::list<Coord> ,float *);
  
  std::list<Coord> getBestBuild(std::list<Coord> ,float *);

  Coord setRandomPosition(std::list<Coord>);

  static SimpleRNA* getRNA();

  Coord getCoordByIndex(int);

  int idxByRouletteOnIdx(std::vector<double>);

  int idxByRouletteOnValue(std::vector<double>);
  
  static void resetRNA()
  {
    rna = NULL;
  };

  ~Ant(){};

};

#endif


