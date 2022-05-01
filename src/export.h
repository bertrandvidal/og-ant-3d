#ifndef EXPORT_H
#define EXPORT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <string>

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <float.h>
#include <algorithm>

#include "cube.h"
#include "ant.h"

extern std::list<Cube*> cubeList;

class Point {
 public :
  float x;
  float y;
  float z;
  Point(){};
 Point(float a,float b,float c) : x(a),y(b),z(c){};
  bool operator==(const Point &p)
  {
    return ((x==p.x) && (y==p.y) && (z==p.z));     
  };
      
  friend std::ostream& operator<<(std::ostream& os, const Point &c){
    os<<"["<<c.x<<","<<c.y<<","<<c.z<<"]";
    return os;
  };
};

class Face {
 public : 
  int pt1; 
  int pt2; 
  int pt3;
  
  Face(){};
  
 Face(int a,int b,int c):pt1(a),pt2(b),pt3(c){};
  
  bool contains(int idx)
  {
    return ( (pt1==idx) || (pt2==idx) || (pt3==idx));
  };
};

/*bresenham 3D*/
void drawLine3D(Coord c, Coord c2);

/*trace le cercle circonscrit / renvoi le derniere cube construit*/
Coord drawArc3D(Coord c1, int radius,int axe);

/*fonction d'export des sculptures au format ant*/
void sculpToAntFile(int,int);

/*fonction d'import des sculptures au format ant*/
bool antFileToSculp(std::string);

/*fonction d'import des sculptures au format obj*/
bool objFileToSculp(std::string, bool);

#endif
