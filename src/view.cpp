#include "view.h"

#define PI 3.14159265

/*gestion de la lumiere*/
const GLfloat light_ambient[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat light_diffuse[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat light_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat light_direction[] = { 0.0f, -1.0f, -1.0f};
const GLfloat light_position[] = { 0.0f, (SIZE_ENV*CUBE_SIZE)*2, (SIZE_ENV*CUBE_SIZE)*2, 0.0f };
const GLfloat light_cut_off = 45;

/*string contenant le nombre de fourmis et de cubes*/
std::string info("nbAnt = nbCube = ");

/*objet gerant les deplacements de la camera via souris + clavier*/
static camera *_cam = new camera(SIZE_ENV,CUBE_SIZE);

/*booleen start/stop*/
static bool play = true;

/*booleen pour type de construction*/
static bool typeConst = true;

/*booleen pour affichage du grand cube*/
static bool bigCube = true;

/*boolean pour afficher le texte d'info*/
static bool displayText = true;

/*booleen pour afficher les cubes en filaire*/
static bool wire = false;

/*booleen pour afficher le sol*/
static bool ground = false;

/*booleen pour la dimunution des pheromones de deplacement des cubes (true <=> evap ON)*/
static bool isEvapMoveSet = false;

/*booleen pour la dimunution des pheromones de construction des cubes (true <=> evap ON)*/
static bool isEvapConstSet = false;

/*booleen pour la representation des pheromones de deplacement*/
static bool repPheroMove = false;

/*booleen pour la representation des pheromones de construction*/
static bool repPheroConst = !repPheroMove;

/*permet le mode pas a pas*/  
static int step = 0 ;

/*type de fourmi qui seront creees*/
/*enum sur le type de population de fourmi
  ALPHA_HALF : une moitie de la population favorise le voisinage et l'autre la position quand vient la question des phero des news cubes
  ALPHA_VOISIN : les fourmis tiennent plus compte du voisinage que de la position (alpha=0.1)
  ALPHA_POS : les fourmis tiennent plus compte de la position que du voisinage (alpha=0.9)
  ALPHA_RANDOM : le alpha des fourmis est aleatoire (la population possede un alpha alea)
  ALPHA_RANDOM_ALL : le alpha des fourmis est aleatoire (chaque fourmi possede un alpha different)
 */
int createAntType = ALPHA_VOISIN;

/*type de structure qui sera construit*/
/*
  ONE_CUBE : un cube place aleatoirement
  CUBE_X : un ensemble de barres 
  CIRCLE : un cercle au milieu de l'env
  CYLINDER : un demi-cylindre couche
  TABLE : une table (4 pieds et un plateau)
  CUBE_PHERO : deux parois de + en + attirantes
  CUBE_ANT : ecrit ANT dans l'env (env de 100 only)
  RANDOM : construit une "arborescence" de 2500 cubes
 */
int createCubeType = RANDOM;


/*environnement*/
Cube* env[SIZE_ENV][SIZE_ENV][SIZE_ENV];
  
/*liste des cubes*/
std::list<Cube*> cubeList;
  
/*liste des fourmis*/
std::list<Ant*> antList;  

/*decalage pour centrer la sculpture*/
static const int shift = (int)(SIZE_ENV/2);


/*fonction d'affichage*/
void display()
{
  std::list<Cube*>::iterator itCube;
  std::list<Ant*>::iterator itAnt;

  /*on nettoie les buffers*/
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  /*on charge l'identite*/
  glLoadIdentity();

  if(displayText)
    {
      /*affichage texte*/

      glDisable( GL_DEPTH_TEST );
 
      glMatrixMode( GL_PROJECTION );
      glPushMatrix();
      glLoadIdentity();
      glColor3f(0.0,0.0,0.0);
      glOrtho( -1.0,1.0,-1.0,1.0,0.0,100.0 );
      glRasterPos2f( -1,0.95);
    
      for (unsigned int i = 0; i < info.size(); i++){ glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, info[i] ); }
    
      glPopMatrix();
      glMatrixMode( GL_MODELVIEW );
    
      glEnable( GL_DEPTH_TEST );
    }


   
  /*placement de la camera*/
  _cam->lockedAndLoaded();

  if(ground)
    {
      glPushMatrix();
      glColor4f(1.0,1.0,1.0,1.0);
      glTranslatef(-(CUBE_SIZE/2),-(SIZE_ENV +(CUBE_SIZE)),-(CUBE_SIZE/2));
      glBegin(GL_QUADS);
     
      glVertex3f( -SIZE_ENV ,0 , -SIZE_ENV);
      glVertex3f( -SIZE_ENV ,0 , SIZE_ENV);
      glVertex3f( SIZE_ENV  ,0 , SIZE_ENV);
      glVertex3f( SIZE_ENV  ,0 , -SIZE_ENV);

      glEnd();
      glPopMatrix();
    }

  /*dessin du "grand" cube vert*/
  if(bigCube)
    {
      glColor4f(0.0,1.0,0.0,1.0);
      glPushMatrix();
      glTranslatef(-CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2);
      glutWireCube(CUBE_SIZE*SIZE_ENV);
      glPopMatrix();
    }
  
  /*dessin des cubes*/
  /*parcours de la liste des cubes pour les dessiner*/
  for(itCube = cubeList.begin(); itCube != cubeList.end(); itCube++)
    {
      glPushMatrix();

      Cube* cube = *itCube;
      /*type de coloration des cubes (pheromones deplacement ou construction ou par leur couleur)*/
      if(repPheroMove)
	{
	  glColor4f(cube->getPheroMove(),0,0,1.0);
	}
      else if(repPheroConst)
	{
	  glColor4f(0,cube->getPheroConst(),0,1.0);
	}
      else
	{
	  //glColor4f(((double)cube->getCoord().x/(double)SIZE_ENV),((double)cube->getCoord().y/(double)SIZE_ENV),((double)cube->getCoord().z/(double)SIZE_ENV),1.0);
	  glColor4f(cube->getColor(0),cube->getColor(0),cube->getColor(0),1.0);
	}
      
      /*evaporation des pheromones (deplacement + construction)*/
      if(play && cube->isSubjectToEvap())
	{
	  if(isEvapMoveSet)
	    { 
	      cube->setPheroMove((1-Cube::evapMove)*cube->getPheroMove());
	    }

	  if(isEvapConstSet)
	    { 
	      cube->setPheroConst((1-Cube::evapConst)*cube->getPheroConst());
	    }
	}

      /*transformation des coordonnees pour les representer en fonction de l'env*/
      int x = ((cube->getCoord().x) - shift)*(CUBE_SIZE);
      int y = ((cube->getCoord().y) - shift)*(CUBE_SIZE);
      int z = ((cube->getCoord().z) - shift)*(CUBE_SIZE);
      
      glTranslatef(x,y,z);
      if(wire)
	{
	  if(typeConst) glutWireCube(CUBE_SIZE);
	  else glutWireSphere(CUBE_SIZE,6,6);     
	}
      else
	{
          if(typeConst) glutSolidCube(CUBE_SIZE);
          else glutSolidSphere(CUBE_SIZE,6,6);
	}     
      glPopMatrix();
    }
  
  /*dessin des fourmis*/
 
  /*parcours de la liste des fourmis pour les dessiner*/
  for(itAnt = antList.begin(); itAnt != antList.end(); itAnt++)
    {
      glPushMatrix();

      Ant* ant =*itAnt;
      glColor4f(ant->getColor(0),ant->getColor(1),ant->getColor(2),0.75);
      
      /*transformation des coordonnees pour les representer en fonction de l'env*/
      int x = ((ant->getCoord().x) - shift)*(CUBE_SIZE);
      int y = ((ant->getCoord().y) - shift)*(CUBE_SIZE);
      int z = ((ant->getCoord().z) - shift)*(CUBE_SIZE);
      
      glTranslatef(x,y,z);

      /*permet la transparence des fourmis*/
      glDisable(GL_LIGHTING);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      //glutSolidSphere(CUBE_SIZE/2,8,8);
      glutSolidCube(CUBE_SIZE);

      glDisable(GL_BLEND);
      glEnable(GL_LIGHTING);
      
      glPopMatrix();
      
      /*mode lecture*/
      if(play)
        {
	  //ant->action();
	  //ant->actionSimpleRNA();
	  ant->actionRNA();
        }
    }


  /*pour une plus grande fluidite on utilise le double buffering*/
  glutSwapBuffers();
  glFlush();
  /*mode pas a pas*/ 
  if(step != 0)
    {
      step = 0;
      for(itAnt = antList.begin(); itAnt != antList.end(); itAnt++)
	{
          Ant* ant =*itAnt;
          ant->actionRNA();
	}
    }

}

/*
 *effectue nbIte parcours de la liste des fourmis
 *qui effectueront leur action
 */
void colonieAction(unsigned int nbCube)
{
  //std::cout<<"COL ACTION [IN]"<<std::endl;
  std::list<Ant*>::iterator itAnt;
  
  while(cubeList.size() < nbCube)
    {
      /*parcours de la liste des fourmis pour les dessiner*/
      for(itAnt = antList.begin(); itAnt != antList.end(); itAnt++)
	{
	  (*itAnt)->actionRNA();
	}
    }
  //std::cout<<"COL ACTION [OUT]"<<std::endl;
}


/*creation des infos a afficher*/
void displayInfo()
{
  std::stringstream sstream;
  /*construction de la chaine d'info*/
  sstream<<"nbAnt:"<<antList.size()<<"_nbCube:"<<cubeList.size();
  /*flush le stream (ce qui est plutôt flex!!!)*/
  unitbuf(sstream);
  /*nettoye la chaine, le remplace et assure qu'elle est null terminated*/
  info.clear();
  sstream>>info;
  info+='\0'; 
}


void createAntALPHA(int nb)
{
  float r,g,b;

  for(int i = 0 ; i < nb ; i++)
    {
      Coord c((int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),0,(int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))));
      r = (float)(((double)rand() / ((double)RAND_MAX + 1)));
      g = (float)(((double)rand() / ((double)RAND_MAX + 1)));
      b = (float)(((double)rand() / ((double)RAND_MAX + 1)));
      	
      Ant *ant = new Ant(r,g,b,c);
      
      if(i%2 == 0)
	{
	  ant->setAlpha(0.1);       
	}
      else
	{
	  ant->setAlpha(0.9);    
	}
      
      antList.push_back(ant);
    }    
}



