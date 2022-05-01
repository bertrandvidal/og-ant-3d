#ifndef EA_H
#define EA_H

#include <list>
#include <vector>
#include <string>

#include "colony.h"
#include "view.h"
#include "util.h"
#include "ant3D.h"

class EvolutionEngine
{
 private :

  static EvolutionEngine *engine;

 protected:

  static int nbParent;
  static int nbGeneration;
  static int nbCubeMax;
  
  static std::string fileName;
  
  static Colony* bestColony;

  std::list<Colony *> listColony;

  EvolutionEngine(){};

 public :
  
  static EvolutionEngine* getInstance()
  {
    if(engine == NULL)
      {
	engine = new EvolutionEngine();
      };

    return engine;
  };

  ~EvolutionEngine()
    {
      if(bestColony)
	{
	  delete bestColony;
	}
    };

  std::list<Colony*> selectColony();

  Colony* crossColony(std::list<Colony*>);

  void mutateColony(Colony *);

  void startEvolution();

  void initParent();
  
  void evaluationColony(Colony *colony, bool isBest = false);
  
  void addColony(Colony *);

  int getNbParent()
  {
    return nbParent;
  };

  static int getNbCubeMax()
  {
    return nbCubeMax;
  };

  static void setNbParent(int ev) 
  {
    nbParent = ev;
  };

  static void setNbGeneration(int gene)
  {
    nbGeneration = gene;
  };

  static void setNbCube(int cube)
  {
    nbCubeMax = cube;
  };

  static Colony* getBestCOlony()
  {
    return bestColony;
  };

  static void setFilename(std::string);
};

#endif

