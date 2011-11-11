#include "ifs.h"
#include <stdio.h>
#include <stdlib.h>
//#include "GL/glui.h"
//#include <iostream>
#include <GL/glut.h>
#include <math.h>
#include "transformation.h"
//#inlcude "Turtle.h"

//defining the values of different contants

#define SIZE 500  /* the size, in pixels, of the square window to open, YOU CAN CHANGE THIS */
#define scale_factor_up 1.5
#define scale_factor_down 0.6666
#define angle_x 10*M_PI/180
#define angle_y 10*M_PI/180
#define angle_z 10*M_PI/180
#define height 3.0
#define width 3.0
#define far 3.0

//location of the camera and object

float object_x,object_y,object_z;
float e_x,e_y,e_z,c_x,c_y,c_z,u_x,u_y,u_z;
int flag_lookat;

float dist;

// object containing the points of the teapot
struct IFS_DATA* data;

float x_1,y_1,z_1,x_2,y_2,z_2;
float theta;

//function to draw the traingles

void draw_triangle(IFS_DATA* ifs_data,int i)
{  
	  int vertex1,vertex2,vertex3;

	  vertex1=(ifs_data->triangles[i].a)->id;
	  vertex2=(ifs_data->triangles[i].b)->id;
	  vertex3=(ifs_data->triangles[i].c)->id;
	  
	  glBegin(GL_LINE_LOOP);
//	  glBegin(GL_TRIANGLES);
	  glVertex3f(ifs_data->vertices[vertex1].x,ifs_data->vertices[vertex1].y,ifs_data->vertices[vertex1].z);
	  glVertex3f(ifs_data->vertices[vertex2].x,ifs_data->vertices[vertex2].y,ifs_data->vertices[vertex2].z);
	  glVertex3f(ifs_data->vertices[vertex3].x,ifs_data->vertices[vertex3].y,ifs_data->vertices[vertex3].z);
	  glEnd();
	  glFlush();
}

//function to draw the teapot

void draw_figure(IFS_DATA* ifs_data)
{
  int i;
  for (i=0; i<ifs_data->numTriangles; ++i) 
  {
      draw_triangle(ifs_data,i);	
  }

}

// my implementation of gllookat

void look_ortho(IFS_DATA *ifs_data,float eye1_x,float eye1_y,float eye1_z,float center1_x,float center1_y,float center1_z,float up1_x,float up1_y,float up1_z)
{

    
    vector w, v, up,u;
    
    Matrix m;	
    
    
    w.A[0] = -1*(center1_x - eye1_x);
    w.A[1] = -1*(center1_y - eye1_y);
    w.A[2] = -1*(center1_z - eye1_z);
    w.A[3]=1.0;
    
   // printf("printing w\n");
   // print_vector(w);
    
    up.A[0] = up1_x;
    up.A[1] = up1_y;
    up.A[2] = up1_z;
    up.A[3]=1.0;

   // printf("printing up\n");
   // print_vector(up);
    
    w=normalize(w);

    float dot=up.A[0]*w.A[0]+up.A[1]*w.A[1]+up.A[2]*w.A[2];
    v.A[0]=up.A[0]-dot*w.A[0];
    v.A[1]=up.A[1]-dot*w.A[1];
    v.A[2]=up.A[2]-dot*w.A[2];
    v.A[3]=1.0;
    v=normalize(v);

    /* Side = forward x up */
    u=cross_product(v, w);
    //print_vector(side);
 //   side=normalize(side);

    /* Recompute up as: up = side x forward */
   // up=cross_product(side, f);

    //print_vector(up);  
    
    m.A[0][0] = u.A[0];
    m.A[1][0] = u.A[1];
    m.A[2][0] = u.A[2];
    m.A[3][0] = 0.0;

    
    m.A[0][1] = v.A[0];
    m.A[1][1] = v.A[1];
    m.A[2][1] = v.A[2];
    m.A[3][1] = 0.0;

    m.A[0][2] = w.A[0];
    m.A[1][2] = w.A[1];
    m.A[2][2] = w.A[2];
    m.A[3][2] = 0.0;
    
        m.A[0][3] = 0.0;
        m.A[1][3] = 0.0;
        m.A[2][3] = 0.0;
        m.A[3][3] = 1.0;
	Matrix T=return_translate(-eye1_x,-eye1_y,-eye1_z);
//	print(T);
    Matrix trans_matrix=multiply(m,T);
    
    
    vector o;
    o.A[0]=object_x;
    o.A[1]=object_y;
    o.A[2]=object_z;
    o.A[3]=1.0;
    o=multiply_vector(&trans_matrix,&o);
    
    object_x=o.A[0];
    object_y=o.A[1];
    object_z=o.A[2];

    //print_vector(o);
    
 //   print(trans_matrix);
    int i,j;
    
    
    
  for (i=0; i<ifs_data->numVertices; ++i) 
  {
    vector v;
    v.A[0]=ifs_data->vertices[i].x;
    v.A[1]=ifs_data->vertices[i].y;
    v.A[2]=ifs_data->vertices[i].z;
    v.A[3]=1.0;
	v=multiply_vector(&trans_matrix,&v);
    //for(=0;i<4;i++)
    ifs_data->vertices[i].x=v.A[0];
    ifs_data->vertices[i].y=v.A[1];
    ifs_data->vertices[i].z=v.A[2];   
  }

	

//    glMultMatrixf(&m[0][0]);
    //glTranslated(-eyex, -eyey, -eyez);

}