void createAntALPHA_RANDOM_ALL(int nb)
{
  for(int i = 0 ; i < nb ; i++)
    {
      Coord c((int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),0,(int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))));
      float r = (float)(((double)rand() / ((double)RAND_MAX + 1)));
      float g = (float)(((double)rand() / ((double)RAND_MAX + 1)));
      float b = (float)(((double)rand() / ((double)RAND_MAX + 1)));
      	
      Ant *ant = new Ant(r,g,b,c,(double)(((double)rand() / ((double)RAND_MAX + 1))));
      antList.push_back(ant);
    }    
}



/*
 *cette fonction cree nb ants avec alpha = a
 */
void createAnt(int nb, double a)
{
  for(int i = 0 ; i < nb ; i++)
    {
      Coord c((int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),0,(int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))));
      float r = (float)(((double)rand() / ((double)RAND_MAX + 1)));
      float g = (float)(((double)rand() / ((double)RAND_MAX + 1)));
      float b = (float)(((double)rand() / ((double)RAND_MAX + 1)));
      	
      Ant *ant = new Ant(r,g,b,c,a);
      antList.push_back(ant);
    } 
}


/*
 *creation des fourmis
 *si l'utilisateur n'a pas proposee une valeur pour alpha (ie ALPHA_POP < 0)
 *on utilise un des types predefinis.
 *la aussi l'utilisateur peut donner un type predefini avec l'option -p=[0,7]
 *sans option on utilise un alpha aleatoire et unique a la population
 */
