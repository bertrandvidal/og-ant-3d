#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glut.h>
#include <math.h>



class camera 
{
  GLfloat *tabRot;
  GLfloat *rot;
  GLint oldX,oldY,spinXY,transXY,zoom;
  GLfloat PI;
  GLfloat sensy;
 public :
  GLfloat *tabTrans;
  camera(int,int);
  void lockedAndLoaded();
  void reshapeWindow(int,int);
  void camOnMouseMove(int,int);
  void camOnMousepress(int,int,int,int);
  void initCam(int,int);
  ~camera();

 private :
  void Delete();
};

#endif
