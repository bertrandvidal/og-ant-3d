#include "export.h"


/*fonction d'import des sculptures*/
bool antFileToSculp(std::string s)
{
  std::FILE *pFile = fopen(s.c_str(),"r");
  int sizeEnv = 0;
  int nbCube = 0;
  float color = 0.0;
  int x, y , z;
  float pMove, pConst;
  int evap;
  Coord last(-1,-1,-1);
  bool retVal = false;
  
  //on nettoie la liste des cubes
  cubeList.clear();
  
  if(pFile != NULL)
    {
      //on lit la taille de l'env et le nb de cube
      fscanf(pFile,"%d",&sizeEnv);
      fscanf(pFile,"%d",&nbCube);

      if(sizeEnv <= SIZE_ENV)
	{
	  for(int i = 0 ; i < nbCube ; i++)
	    {
	      // raz des var
	      color = 0.0;
	      x=y=z=0;
	      pMove=pConst=0.0;
	      evap=false;
	      //lecture des var du cube
	      fscanf(pFile,"%f",&color);
	      fscanf(pFile,"%d",&x);
	      fscanf(pFile,"%d",&y);
	      fscanf(pFile,"%d",&z);
	      fscanf(pFile,"%f",&pMove);
	      fscanf(pFile,"%f",&pConst);
	      fscanf(pFile,"%d",&evap);
	  
	      //on elimine les doublons
	      Coord c(x,y,z);
	  
	      if(!(c == last))
		{
		  //instanciation du cube & ajout a la liste
		  Cube *cube = new Cube(color,color,color,c,pMove,pConst,evap);
		  cubeList.push_back(cube); 
		  last.x = c.x;
		  last.y = c.y;
		  last.z = c.z;
		}
	    }
	  retVal = true;
	  //std::cout<<"LOADING : "<<cubeList.size()<<" cubes!!!"<<std::endl;
	}
      else 
	{
	  std::cout<<"Impossible d'utiliser ce fichier SIZE_ENV trop grand..."<<std::endl;  
	  retVal = false;
	}
      //fermeture du flux
      fclose(pFile);  
    }
  else 
    {
      std::cout<<"Erreur ouverture fichier "<<s<<"..."<<std::endl;
      retVal = false;
    }

  return retVal;
}

void sculpToAntFile(int antType, int nbAnt)
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
  s<<"export_"<<timeinfo->tm_mday<<"_"<<timeinfo->tm_mon+1<<"_";
  //heure de l'enregistrement
  s<<timeinfo->tm_hour<<"_"<<timeinfo->tm_min<<"_"<<timeinfo->tm_sec<<"_";
  //le type alpha et nb ant
  s<<antType<<"_"<<nbAnt<<".ant";
  //on flush le stream
  unitbuf(s);
  
  //creation du flux + creation du fichier en mode ecriture
  std::fstream fs(s.str().c_str(),std::fstream::out);
  std::list<Cube*>::iterator itCube;

  //ecriture de taille env et nb cube
  fs<<SIZE_ENV<<" "<<cubeList.size()<<std::endl;

  //pour chaque cube on ecrit ses caracts
  for(itCube = cubeList.begin(); itCube != cubeList.end(); itCube++)
    {
      Cube* cube = *itCube;
      Coord c = cube->getCoord();
      fs<<cube->getColor(0)<<" "<<c.x<<" "<<c.y<<" "<<c.z<<" "<<cube->getPheroMove()<<" "<<cube->getPheroConst()<<" "<<cube->isSubjectToEvap()<<std::endl;
    }

  //force toutes operations que n'ont pas encore etaient faite
  fs.flush();
  //fermeture du flux
  fs.close();
  
  std::cout<<"SAVEGARDE : "<<s.str()<<std::endl;
}