void createAnt()
{ 
      switch(createAntType)
	{
	case ALPHA_HALF : createAntALPHA(NB_ANT);
	  break;
	case ALPHA_VOISIN : createAnt(NB_ANT,0.05);
	  break;
	case ALPHA_POS : createAnt(NB_ANT,0.95);
	  break;
	case ALPHA_RANDOM : createAnt(NB_ANT,(double)(((double)rand() / ((double)RAND_MAX + 1))));
	  break;
	case ALPHA_RANDOM_ALL : createAntALPHA_RANDOM_ALL(NB_ANT);
	  break;
	default : createAnt(NB_ANT,(double)(((double)rand() / ((double)RAND_MAX + 1))));
	}
}
/*creation des cubes*/
void createCube()
{   

  switch(createCubeType)
    {
    case ONE_CUBE : createOneCube();
      break;
    case CUBE_X : createCubeX();
      break;
    case CIRCLE : createCircle();
      break;
    case CYLINDER : createCylinder();
      break;
    case TABLE : createTable();
      break; 
      //pour test avec CubePhero ne pas les faire construire
    case CUBE_PHERO : createCubePhero();
      break;
    case CUBE_ANT : createANT();
      break;
    case RANDOM : createRandom();
      break;
    default : createOneCube();
    }
}