//function to scale the object around its own center
void scale_constant(IFS_DATA* ifs_data,float scale_x,float scale_y,float scale_z)
{
  int i,j;
  
    Matrix trans_matrix;
    
    Matrix T=return_translate(-1*object_x,-1*object_y,-1*object_z);
    Matrix scale=return_scale_Matrix(scale_x,scale_y,scale_z);
    
    trans_matrix=multiply(scale,T);
    
    T=return_translate(object_x,object_y,object_z);
    
    trans_matrix=multiply(trans_matrix,T);

    //print(trans_matrix);
    for (i=0; i<ifs_data->numVertices; ++i) 
  {
    vector v;
    v.A[0]=ifs_data->vertices[i].x;
    v.A[1]=ifs_data->vertices[i].y;
    v.A[2]=ifs_data->vertices[i].z;
    v.A[3]=1.0;
    
    //v=create_scale_Matrix(&v,scale_x,scale_y,scale_z);
    
    v=multiply_vector(&trans_matrix,&v);
    
    ifs_data->vertices[i].x=v.A[0];
    ifs_data->vertices[i].y=v.A[1];
    ifs_data->vertices[i].z=v.A[2];
      
  }
    
    
}

//function to scale aroud the world coordinate axes
void scale(IFS_DATA* ifs_data,float scale_x,float scale_y,float scale_z)
{
  int i,j;
  
    Matrix trans_matrix=return_scale_Matrix(scale_x,scale_y,scale_z);
    object_x*=scale_x;
    object_y*=scale_y;
    object_z*=scale_z;
    //print(trans_matrix);
    for (i=0; i<ifs_data->numVertices; ++i) 
    {
    vector v;
    v.A[0]=ifs_data->vertices[i].x;
    v.A[1]=ifs_data->vertices[i].y;
    v.A[2]=ifs_data->vertices[i].z;
    v.A[3]=1.0;
    
    //v=create_scale_Matrix(&v,scale_x,scale_y,scale_z);
    
    v=multiply_vector(&trans_matrix,&v);
    
    ifs_data->vertices[i].x=v.A[0];
    ifs_data->vertices[i].y=v.A[1];
    ifs_data->vertices[i].z=v.A[2];
      
  }
    
    
}


