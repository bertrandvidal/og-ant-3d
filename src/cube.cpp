#include "cube.h"


double Cube::evapMove = 0.003;
double Cube::evapConst = 0.003;


Cube::Cube(float r, float g, float b, Coord c)
{
  pos = c;
  env[pos.x][pos.y][pos.z] = this;

  pheroMove = 0.01;
  pheroConst = 0.01;
  subjectToEvap = true;

  color[0] = r;
  color[1] = g;
  color[2] = b;
};

Cube::Cube(float r,Coord c, double pMove,double pConst)
{
  pos = c;
  env[pos.x][pos.y][pos.z] = this;
  
  setPheroMove(pMove);
  setPheroConst(pConst);
  subjectToEvap = true;

  color[0] = r;
  color[1] = r;
  color[2] = r;
}

Cube::Cube(float r,Coord c, double pMove,double pConst,bool evap)
{
  pos = c;
  env[pos.x][pos.y][pos.z] = this;
  
  setPheroMove(pMove);
  setPheroConst(pConst);
  subjectToEvap = evap;

  color[0] = r;
  color[1] = r;
  color[2] = r;         
}

Cube::Cube(float r, float g, float b, Coord c, double pMove, double pConst)
{
  pos = c;
  env[pos.x][pos.y][pos.z] = this;
  
  setPheroMove(pMove);
  setPheroConst(pConst);
  subjectToEvap = true;

  color[0] = r;
  color[1] = g;
  color[2] = b;               
};


Cube::Cube(float r, float g, float b, Coord c, double pMove,double pConst,bool evap)
{
  pos = c;
  env[pos.x][pos.y][pos.z] = this;
  
  setPheroMove(pMove);
  setPheroConst(pConst);
  subjectToEvap = evap;

  color[0] = r;
  color[1] = g;
  color[2] = b;  
}

Cube::Cube(const Cube &c)
{
  pos = c.pos;
  env[pos.x][pos.y][pos.z] = this;

  setPheroMove(c.pheroMove);
  setPheroConst(c.pheroConst);
  subjectToEvap = true;

  for(int i = 0 ; i < 3 ; i++)
    {
      color[i] = c.color[i];
    }
};

Cube::~Cube()
{ 
  env[pos.x][pos.y][pos.z] = NULL;
};

void Cube::setColor(GLfloat c[3])
{
  color[0] = c[0];
  color[1] = c[1];
  color[2] = c[2];
};