/*initialisation global*/
void globalInit()
{
  /*initialisation du generateur de nombre aleatoire*/
  srand(time(NULL));

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /*nettoyage de l'ecran*/
  glClearColor(0.5,0.5,1.0,1);

  /*pas d'affichage des faces arriere pour alleger le gpu*/
  //glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_BLEND);

  /*
   *nettoyage du buffer de profondeur + 
   *gestion de la profondeur (Z-buffer)
   */
  glClearDepth(1.0);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glShadeModel(GL_SMOOTH);

  /*activation + initialisation de la lumiÃ¨re et de la transparence*/
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);

  glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  /*definit la couleur de dessin (ie blanc de base)*/
  glColor3f(1.0,1.0,1.0);  

  /*initialisation de l'env
  for(int i = 0 ; i < SIZE_ENV ; i++)
    {
      for(int j = 0 ; j < SIZE_ENV ; j++)
    	{
    	  for(int k = 0 ; k < SIZE_ENV ; k++)
    	    {
    	      env[i][j][k] = NULL;
    	    }
    	}
    }*/

  clearEnv();
}


void clearEnv()
{
  //std::cout<<"CLR ENV [IN]"<<std::endl;
  /*initialisation de l'env*/
  for(int i = 0 ; i < SIZE_ENV ; i++)
    {
      for(int j = 0 ; j < SIZE_ENV ; j++)
    	{
    	  for(int k = 0 ; k < SIZE_ENV ; k++)
    	    {
    	      env[i][j][k] = NULL;
    	    }
    	}
    } 
  //std::cout<<"CLR ENV [OUT]"<<std::endl;
}

/*fonction appelee avant de quitter le program pour nettoyer la memoire allouee*/
void cleanAll()
{
  if(_cam){
    delete _cam;
  }
}

/*****************************************************************/
//                   FONCTIONS CREATION CUBES                             
/*****************************************************************/


/*
 *Cree une structure "aleatoire" basee sur des droites et des demis cercles
 *la forme suivante a tracee l'est toujours a partir du dernier cube trace
 *le dernier cube cree est la cible de la colonie
 */
void createRandom()
{
  Coord start((int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),0,(int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))));
  Coord end((int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),(int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),(int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))));
  std::list<Cube *>::iterator itCube;
  double probaLine;
  int axe;

  drawLine3D(start,end);

  // a partir du dernier du cube
  itCube = cubeList.end();
  itCube--;     
  start = (*itCube)->getCoord();
  
  while(cubeList.size() < 2500)
    {
      probaLine = (float)(((double)rand() / ((double)RAND_MAX + 1)));
      if(probaLine >0.5)
	{
	  axe = (int)(((double)rand() / ((double)RAND_MAX + 1)*(3)));
	  start = drawArc3D(start,(int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV/4))),axe);
	}
      else
	{
	  if((cubeList.size()%5) == 0)
	    {
	      end.y = 0;
	    }
	  else
	    {
	      end.y = (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1)));
	    }

	  end.x = (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1)));
	  end.z = (int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1)));
      
	  drawLine3D(start,end);

	  itCube = cubeList.end();
	  itCube--;
          
	  start = (*itCube)->getCoord();
	}
    }
    
  itCube = cubeList.end();
  itCube--;
  Ant::target = (*itCube)->getCoord();
}



