/*******************************************************
 *
 *Le main permet de creer le context OpenGL (via glut)
 *
 ******************************************************/

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include "ant3D.h"

/*point d'entree du prog*/
int main(int argc, char** argv)
{
  bool status = false;
  int nbCube = 0;
  int nbParent = 0;
  int nbGene = 0;
  EvolutionEngine* engine = NULL;
  
  /*fonction a appeler avant de quitter le programe*/
  atexit(cleanAll);

  /*initialise GLUT et traitement d'eventuels arguments en ligne de commande*/
  glutInit(&argc,argv);
  
  /*specifie le mode KROMATIK, le double tampon, profondeur*/
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  
  /*taille de la fenetre */
  glutInitWindowSize(800,600);
  
  /*position de la fenetre*/
  glutInitWindowPosition(0,0);
  
  /*creation de la fenetre dans le contexte specifie*/
  glutCreateWindow("Ant3D");

  /*initialisation des differents params*/
  globalInit();

  if(argc == 1)
    {
      status = true;
      createAnt();
      createCube(); 
    }
  if(argc == 2)
    {
      char * extension = strchr(argv[1],'.');
      
      if(strncmp(extension,".cfg",strlen(".cfg")) == 0)
	{
	  status = extractConfigFile(argv[1]);
	}
      else
	{
	  std::cout<<"MAUVAIS FORMAT DE FICHIER => .cfg"<<std::endl;
	}
    }
  else if(argc == 5)
    { 
      status = true;
      nbGene = atoi(argv[2]);
      nbParent = atoi(argv[3]);  
      nbCube = atoi(argv[4]);
    }


  if(status)
    {
      /*si il y a un nom de fichier specifie*/
      if(argc < 3)
	{
	  /*definition des fonctions callback de glut*/
	  glutReshapeFunc(reshape);
	  glutKeyboardFunc(keyboard);
	  glutSpecialFunc(keypress);
	  glutMouseFunc(mouse);       
	  glutMotionFunc(mouseMotion);
	  glutDisplayFunc(display);
	  glutIdleFunc(idle);
      
	  glutFullScreen();

	  glutMainLoop();
	}
      else if(argc == 5)
	{
	  EvolutionEngine::setNbCube(nbCube);
	  EvolutionEngine::setNbParent(nbParent);
	  EvolutionEngine::setNbGeneration(nbGene);
	  EvolutionEngine::setFilename(std::string(argv[1]));
	  engine = EvolutionEngine::getInstance();
	  engine->startEvolution();
	}
    }
  else
    {
      std::cout<<"ERREUR!! SHUTTING DOWN"<<std::endl;
    }
  

  return EXIT_SUCCESS;
}


bool extractConfigFile(std::string str)
{
  FILE *pFile = fopen(str.c_str(),"r");
  char line[256];
  char *c;
  char param[128];
  char val[128];
  size_t s;
  bool filaire = false;
  bool status = true;
  char * extension;
  double alpha = 0.25;
  int nbAnt = 100;

  if(pFile != NULL)
    {
      while(!feof(pFile))
	{
	  fgets(line,255,pFile);
	  
	  c = strchr(line,'=');
	  s = strlen(line) - strlen(c);
	  
	  strncpy(param,line,s); 
	  param[s] = '\0';
	  
	  s = strlen(line) - s;
	  
	  strncpy(val,c+1,s-2); 
	  val[s-2] = '\0';
	  
	  if(strncmp("alpha",param,strlen("alpha")) == 0)
	    {
	      alpha = atof(val);
	    }
	  if(strncmp("ant",param,strlen("ant")) == 0)
	    {
	      nbAnt = atoi(val);
	      NB_ANT = nbAnt;
	    }
	  if(strncmp("evapm",param,strlen("evapm")) == 0)
	    {
	      Cube::evapMove = atof(val);
	    }
	  if(strncmp("evapc",param,strlen("evapc")) == 0)
	    {
	      Cube::evapConst = atof(val);
	    }
	  if(strncmp("wire",param,strlen("wire")) == 0)
	    {
	      filaire = atoi(val);
	    }
	  
	  if(strncmp("file",param,strlen("file")) == 0)
	    {
	      extension = strrchr(val,'.'); 
	      
	      //import de .obj
	      if(strncmp(extension,".obj",strlen(".obj")) == 0)
		{
		  std::cout<<"LOADING OBJ FILE : "<<val<<std::endl;
		  status = objFileToSculp(val,filaire);
		}
	      //import de .ant
	      else if(strncmp(extension,".ant",strlen(".ant")) == 0)
		{
		  //std::cout<<"LOADING ANT FILE : "<<val<<std::endl; 
		  status = antFileToSculp(val);
		}
	      //import de rien du tout...
	      else
		{
		  std::cout<<"FORMAT INCONNU ..."<<std::endl;  
		  status = false;
		}
	      
	      if(status)
		{
		  //std::cout<<"LOADING COMPLETE"<<std::endl;
		}
	      else 
		{
		  std::cout<<"LOADING INCOMPLETE"<<std::endl;
		} 
	    }
	  if(strncmp("x",param,strlen("x")) == 0)
	    {
	      Ant::target.x = atoi(val);
	    }
	  if(strncmp("y",param,strlen("y")) == 0)
	    {
	      Ant::target.y = atoi(val);
	    }
	  if(strncmp("z",param,strlen("z")) == 0)
	    {
	      Ant::target.z = atoi(val);
	    }
	}
      
      createAnt(NB_ANT,alpha);
    }
  else 
    {
      std::cout<<"ERREUR OUVERTURE FICHIER CONFIG..."<<std::endl;
      status = false;
    }

  return status;
}
