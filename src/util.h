#ifndef UTIL_H
#define UTIL_H

#include <math.h>
#include <iostream>
#include <sstream>
#include <stdarg.h>

/*taille de l'environnement ie nb de cubes par cote 
 *il est preferable de mettre un nombre pair pour eviter 
 *des pbs d'arrondis
 */
const static int SIZE_ENV = 100;

/*taille d'un cube*/
const static int CUBE_SIZE = 2;

/*enum sur le type de population de fourmi
  ALPHA_VOISIN : les fourmis tiennent plus compte du voisinage que de la position (alpha=0.1)
  ALPHA_POS : les fourmis tiennent plus compte de la position que du voisinage (alpha=0.9)
  ALPHA_HALF : une moitie de la population favorise le voisinage et l'autre la position quand vient la question des phero des news cubes
  ALPHA_RANDOM : le alpha des fourmis est aleatoire (la population possede un alpha alea)
  ALPHA_RANDOM_ALL : le alpha des fourmis est aleatoire (chaque fourmi possede un alpha different)
 */
enum antType {ALPHA_VOISIN=0,ALPHA_POS,ALPHA_HALF,ALPHA_RANDOM,ALPHA_RANDOM_ALL};

/*
 *classe utilitaire permettant de materialiser 
 *les coordonnees des cubes et la position
 *des fourmis
 */
class Coord 
{
 public:
  int x;
  int y; 
  int z;
  
  double norme;

  Coord()
    {
      x = y = z = 0 ;
      norme = 0.0;
    };

 Coord(int i, int j ,int k) : x(i), y(j), z(k)
    {
      if(x < 0) x = 0 ;
      if(y < 0) y = 0 ;
      if(z < 0) z = 0 ;

      if(x >= SIZE_ENV) x = SIZE_ENV -1 ;
      if(y >= SIZE_ENV) y = SIZE_ENV -1 ;
      if(z >= SIZE_ENV) z = SIZE_ENV -1 ;
	
      norme = sqrt(pow(x,2) + pow(y,2) + pow(z,2) );
    };

  Coord(const Coord &c)
    {
      x = c.x;
      y = c.y;
      z = c.z;
      
      norme = sqrt(pow(x,2) + pow(y,2) + pow(z,2) );
    };
  
  Coord copy(const Coord &c)
  {
    Coord cp(c.x, c.y,c.z);
    cp.norme = c.norme;
    return cp;
  };

  /*
   *Calcul de la distance euclidienne entre l'instance courante et la coordonnee en parametre
   */
  double distance(Coord c)
  {
    return sqrt(pow(x-c.x,2)+pow(y-c.y,2)+pow(z-c.z,2));
  };
  
  
  /*
   *Calcul la distance entre l'instance courante de coord et celle passee en param
   *Dans cette methode on normalise les coord par la + grande norme et on calcul 
   *la distance euclidienne.
   *si attirance == true => + on est proche de c plus la valeur de retour est grande => return 1-dist => attirance pour la coord c
   *si attirance == false => repose par c => return dist
   * /!\ on ne peut pas passer la coord 0,0,0 sinon dist = 0 /!\
   */  
  double influenceDistance(Coord c, bool attirance = true)
  {
     double dist = 0;
     double biggestNorme = norme;
      
     //recupere la plus grande norme
     if(c.norme > biggestNorme)
     {
      biggestNorme = c.norme;           
     }
         
     //normalisation des coord
     double vect1[3] = {(double)((double)x/(double)biggestNorme),(double)((double)y/(double)biggestNorme),(double)((double)z/(double)biggestNorme)};
     double vect2[3] = {(double)((double)c.x/(double)biggestNorme),(double)((double)c.y/(double)biggestNorme),(double)((double)c.z/(double)biggestNorme)};
     
     //calcul de la distance
     for(int i = 0 ; i < 3 ; i++)
     {
      dist += pow(vect1[i]-vect2[i],2);        
     }
     
     dist = sqrt(dist);
     
     //attirance ou r�pulsation par c
     if(attirance == true)
     {
      dist = 1 - dist;
     }
     
     return dist;
  };

  friend std::ostream& operator<<(std::ostream& os, const Coord &c){
    os<<"["<<c.x<<","<<c.y<<","<<c.z<<"]";
    return os;
  };

  friend bool operator==(const Coord &c1,const Coord &c2)
  {return (c1.x==c2.x) && (c1.y==c2.y) && (c1.z==c2.z);};

  ~Coord(){};
};

#endif