void createCylinder()
{
  for(int y = 0 ; y < SIZE_ENV  ; y++)
    {
      for(int i = 0 ; i < 360; i++)
	{
	  double cosinus = (cos(i*PI/180)+1)*(SIZE_ENV/2);
	  double sinus = (sin(i*PI/180)+1)*(SIZE_ENV/2);
	  Coord cd((int)cosinus,(int)sinus,y);
	  double dist = cd.influenceDistance(Ant::target);
	  Cube *cube = new Cube(1,1,1,cd,dist,dist,false);
	  cubeList.push_back(cube); 
	}
    }
  cubeList.unique();
}



void createCircle()
{
  for(int i = 0 ; i <= 360; i++)
    {
      double cosinus = (cos(i*PI/180)+1)*(SIZE_ENV/2);
      double sinus = (sin(i*PI/180)+1)*(SIZE_ENV/2);
      Coord cd((int)cosinus,(int)sinus,SIZE_ENV/2);
      double dist = cd.influenceDistance(Ant::target);
      Cube *cube = new Cube(1,1,1,cd,dist,dist,false);
      cubeList.push_back(cube); 
    }
  cubeList.unique();
}



void createOneCube()
{
  Coord c((int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))),0,(int)(((double)rand() / ((double)RAND_MAX + 1)*(SIZE_ENV-1))));
  Cube *cube = new Cube(1,1,1,c,(float)(((double)rand() / ((double)RAND_MAX + 1))),(float)(((double)rand() / ((double)RAND_MAX + 1))),false);
  cubeList.push_back(cube);
}



/*creation des cubes*/
void createCubeX()
{
  /*la couleur des cubes est elle random ou pas*/
  bool randomColor = false;
  /*composante RGB des cubes*/
  float r,g,b;
  Coord origin(0,0,0);
  Coord center(SIZE_ENV/2,SIZE_ENV/2,SIZE_ENV/2);

  for(int i = 0 ; i <SIZE_ENV ; i++)
    {
      if(!randomColor)
	{
	  r=g=b=0;
	}
      else 
	{
	  r=g=b= (float)(((double)rand() / ((double)RAND_MAX + 1)));
	}
      //colonnes B&W 4 coins + centre
      Coord c(SIZE_ENV/2,i,SIZE_ENV/2);
      Cube *cube = new Cube(1-r,1-g,1-b,c,1,1,false);
      cubeList.push_back(cube);

      Coord c2(0,i,0);
      Cube *cube2 = new Cube(r,g,b,c2,1,1,false);
      cubeList.push_back(cube2);
     
      Coord c3(SIZE_ENV-1,i,0);
      Cube *cube3 = new Cube(1-r,1-g,1-b,c3,1,1,false);
      cubeList.push_back(cube3);
	
      Coord c4(0,i,SIZE_ENV-1);
      Cube *cube4 = new Cube(1-r,1-g,1-b,c4,1,1,false);
      cubeList.push_back(cube4);

      Coord c5(SIZE_ENV-1,i,SIZE_ENV-1);
      Cube *cube5 = new Cube(r,g,b,c5,1,1,false);
      cubeList.push_back(cube5);
      
      //X 3D B&W
      Coord c6(SIZE_ENV/2,i,SIZE_ENV/2);
      Cube *cube6 = new Cube(1-r,1-g,1-b,c6,1,1,false);
      cubeList.push_back(cube6);
      
      Coord c7(SIZE_ENV-1-i,i,SIZE_ENV-1-i);
      Cube *cube7 = new Cube(r,g,b,c7,1,1,false);
      cubeList.push_back(cube7);

      Coord c8(SIZE_ENV-1-i,i,i);
      Cube *cube8 = new Cube(1-r,1-g,1-b,c8,1,1,false);
      cubeList.push_back(cube8);
      	
      Coord c9(i,i,SIZE_ENV-1-i);
      Cube *cube9 = new Cube(r,g,b,c9,1,1,false);
      cubeList.push_back(cube9);
	
      Coord c10(i,i,i);
      Cube *cube10 =  new Cube(1-r,1-g,1-b,c10,1,1,false);
      cubeList.push_back(cube10);
      
      Coord c11(SIZE_ENV/2,SIZE_ENV/2,i);
      Cube *cube11 = new Cube(r,g,b,c11,1,1,false);
      cubeList.push_back(cube11);
      
      Coord c12(i,SIZE_ENV/2,SIZE_ENV/2);
      Cube *cube12 =  new Cube(1-r,1-g,1-b,c12,1,1,false);
      cubeList.push_back(cube12);

      Coord c13(SIZE_ENV/2,i,0);
      Cube *cube13 = new Cube(r,g,b,c13,1,1,false);
      cubeList.push_back(cube13);
	
      Coord c14(SIZE_ENV/2,i,SIZE_ENV-1);
      Cube *cube14 =  new Cube(1-r,1-g,1-b,c14,1,1,false);
      cubeList.push_back(cube14);
      
      Coord c15(0,i,SIZE_ENV/2);
      Cube *cube15 = new Cube(r,g,b,c15,1,1,false);
      cubeList.push_back(cube15);
      
      Coord c16(SIZE_ENV-1,i,SIZE_ENV/2);
      Cube *cube16 =  new Cube(1-r,1-g,1-b,c16,1,1,false);
      cubeList.push_back(cube16);
    }  
  
}

