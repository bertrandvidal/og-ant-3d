#ifndef CELL_H
#define CELL_H

#include <list>

#include "util.h"
#include "ant.h"
#include "cube.h"

/*permet d'autoriser Ant & Cube d'avoir acces a Coord*/

class Cell 
{
 private:

  Coord coord;

  Cube * cube;

 public: 

  std::list<Ant*> ants();

  Cell(Coord c)
    {
      coord.copy(c);
      cube = NULL;
    };
  
  Cell(const Cell &c)
    {
      coord.copy(c.coord);
      cube = c.cube;
    };

  Cell(int i, int j, int k)
    {
      coord.x = i;
      coord.y = j;
      coord.z = k;
      cube = NULL;
    };

  Cell(){};

  Cell& operator=(const Cell& c)
    {
      coord.copy(c.coord);
      cube = c.cube;
      return *this;
    };

  Cube* getCube(){return cube;};
  void setCube(Cube *c){cube = c;};
 
  Coord getCoord(){return coord;};
  void setCoord(Coord c){coord = c;};

  ~Cell(){};

}; 

#endif 