bool objFileToSculp(std::string s,bool filaire)
{
  //mins et max sur chaque axe
  float minX = FLT_MAX;
  float maxX = FLT_MIN;
  float minY = FLT_MAX;
  float maxY = FLT_MIN;
  float minZ = FLT_MAX;
  float maxZ = FLT_MIN;
  //ecart entre min et max sur chaque axe + le max des ecart => garde l'aspect de l'obj
  float deltaX, deltaY,deltaZ,deltaMax;
  //taille de l'environnement
  int se = SIZE_ENV;
  //va contenir chaque ligne que l'on lit pour traitement
  char line[256];
  //corresond aux mins et max mais dans le systeme de coord de l'env
  int newMinZ,newMaxZ,newMinX,newMaxX;
  //charactere de debut de ligne du .obj pour differencier points et faces
  char c = 'a';
  //valeur de retour
  bool retVal = false;

  //on nettoie la liste des cubes
  cubeList.clear();
    
  //representation du fichier
  FILE *pFile = fopen(s.c_str(),"r");
  //contiendra tous les points de l'obj
  std::vector<Point> vectPoint;
  //iterator sur le vect de points
  std::vector<Point>::iterator itPt;
  //contiendra toutes les faces de l'obj
  std::list<Face> listFace;
  std::list<Face>::iterator itFace;
  //contient la ligne lue => permet les traitements simplifies
  std::stringstream sstream;
    
  if(pFile != NULL)
    {
      while(!feof(pFile))
	{
	  //on recupere la premier ligne
	  fgets(line,255,pFile);
	  sstream<<line;
       
	  //on lit le premier caractere
	  sstream>>c;
       
	  //si c'est un point
	  if(c == 'v')
	    {
	      //on cree le point
	      Point p;
	      sstream>>p.x;
	      sstream>>p.y;
	      sstream>>p.z;
        
	      //on conserve les min/max sur chaque axe pour la transposition dans le nouveau repere
	      if(p.x<minX)minX = p.x;
	      if(p.x>maxX)maxX = p.x;

	      if(p.y<minY)minY = p.y;
	      if(p.y>maxY)maxY = p.y;
        
	      if(p.z<minZ)minZ = p.z;
	      if(p.z>maxZ)maxZ = p.z;        
        
	      //on stock le point
	      vectPoint.push_back(p);
	    }   
	  //si c'est une face
	  else if(c == 'f')
	    {
	      //on recupere les 3 indices des points qui forment la face
	      Face f;        
	      sstream>>f.pt1;
	      sstream>>f.pt2;
	      sstream>>f.pt3;
        
	      //on stocke la face
	      listFace.push_back(f);
	    }     
	}
      
      //calculs pour gains de temps (pour ne pas recalculer a chaque boucle)
      deltaX = maxX-minX;
      deltaY = maxY-minY;
      deltaZ = maxZ-minZ;
      
      //permet de garder l'echelle de l'obj
      deltaMax = deltaX;
      if(deltaMax < deltaY)deltaMax = deltaY;
      if(deltaMax < deltaZ)deltaMax = deltaZ;
       
      //permet de centrer l'obj dans l'env (pas sur Y pour garder contact au sol)
      newMinZ = (int)((se-1) * (1 - ((maxZ-minZ) / deltaZ)) * (deltaZ/deltaMax));
      newMaxZ = (int)((se-1) * (deltaZ/deltaMax));
      newMinX = (int)((se-1) * (1 - ((maxX-minX) / deltaX)) * (deltaX/deltaMax));
      newMaxX = (int)((se-1) * (deltaX/deltaMax));
      
      //on ajoute les sommets dans le cas ou le .obj n'est pas d'info sur les faces
      for(itPt = vectPoint.begin() ; itPt != vectPoint.end() ; itPt++)
	{
	  //on transforme le point pour qu'il soit representable dans notre env (centrer en X et Z)
	  (*itPt).x = (int)((se-1) * (1 - ((maxX-(*itPt).x) / deltaX)) * (deltaX/deltaMax)+ abs((se-newMaxX)/2-newMinX));
	  (*itPt).y = (int)((se-1) * (1 - ((maxY-(*itPt).y) / deltaY)) * (deltaY/deltaMax));
	  (*itPt).z = (int)((se-1) * (1 - ((maxZ-(*itPt).z) / deltaZ)) * (deltaZ/deltaMax) + abs((se-newMaxZ)/2-newMinZ));

	  //ajout du cube dans la liste des cubes a afficher
	  Coord cd((int)(*itPt).x,(int)(*itPt).y,(int)(*itPt).z);
	  double dist = cd.influenceDistance(Ant::target);
	  Cube *c = new Cube(0.5,cd,dist,dist,false);        
	  cubeList.push_back(c);  
	}

      // si on veut tracer en mode filaire
      if(filaire == true)
	{
	  //tracage des faces en mode filaire
	  for(itFace = listFace.begin() ; itFace != listFace.end() ; itFace++)
	    {
	      //on recupere les 3 sommets de la face (indice commence a 1)
	      Point p1 = vectPoint.at((*itFace).pt1-1);
	      Point p2 = vectPoint.at((*itFace).pt2-1);
	      Point p3 = vectPoint.at((*itFace).pt3-1);
	 
	      //trace les 3 droites du triangle
	      drawLine3D(Coord((int)p1.x,(int)p1.y,(int)p1.z),Coord( (int)p2.x, (int)p2.y, (int)p2.z));
	      drawLine3D(Coord((int)p2.x,(int)p2.y,(int)p2.z),Coord( (int)p3.x, (int)p3.y, (int)p3.z));
	      drawLine3D(Coord((int)p3.x,(int)p3.y,(int)p3.z),Coord( (int)p1.x, (int)p1.y, (int)p1.z));
	    }
	}

      std::cout<<"LOADING : "<<cubeList.size()<<" cubes!!!"<<std::endl;
      retVal = true;
      //fermeture du fichier   
      fclose(pFile);
    }
  else 
    {
      std::cout<<"Erreur ouverture fichier "<<s<<" ..."<<std::endl;
      retVal = false;
    }

  return retVal;
}


