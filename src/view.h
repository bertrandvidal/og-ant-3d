/*******************************************************
 *
 *La partie representation du projet se trouve dans ce 
 *fichier: la creation du contexte OpenGL, gestion des
 *des interactions (clavier+souris)  ainsi que la
 *gestion de la camera (cf camera.h).
 *
 ******************************************************/
#ifndef VIEW_H
#define VIEW_H

#include <GL/glut.h> 
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>
#include <list>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string>
#include <stdlib.h>

#include "camera.h"
#include "util.h"
#include "ant.h"
#include "cube.h"
#include "export.h"

/*les differentes formes de base*/
/*
  ONE_CUBE : un cube place aleatoirement
  CUBE_X : un ensemble de barres 
  CIRCLE : un cercle au milieu de l'env
  CYLINDER : un demi-cylindre couche
  TABLE : une table (4 pieds et un plateau)
  CUBE_PHERO : deux parois de + en + attirantes
  CUBE_ANT : ecrit ANT dans l'env
  RANDOM : creation de 2500 cube aléatoirement dans l'env
 */
enum structType {ONE_CUBE,CUBE_X,CIRCLE,CYLINDER,TABLE,CUBE_PHERO,CUBE_ANT,RANDOM};

/*nombre de fourmis*/
static int NB_ANT = 400;

/*parcours antList pour action*/
void colonieAction(unsigned int);

/*fonction d'affichage*/
void display();

/*creation des cubes*/
void createCube();

void createOneCube();
void createCubeX();
void createCircle();
void createCylinder();
void createTable();
void createCubePhero();
void createANT();
void createRandom();

/*creation des fourmis*/
void createAnt();

/*fonctions creant les != types de fourmis*/
void createAnt(int, double);
void createAntALPHA(int);
void createAntALPHA_RANDOM_ALL(int);

void displayInfo();

/*gestion des touches "speciales" (ie F1,.., PAGE_UP)*/
void keypress(int k, int x, int y);

/*gestion des touches simples*/
void keyboard(unsigned char k,int x,int y);

/*fonction appelee lorsqu'il n'y a rien d'autre a faire*/
void idle();

/*comment gerer le redimensionnement de la fenetre*/
void reshape(int w,int h);

/*gestion des mouvements de la souris*/
void mouseMotion(int x,int y);

/*gestion des boutons de la souris*/
void mouse(int button,int state,int x,int y);

/*initialisation global*/
void globalInit();

void clearEnv();

/*fonction appelee avant de quitter le program pour nettoyer la memoire allouee*/
void cleanAll();

#endif

