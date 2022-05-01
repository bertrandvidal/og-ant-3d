#ifndef CUBE_H
#define CUBE_H

#include <GL/gl.h>
#include <iostream>

#include "util.h"
class Cube
{

 private:
  
  float color[3];

  Coord pos;
  
  double pheroMove;

  double pheroConst;

  bool subjectToEvap;

 public : 
 
  static double evapMove;
  static double evapConst;

  Cube(){};

  Cube(float r, float g, float b, Coord c);
  
  Cube(float r, float g, float b, Coord c, double pMove,double pConst);

  Cube(float r,Coord c, double pMove,double pConst);
  
  Cube(float r,Coord c, double pMove,double pConst,bool evap);

  Cube(float r, float g, float b, Coord c, double pMove,double pConst,bool evap);

  Cube(const Cube &c);

  GLfloat getColor(int i){return color[i];};
  
  Coord getCoord(){return pos;};
  
  double getPheroMove(){return pheroMove;};
  
  void setPheroMove(double p)
  {
    if(p < 0)
      {
	pheroMove = 0;
      }
    else pheroMove = p;
  };

  double getPheroConst(){return pheroConst;};
  
  void setPheroConst(double p)
  {
    if(p < 0)
      {
	pheroConst = 0;
      }
    else pheroConst = p;
  };

  void setColor(GLfloat c[3]);

  bool isSubjectToEvap(){return subjectToEvap;};

  void setSujectToEvap(bool b){subjectToEvap=b;};

  friend bool operator==(const Cube &c1,const Cube &c2)
  {
    return (c1.pos==c2.pos);
  };


  ~Cube();
}; 

extern Cube* env[SIZE_ENV][SIZE_ENV][SIZE_ENV];

#endif