/*
 *Bresenham 3D
 */
void drawLine3D(Coord c1, Coord c2)
{
  int dx = c2.x - c1.x;
  int dy = c2.y - c1.y;
  int dz = c2.z - c1.z;
  
  int x_inc,y_inc,z_inc;
  int l,m,n;
  double err_1;
  double err_2;

  if(dx < 0) x_inc = -1; 
  else x_inc=1;

  l = abs(dx);

  if(dy < 0) y_inc = -1 ;
  else y_inc=1;
   
  m = abs(dy);
    
  if(dz < 0) z_inc = -1 ;
  else z_inc=1;
    
  n = abs(dz); 
  
  double dx2 = (2*l);
  double dy2 = (2*m);
  double dz2 = (2*n);


  if ((l >= m) && (l >= n)) 
    {
      err_1 = dy2 - l;
      err_2 = dz2 - l;
      for(int i = 0 ; i < l ; i++) 
	{
	  Coord cd(c1.x,c1.y,c1.z);
	  double dist = cd.influenceDistance(Ant::target);
	  Cube* c = new Cube(0.5,cd,dist,dist,false);
	  cubeList.push_back(c);

	  if(err_1 > 0) 
	    {
	      c1.y += y_inc;
	      err_1 -= dx2;
	    } 

	  if (err_2 > 0) 
	    {
	      c1.z += z_inc;
	      err_2 -= dx2;
	    } 
	  
	  err_1 = err_1 + dy2;
	  err_2 = err_2 + dz2;
	  
	  c1.x += x_inc;
	}
    }
  else if((m >= l) && (m >= n))
    {
      err_1 = dx2 - m;
      err_2 = dz2 - m;
      for(int i = 0 ; i < m ; i++) 
	{
	  Coord cd(c1.x,c1.y,c1.z);
	  double dist = cd.influenceDistance(Ant::target);
	  Cube* c = new Cube(0.5,cd,dist,dist,false);
	  cubeList.push_back(c); 

	  if(err_1 > 0) 
	    {
	      c1.x += x_inc;
	      err_1 -= dy2;
	    }
            
	  if(err_2 > 0) 
	    {
	      c1.z += z_inc;
	      err_2 -= dy2;
	    }
	    
	   err_1 = err_1 + dx2;
	   err_2 = err_2 + dz2;
	   c1.y = c1.y + y_inc;
	}
    }
  else 
    {
      err_1 = dy2 - n;
      err_2 = dx2 - n;
      
      for(int i = 0 ; i < n ; i++) 
	{
	  Coord cd(c1.x,c1.y,c1.z);
	  double dist = cd.influenceDistance(Ant::target);
	  Cube* c = new Cube(0.5,cd,dist,dist,false);
	  cubeList.push_back(c); 

	  if(err_1 > 0) 
	    {
	      c1.y += y_inc;
	      err_1 -= dz2;
	    }
            
	  if(err_2 > 0) 
	    {
	      c1.x += x_inc;
	      err_2 -= dz2;
	    }

	  err_1 = err_1 + dy2;
	  err_2 = err_2 + dx2;
	  c1.z = c1.z + z_inc;  
	}
    }

}