void createANT()
{
  //"A"
  drawLine3D(Coord(0,0,5),Coord(SIZE_ENV/4,SIZE_ENV/2,5));
  drawLine3D(Coord(SIZE_ENV/4,SIZE_ENV/2,5),Coord(SIZE_ENV/2,0,5));
  drawLine3D(Coord(SIZE_ENV/8,SIZE_ENV/4,5),Coord(3*SIZE_ENV/8,SIZE_ENV/4,5));

  //"N"
  drawLine3D(Coord(SIZE_ENV/3,0,SIZE_ENV/2),Coord(SIZE_ENV/3,SIZE_ENV/2,SIZE_ENV/2));
  drawLine3D(Coord(SIZE_ENV/3,SIZE_ENV/2,SIZE_ENV/2),Coord(2*SIZE_ENV/3,0,SIZE_ENV/2));
  drawLine3D(Coord(2*SIZE_ENV/3,0,SIZE_ENV/2),Coord(2*SIZE_ENV/3,SIZE_ENV/2,SIZE_ENV/2));

  //"T"
  drawLine3D(Coord(3*SIZE_ENV/4,0,SIZE_ENV-5),Coord(3*SIZE_ENV/4,SIZE_ENV/2,SIZE_ENV-5));
  drawLine3D(Coord((int)4.5*SIZE_ENV/8,SIZE_ENV/2,SIZE_ENV-5),Coord(SIZE_ENV-5,SIZE_ENV/2,SIZE_ENV-5));
}



/*
 *Creer "une table"
 */
void createTable()
{
  //4 pieds
  drawLine3D(Coord(SIZE_ENV/4,0,SIZE_ENV/4),Coord(SIZE_ENV/4,SIZE_ENV/2,SIZE_ENV/4));
  drawLine3D(Coord(3*SIZE_ENV/4,0,3*SIZE_ENV/4),Coord(3*SIZE_ENV/4,SIZE_ENV/2,3*SIZE_ENV/4));
  drawLine3D(Coord(3*SIZE_ENV/4,0,SIZE_ENV/4),Coord(3*SIZE_ENV/4,SIZE_ENV/2,SIZE_ENV/4));
  drawLine3D(Coord(SIZE_ENV/4,0,3*SIZE_ENV/4),Coord(SIZE_ENV/4,SIZE_ENV/2,3*SIZE_ENV/4));

  //plateau
  for(int i = (SIZE_ENV/4) - (SIZE_ENV/8) ; i < (3*SIZE_ENV/4) + (SIZE_ENV/8); i++)
    {
      for(int j = (SIZE_ENV/4) - (SIZE_ENV/8) ; j < (3*SIZE_ENV/4) + (SIZE_ENV/8); j++)
	{
	  Cube *cube5 = new Cube(0.5,Coord(i,SIZE_ENV/2,j),1,1,false);
          cubeList.push_back(cube5);        
	}         
    }
}