//function to rotate the camera around camera coordinate system
Rotate_Camera_around_own_axis(float theta,int axes)
{
  vector v;
  v.A[0]=c_x;
  v.A[1]=c_y;
  v.A[2]=c_z;
  v.A[3]=1.0;
  
  vector v1;
  v1.A[0]=u_x;//-e_x;
  v1.A[1]=u_y;//-e_y;
  v1.A[2]=u_z;//-e_z;
  v1.A[3]=1.0;

  
  Matrix m;
  switch(axes)
  {
    case 1:
      m=return_rotate_x(theta);
      break;
    case 2:
      m=return_rotate_y(theta);
      break;
    case 3:
      m=return_rotate_z(theta);
      break;
  }
  
  v=multiply_vector(&m,&v);
//  printf("vector v again is \n");
 // print_vector(v);
  v1=multiply_vector(&m,&v1);
//  printf("vector v1 again is \n");
 // print_vector(v1);

 
 look_ortho(data,e_x,e_y,e_z,v.A[0],v.A[1],v.A[2],v1.A[0],v1.A[1],v1.A[2]);
  
}

//function to rotate along the world coordinate system
void rotate_along(IFS_DATA* ifs_data,int axes)
{
  Matrix k;
  switch(axes)
  {
    case 1:
      k=return_rotate_x(theta);
      break;
    case 2:
      k=return_rotate_y(theta);
      break;
    case 3:
      k=return_rotate_z(theta);
      break;
  }
  
  vector temp;
  temp.A[0]=object_x;
  temp.A[1]=object_y;
  temp.A[2]=object_z;
  temp.A[3]=1.0;
  
  temp=multiply_vector(&k,&temp);
  
  object_x=temp.A[0];
  object_y=temp.A[1];
  object_z=temp.A[2];

int i,j;
  for (i=0; i<ifs_data->numVertices; ++i) 
  {
    vector v;
    v.A[0]=ifs_data->vertices[i].x;
    v.A[1]=ifs_data->vertices[i].y;
    v.A[2]=ifs_data->vertices[i].z;
    v.A[3]=1.0;

    switch(axes)
    {
      case 1:
	v=rotate_x(&v,angle_x);
	break;
      case 2:
	v=rotate_y(&v,angle_y);
	break;
      case 3:
	v=rotate_z(&v,angle_z);
	break;
    }
    ifs_data->vertices[i].x=v.A[0];
    ifs_data->vertices[i].y=v.A[1];
    ifs_data->vertices[i].z=v.A[2];   
  }
}

//fuction to translate around world coordinate system
void translate_along(IFS_DATA* ifs_data,float x,float y,float z)
{
  int i,j;
  
  object_x+=x;
  object_y+=y;
  object_z+=z;
  
  for (i=0; i<ifs_data->numVertices; ++i) 
  {
    vector v;
    v.A[0]=ifs_data->vertices[i].x;
    v.A[1]=ifs_data->vertices[i].y;
    v.A[2]=ifs_data->vertices[i].z;
    v.A[3]=1.0;
	v=translate(&v,x,y,z);
    //for(=0;i<4;i++)
    ifs_data->vertices[i].x=v.A[0];
    ifs_data->vertices[i].y=v.A[1];
    ifs_data->vertices[i].z=v.A[2];   
  }
}