Coord drawArc3D(Coord c, int radius,int axe)
{
  Coord cd(c);
  Coord end(c);
  int x = 0;
  int y = radius;
  double d = 3 - 2 * radius;

  if(axe == 0)
    {
      cd.x += radius;
      end.x += 2 * radius;
    }
  else if(axe == 1)
    {
      cd.y += radius;
      end.y += 2 * radius;
    }
  else 
    {
      cd.z += radius;
      end.z += 2 * radius; 
    }

  while( y > x)  
    {
      if (d<0)
	{
	  d += 4 * x + 6 ;
	}
      else
	{
	  d += 4 * (x - y) + 10 ;
	  y-- ;
	}

      x++ ;      

      if(axe == 0)
	{
	  Coord cd1(cd.x + x, cd.y + y, cd.z);
	  double dist1 = cd1.influenceDistance(Ant::target);
	  Cube* c1 = new Cube(0.5,cd1,dist1,dist1,false);
	  cubeList.push_back(c1); 

	  Coord cd2(cd.x - x, cd.y + y, cd.z);
	  double dist2 = cd2.influenceDistance(Ant::target);
	  Cube* c2 = new Cube(0.5,cd2,dist2,dist2,false);
	  cubeList.push_back(c2); 

	  Coord cd3(cd.x + y ,cd.y + x ,cd.z);
	  double dist3 = cd3.influenceDistance(Ant::target);
	  Cube* c3 = new Cube(0.5,cd3,dist3,dist3,false);
	  cubeList.push_back(c3); 

	  Coord cd4(cd.x - y ,cd.y + x , cd.z);
	  double dist4 = cd4.influenceDistance(Ant::target);
	  Cube* c4 = new Cube(0.5,cd4,dist4,dist4,false);
	  cubeList.push_back(c4);
	}
      else if(axe == 1)
	{
	  Coord cd1(cd.x , cd.y + x, cd.z + y);
	  double dist1 = cd1.influenceDistance(Ant::target);
	  Cube* c1 = new Cube(0.5,cd1,dist1,dist1,false);
	  cubeList.push_back(c1); 
	 
	  Coord cd2(cd.x, cd.y - x, cd.z + y);
	  double dist2 = cd2.influenceDistance(Ant::target);
	  Cube* c2 = new Cube(0.5,cd2,dist2,dist2,false);
	  cubeList.push_back(c2); 

	  Coord cd3(cd.x,cd.y + y,cd.z + x);
	  double dist3 = cd3.influenceDistance(Ant::target);
	  Cube* c3 = new Cube(0.5,cd3,dist3,dist3,false);
	  cubeList.push_back(c3); 

	  Coord cd4(cd.x,cd.y - y, cd.z + x);
	  double dist4 = cd4.influenceDistance(Ant::target);
	  Cube* c4 = new Cube(0.5,cd4,dist4,dist4,false);
	  cubeList.push_back(c4);
	}
      else 
	{
	  Coord cd1(cd.x + y, cd.y, cd.z + x);
	  double dist1 = cd1.influenceDistance(Ant::target);
	  Cube* c1 = new Cube(0.5,cd1,dist1,dist1,false);
	  cubeList.push_back(c1); 

	  Coord cd2(cd.x + y, cd.y, cd.z - x);
	  double dist2 = cd2.influenceDistance(Ant::target);
	  Cube* c2 = new Cube(0.5,cd2,dist2,dist2,false);
	  cubeList.push_back(c2); 

	  Coord cd3(cd.x + x,cd.y,cd.z + y);
	  double dist3 = cd3.influenceDistance(Ant::target);
	  Cube* c3 = new Cube(0.5,cd3,dist3,dist3,false);
	  cubeList.push_back(c3); 

	  Coord cd4(cd.x + x,cd.y, cd.z - y);
	  double dist4 = cd4.influenceDistance(Ant::target);
	  Cube* c4 = new Cube(0.5,cd4,dist4,dist4,false);
	  cubeList.push_back(c4);
	}
     

    }

  return end;
}