/*Les fourmis sont attirées par le sommet de la face arriere*/
void createCubePhero()
{

  for(int i = 0 ; i < SIZE_ENV ; i++)
    {
      for(int j = 0 ; j < SIZE_ENV ; j++)
	{
	  //Face arriere
	  Cube *cube1 = new Cube(0.0,0.0,0.0,Coord(i,j,0),(SIZE_ENV-j)*10 + (j*100),((SIZE_ENV-j)*10 + (j*100)));
	  cubeList.push_back(cube1);  
      
	  //sol
	  Cube *cube2 = new Cube(0.0,0.0,0.0,Coord(i,0,j),(SIZE_ENV-j)*10,((SIZE_ENV-j)*10));
	  cubeList.push_back(cube2); 
	}
    }     

}

/*****************************************************************/
//                   FONCTIONS OPENGL                             
/*****************************************************************/


/*gestion des touches "speciales" (ie F1,..,PAGE_UP)*/
void keypress(int k, int x, int y)
{
  int i=1;
  switch(k){
  case GLUT_KEY_F1 : _cam->initCam(SIZE_ENV,CUBE_SIZE);
    break;
  case GLUT_KEY_F2 : bigCube = !bigCube;
    break;
  case GLUT_KEY_F3 : typeConst = !typeConst;
    break;  
  case GLUT_KEY_F4 :  displayText= !displayText;
    break; 
  case GLUT_KEY_F5 :  wire = !wire;
    break;
  case GLUT_KEY_F6 :  ground = !ground;
    break;
  case GLUT_KEY_F7 :  
    {
      repPheroMove = !repPheroMove;
      repPheroConst = false;
    }
    break;
  case GLUT_KEY_F8 :  
    {
      repPheroConst = !repPheroConst;
      repPheroMove = false;
    }
    break;
  case GLUT_KEY_F9 :  
    {
      sculpToAntFile(createAntType,NB_ANT);
    }
    break;
  case GLUT_KEY_INSERT:
    {
     createAnt(1,(double)(((double)rand() / ((double)RAND_MAX + 1)))); 
    }
  default : i--;break;
  }
 
  if(i)glutPostRedisplay();
}

/*gestion des touches simples*/
void keyboard(unsigned char k,int x,int y)
{
  int i=1;
  switch(k){
  case 9   : step++;break;/*tab*/
  case 27  : exit(0);break;/*escape*/
  case 32  : play=!play;break;/*space bar*/
  case 'z' : _cam->tabTrans[2]+=5.0;break;/*avance la camera*/
  case 's' : _cam->tabTrans[2]-=5.0;break;/*recule de la camera*/
  case 'q' : _cam->tabTrans[0]+=5.0;break;/*straff droite de la camera*/
  case 'd' : _cam->tabTrans[0]-=5.0;break;/*starff gauche de la camera*/
  default : i--;break;
  }
  
  /*on ne re-display pas si rien n'a change*/
  if(i)glutPostRedisplay();

}

/*fonction appelee lorsqu'il n'y a rien d'autre a faire*/
void idle()
{
  glutPostRedisplay();
  displayInfo();
}

/*comment gerer le redimensionnement de la fenetre*/
void reshape(int w,int h)
{
  _cam->reshapeWindow(w,h);
}

/*gestion des mouvements de la souris*/
void mouseMotion(int x,int y)
{
  _cam->camOnMouseMove(x,y);
}

/*gestion des boutons de la souris*/
void mouse(int button,int state,int x,int y)
{
  _cam->camOnMousepress(button,state,x,y);
}