// my implementation of arbitrary axis
void rotate_arbit_axis(IFS_DATA *ifs_data,float x1,float y1,float z1,float x2,float y2,float z2,float theta)
{
  Matrix trans_matrix;
  int A[4][4]={{1.0,0.0,0.0,0.0},{0.0,1.0,0.0,0.0},{0.0,0.0,1.0,0.0},{0.0,0.0,0.0,1.0}};
  int i,j;
  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      trans_matrix.A[i][j]=A[i][j];
  
    Matrix T=return_translate(-x1,-y1,-z1);
  
  trans_matrix=T;

  
  float u=sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) +(z2-z1)*(z2-z1));
  float x=(x2-x1)/u;
  float y=(y2-y1)/u;
  float z=(z2-z1)/u;
  //printf("%f %f %f \n\n\n",x,y,z);
  
  Matrix R;
  float B[4][4]={{x*x*(1-cos(theta))+cos(theta),x*(1-cos(theta))*y+z*sin(theta),z*x*(1-cos(theta))-y*sin(theta),0.0}
	      ,{x*(1-cos(theta))*y-z*sin(theta),y*y*(1-cos(theta))+cos(theta),z*y*(1-cos(theta))+x*sin(theta),0.0}
	      ,{z*(1-cos(theta))*x+y*sin(theta),z*(1-cos(theta))*y-x*sin(theta),z*z*(1-cos(theta))+cos(theta),0.0}
	      ,{0.0,0.0,0.0,1.0}};
	

  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      R.A[i][j]=B[i][j];
  
  trans_matrix=multiply(R,trans_matrix);
    T=return_translate(x1,y1,z1);
    trans_matrix=multiply(T,trans_matrix);

  for (i=0; i<ifs_data->numVertices; ++i) 
  {
    vector v;
    v.A[0]=ifs_data->vertices[i].x;
    v.A[1]=ifs_data->vertices[i].y;
    v.A[2]=ifs_data->vertices[i].z;
    v.A[3]=1.0;
	v=multiply_vector(&trans_matrix,&v);
    //for(=0;i<4;i++)
    ifs_data->vertices[i].x=v.A[0];
    ifs_data->vertices[i].y=v.A[1];
    ifs_data->vertices[i].z=v.A[2];   
  }
    
    
}


//function to draw the original figure
void get_original()
{
  data=load_ifs_file("teapot.ifs");
  
  //data=d;
  
  object_x=0.0;
  object_y=0.0;
  object_z=0.0;

  e_x=0.0;
  e_y=0.0;
  e_z=0.0;

  c_x=0.0;
  c_y=0.0;
  c_z=-1.0;

  u_x=0.0;
  u_y=1.0;
  u_z=0.0;
  dist=0.0;

}


//init function containing the lightning
void init(void)
{
     GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light_position[] = { 0, 0, 3, 0.0 };
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);

   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);

   
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-height, height, -width, width,-far, far);
	glMatrixMode(GL_MODELVIEW);
	//gluPerspective(60.0,1.0,-1.0,2.0);
}

