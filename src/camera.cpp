 #include "camera.h"


camera::camera(int sizeEnv,int cubeSize)
{
  tabTrans=new GLfloat[3];
  rot=new GLfloat[2];
  sensy=0.5;
  tabRot=new GLfloat[16];
  initCam(sizeEnv,cubeSize);
  spinXY=transXY=zoom=0;
  PI = 3.14159265;
}

camera::~camera()
{
  Delete();
}

void camera::Delete()
{
  delete [] tabTrans;
  delete [] rot;
  delete [] tabRot;
}


void camera::lockedAndLoaded()
{
     glTranslatef(tabTrans[0],tabTrans[1],tabTrans[2]);
     glMultMatrixf(tabRot); 
}


void camera::reshapeWindow(int w,int h)
{
  /* aspect de la fenetre */
  double ratio = (double)w/h;

  /* place de l'image dans la fenetre */
  glViewport( 0, 0, w, h );

  
  /* transformation perspective */
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  
  gluPerspective(60.0,ratio,0.1,10000.0);
  glMatrixMode(GL_MODELVIEW);
}

void camera::initCam(int sizeEnv,int cubeSize)
{
  GLfloat tempTrans[3] = { 0,0,-2*(sizeEnv*cubeSize)};
  
  GLfloat tempPos[3] = { 0.0,0.0,0.0};
  for(int i=0;i<3;i++)
    {
      tabTrans[i]=tempTrans[i];
      if(i<2)rot[i]=tempPos[i];
    }
  GLfloat tempRot[16] = {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
  };
  for(int i=0;i<16;i++)tabRot[i]=tempRot[i];
}



void camera::camOnMouseMove(int x,int y)
{
  double dx,dy,nrm;

  if( spinXY || transXY || zoom )
    {
      /* deplacement */
      dx = x - oldX;
      dy = oldY - y; /* axe vertical dans l'autre sens */

      if( spinXY ){
	nrm = sqrt( dx*dx+dy*dy+dx*dx+dy*dy );
	glLoadIdentity();
	glRotatef( nrm, -dy, dx, 0 );/*axe perpendiculaire au deplacement*/
	glMultMatrixf( tabRot );
	glGetFloatv( GL_MODELVIEW_MATRIX, tabRot );
      }
      else if( transXY){
	tabTrans[0] += dx/5.0;
	tabTrans[1] += dy/5.0;
      }
      else {
	glLoadIdentity();
	glRotatef( dx, 0,0,-1 );/*axe perpendiculaire a l'ecran*/
	glMultMatrixf( tabRot );
	glGetFloatv( GL_MODELVIEW_MATRIX, tabRot );
      }
      oldX = x;
      oldY = y;
      glutPostRedisplay();
    }
}


void camera::camOnMousepress(int button,int state,int x,int y)
{
 /* enfoncer gauche */
  if( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ){
    spinXY = 1;
    oldX = x;
    oldY = y;
  }
  /* relacher gauche */
  else if( button==GLUT_LEFT_BUTTON && state==GLUT_UP ){
    spinXY = 0;
  }
  /* enfoncer milieu */
  if( button==GLUT_MIDDLE_BUTTON && state==GLUT_DOWN ){
    zoom = 1;
    oldX = x;
    oldY = y;
  }
  /* relacher milieu */
  else if( button==GLUT_MIDDLE_BUTTON && state==GLUT_UP ){
    zoom = 0;
  }
  /* enfoncer droit */
  else if( button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN ){
    transXY = 1;
    oldX = x;
    oldY = y;
  }
  /* relacher droit */
  else if( button==GLUT_RIGHT_BUTTON && state==GLUT_UP ){
    transXY = 0;
  }

}