void ClearScreen()
{
	//Implement this function
	glClearColor(0, 0, 0,0.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}


//GLUT display callback
void display( void )
{
  ClearScreen();
  glColor3f(1.0,1.0,1.0);
  draw_figure(data);
}

//function to zoom in the camera along look vector
void zoom_in(float dist)
{
  vector v;
  v.A[0]=c_x-e_x;
  v.A[1]=c_y-e_y;
  v.A[2]=c_z-e_z;
  v.A[3]=1.0;
  v=normalize(v);
  float x_inc=dist*v.A[0];
  float y_inc=dist*v.A[1];
  float z_inc=dist*v.A[2];

  look_ortho(data,e_x+x_inc,e_y+y_inc,e_z+z_inc,c_x,c_y,c_z,u_x,u_y,u_z);

  //glMatrixMode(GL_MODELVIEW);
  //glLoadIdentity();
  //gluLookAt(e_x,e_y,e_z,c_x,c_y,c_z,u_x,u_y,u_z);
  
  
}


//GLUT callback to handle keyboard events
void keyboard( unsigned char key, int x, int y ) 
{
   
   switch(key)
   {
     case 'n':			//scaling
       scale_constant(data,1.5,1.5,1.5);
       glutPostRedisplay();
       break;
     case 'm':
       scale_constant(data,1.0/1.5,1.0/1.5,1.0/1.5);
       glutPostRedisplay();
       break;
     case '+':			//scaling
       zoom_in(0.3);
       glutPostRedisplay();
       break;
     case '_':			//scaling
       zoom_in(-0.3);
       glutPostRedisplay();
       break;
     case 'i':
       Rotate_Camera_around_own_axis(10*M_PI/180,1);
       glutPostRedisplay();
       break;
     case 'I':
       Rotate_Camera_around_own_axis(-10*M_PI/180,1);
       glutPostRedisplay();
       break;
     case 'j':
       Rotate_Camera_around_own_axis(10*M_PI/180,2);
       glutPostRedisplay();
       break;
     case 'J':
       Rotate_Camera_around_own_axis(-10*M_PI/180,2);
       glutPostRedisplay();
       break;
     case 'k':
       Rotate_Camera_around_own_axis(10*M_PI/180,3);
       glutPostRedisplay();
       break;
     case 'K':
       Rotate_Camera_around_own_axis(-10*M_PI/180,3);
       glutPostRedisplay();
       break;
       /*
     case 116:
	//rotate_along(data,3);
	translate_along(data,0.25,0.3,0.33);
       glutPostRedisplay();
       break;
       */
     case '1':		//translate along x for world
       translate_along(data,0.1,0.0,0.0);
       glutPostRedisplay();
	break;
     case '!':		//translate along -x for world
       translate_along(data,-0.1,0.0,0.0);
       glutPostRedisplay();
	break;
     case '2':		//translate along y for world
       translate_along(data,0.0,0.1,0.0);
       glutPostRedisplay();
	break;
     case '@':		//translate along -y for world
       translate_along(data,0.0,-0.1,0.0);
       glutPostRedisplay();
	break;
     case '3':		//translate along z for world
       translate_along(data,0.0,0.0,0.1);
       glutPostRedisplay();
	break;
     case '#':		//translate along -z for world
       translate_along(data,0.0,0.0,-0.1);
       glutPostRedisplay();
	break;
     case '4':		//rotate around X for world
       rotate_along(data,1);
       glutPostRedisplay();
	break;
     case '5':		//rotate around Y for world
       rotate_along(data,2);
       glutPostRedisplay();
	break;
     case '6':		//rotate around Z for world
       rotate_along(data,3);
       glutPostRedisplay();
	break;
     case '7':		//scale about x
       scale(data,1.5,1.0,1.0);
       glutPostRedisplay();
	break;
     case '&':		//scale about x
       scale(data,1.0/1.5,1.0,1.0);
       glutPostRedisplay();
	break;
     case '8':		//scale about y
       scale(data,1.0,1.5,1.0);
       glutPostRedisplay();
	break;
     case '*':		//scale about y
       scale(data,1.0,1.0/1.5,1.0);
       glutPostRedisplay();
	break;
     case '9':		//scale about z
       scale(data,1.0,1.0,1.5);
       glutPostRedisplay();
	break;
     case ')':		//scale about z
       scale(data,1.0,1.0,1.0/1.5);
       glutPostRedisplay();
	break;
/*
     case 't':		
       printf("Enter the distance in x direction to translate about world coordinate system:-\n");
       scanf("%f",&x_1);
       printf("Enter the distance in y direction to translate about world coordinate system:-  \n");
       scanf("%f",&y_1);
       printf("Enter the distance in z direction to translate about world coordinate system:-  \n");
       scanf("%f",&z_1);
       object_x+=x_1;
       object_y+=y_1;
       object_z+=z_1;       
       translate_along(data,x_1,y_1,z_1);
       glutPostRedisplay();
	break;
*/
     case 'o':
       get_original();
       glutPostRedisplay();
       break;
     case 'r':
       printf("specify the coordidates of the axis represented by 2 vertices\n");
       scanf("%f %f %f %f %f %f",&x_1,&y_1,&z_1,&x_2,&y_2,&z_2);
	printf("enter the angle of rotation\n");
	scanf("%f",&theta);
       rotate_arbit_axis(data,x_1,y_1,z_1,x_2,y_2,z_2,theta*M_PI/180);
       glutPostRedisplay();

     case 'l':
     
      printf("Enter the x y and z coordinates of the eye");
      float eye2_x,eye2_y,eye2_z,center2_x,center2_y,center2_z,up2_x,up2_y,up2_z;
      scanf("%f %f %f",&eye2_x,&eye2_y,&eye2_z);
      printf("Enter the x y and z  cocordinates of the center");
      scanf("%f %f %f",&center2_x,&center2_y,&center2_z);
      printf("enter the  x y and z coordinates of the up vector");
      scanf("%f %f %f",&up2_x,&up2_y,&up2_z);
      
      look_ortho(data,eye2_x,eye2_y,eye2_z,center2_x,center2_y,center2_z,up2_x,up2_y,up2_z);
  	/*
      gluLookAt(0.5,0.0,0.0,0.0,0.0,-1.0,0.0,1.0,0.0);
      glutPostRedisplay();
      break;
      */
     case 'x':
       dist=0.1;
       look_ortho(data,e_x+dist,e_y,e_z,c_x,c_y,c_z,u_x,u_y,u_z);
       glutPostRedisplay();
       break;
     case 'X':
       dist=-0.1;
       look_ortho(data,e_x+dist,e_y,e_z,c_x,c_y,c_z,u_x,u_y,u_z);
       glutPostRedisplay();
       break;
     case 'y':
       dist=0.1;
      look_ortho(data,e_x,e_y+dist,e_z,c_x,c_y,c_z,u_x,u_y,u_z);
       glutPostRedisplay();
       break;
     case 'Y':
       dist=-0.1;
         look_ortho(data,e_x,e_y+dist,e_z,c_x,c_y,c_z,u_x,u_y,u_z);
       glutPostRedisplay();
       break;
     case 'z':
       dist=0.1;
      look_ortho(data,e_x,e_y,e_z+dist,c_x,c_y,c_z,u_x,u_y,u_z);
       glutPostRedisplay();
       break;
     case 'Z':
       dist=-0.1;
       look_ortho(data,e_x,e_y,e_z+dist,c_x,c_y,c_z,u_x,u_y,u_z);
       glutPostRedisplay();
       break;
     case 'u':
       rotate_arbit_axis(data,object_x,object_y,object_z,object_x+0.1,object_y,object_z,10*M_PI/180);
       glutPostRedisplay();
       break;
     case 'U':
       rotate_arbit_axis(data,object_x,object_y,object_z,object_x+0.1,object_y,object_z,-10*M_PI/180);
       glutPostRedisplay();
       break;
     case 'v':
       rotate_arbit_axis(data,object_x,object_y,object_z,object_x,object_y+0.1,object_z,10*M_PI/180);
       glutPostRedisplay();
       break;
     case 'V':
       rotate_arbit_axis(data,object_x,object_y,object_z,object_x,object_y+0.1,object_z,-10*M_PI/180);
       glutPostRedisplay();
       break;
     case 'w':
       rotate_arbit_axis(data,object_x,object_y,object_z,object_x,object_y,object_z+0.1,10*M_PI/180);
       glutPostRedisplay();
       break;
     case 'W':
       rotate_arbit_axis(data,object_x,object_y,object_z,object_x,object_y,object_z+0.1,-10*M_PI/180);
       glutPostRedisplay();
       break;
     case 'g':
       flag_lookat++;
       break;
     default:
       glutPostRedisplay();
       break;
       //glutPostRedisplay();
   }
}

void mouse_click(int button,int state,int x,int y)
{
  if(state=GLUT_DOWN)
  {
    if(button==GLUT_LEFT_BUTTON)
    {
      c_x=(((float)x/(float)250)-1)*3;
      c_y=(((float)y/(float)250)-1)*3;
      //look_ortho(data,e_x,e_y,e_z,c_x,c_y,c_z,u_x,u_y,u_z);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(e_x,e_y,e_z,c_x,c_y,c_z,u_x,u_y,u_z);
      glutPostRedisplay();
    }
    
  }
}
void mouse_move(x,y)
{
  if(flag_lookat%2)
  {
    e_x=(((float)x/(float)250)-1)*3;
    e_y=(((float)y/(float)250)-1)*3;
    //look_ortho(data,e_x,e_y,e_z,c_x,c_y,c_z,u_x,u_y,u_z);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(e_x,e_y,e_z,c_x,c_y,c_z,u_x,u_y,u_z);
    glutPostRedisplay();
  }
}

void main (int argc, char **argv)
{

flag_lookat=0;

  object_x=0.0;
object_y=0.0;
object_z=0.0;

e_x=0.0;
e_y=0.0;
e_z=0.0;

c_x=0.0;
c_y=0.0;
c_z=-1.0;

u_x=0.0;
u_y=1.0;
u_z=0.0;
dist=0.0;
  data=load_ifs_file("teapot.ifs");
    
  
  FILE *fp;
  fp=fopen("summary.txt", "w");  
  //print_ifs_summary(fp, data);

  printf("					--------------------------------- KEYS FOR OPERATING---------------------------------\n\n");
  
  printf("-----------------TRANSFORMATIONS WRT WORLD COORDINATE SYSTEM---------               ---------------TRANSFORMATIONS WRT OBJECT COORDINATE SYSTEM----------------\n");
  printf("    KEY                                         ACTION                                     KEY                                ACTION\n\n");
  printf("    1                     Translate the object in positive X              	n	      Scale up the object around the object Coordinate System	\n");
  printf("    Shift + 1             Translate the object in negative X  			m	      Scale down the object around the object Coordinate System\n");
  printf("    2                     Translate the object in positive Y  			u		Rotate object around its u axis anticlockwise\n");
  printf("    Shift + 2             Translate the object in negative Y  		Shift + u		Rotate object around its u axis clickwise\n");
  printf("    3                     Translate the object in positive Z  			v		Rotate object around its w axis anticlockwise\n");
  printf("    Shift + 3             Translate the object in negative Z  		Shift + v		Rotate object around its w axis clockwise\n");
  printf("    4             	  Rotate the object around X axis     			w		Rotate object around its w axis anticlockwise\n");
  printf("    5                     Rotate the object around Y axis     		Shift + w		Rotate object around its w axis clockwise\n");
  printf("    6                     Rotate the object around Z axis     \n");
  printf("    7             	  Scale up the object in  X around world  \n");
  printf("    Shift + 7             Scale Down the object in  X around world  \n");
  printf("    8             	  Scale up the object in  Y around world  \n");
  printf("    Shift + 8            Scale Down the object in  X around world  \n");
  printf("    9             	  Scale up the object in  Z around world  \n");
  printf("    Shift + 9           Scale Down the object in  X around world  \n\n");
  
  printf("----------------------------------------------TO GET THE ORIGINAL TEAPOT PRESS THE KEY o        -------------------------------------\n\n");
  printf("--------------------------------To start and stop the functionality of glulookat PRESS THE KEY G     -------------------------------------\n\n");
  
  printf("--------------------------------------------------TRANSFORMATIONS WRT CAMERA COORDINATE SYSTEM------------------------------------\n");
  printf("          Key                                                                      Action\n");
  printf("          x                                           Translate the camera in positive x direction\n");
  printf("          Shift + 'x'                                 Translate the camera in negative x direction\n");
  printf("          y                                           Translate the camera in positive y direction\n");
  printf("          Shift + 'y'                                 Translate the camera in negative y direction\n");
  printf("          z                                           Translate the camera in positive z direction\n");
  printf("          Shift + 'z'                                 Translate the camera in negative z direction\n");
  printf("          Shift + '+'                                 Zoom the camera In\n");
  printf("          Shift + '-'                                 zoom the camera out\n");
  printf("          i                                          rotate the camera anticlockwise around x\n");
  printf("          shift + 'i'                                rotate the camera clockwise around x\n");
  printf("          j                                          rotate the camera anticlockwise around y\n");
  printf("          shift + 'j'                                rotate the camera clockwise around y\n");
  printf("          k                                          rotate the camera anticlockwise around z\n");
  printf("          shift + 'k'                                rotate the camera clockwise around z\n");
  
  
  
  
  
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGB | GLUT_SINGLE );
  glutInitWindowSize (SIZE, SIZE);
  glutInitWindowPosition (50, 100);
  glutCreateWindow ("Teapot");

  init();
  //Register the GLUT callbacks
  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  glutPassiveMotionFunc(mouse_move);
//  glutMouseFunc(mouse_click);
 
  glutMainLoop();
  
}