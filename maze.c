

#include <GLUT/glut.h>
#include <stdio.h>	
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdbool.h>
#include<stdlib.h>
#include <time.h>
#include<unistd.h>


bool fullScreen = false;
int windowID;

int LoadTextureRAW (const char * filename, int isWrap, int width, int height, int iTextures);
void FreeTexture();
void bitmap_output(int x,int y, void *font, char *s);

const int numTextures = 3;					// total texture images
const int ww = 1000, hh = 670;
float angle = 0.0;
float blend_color[] = {1.0, 1.0, 1.0, 1.0};

GLuint	texture[numTextures];				// Storage Textures
GLUquadricObj *pObj;		// Get a new Quadric off the stack

int i;
bool collide=0;
const GLfloat bounceback=2.0f;

float x=0.0f,y=1.75f,z=5.0f;
float lx=0.0f,ly=0.0f,lz=-1.0f;

float deltaAngle = 0.0,ratio;
float deltaVAngle = 0.0;

float deltaMove = 0.0;

float udspd=1;
float lrspd=0.1;

int fd; 

GLfloat brick_ambdiff[] = { 0.6, 0.4, 0.2, 1.0 };



void changeSize(int w, int h)
{
    
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;
    
	ratio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
    
	// Set the clipping volume
	gluPerspective(45,ratio,1,10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, x + lx,y + ly,z + lz,
			  0.0f,1.0f,0.0f);
    
    
}

//Drawing wood wall
void draw_side(float x1, float y1, float y2, float z1, float z2){

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);	
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    
    
    for(i=0;i<5;i++){
      
    glTexCoord2f(3.0, 0.0); glVertex3f(x1, y1, z2);
    glTexCoord2f(0.0, 0.0); glVertex3f(x1, y1, z1);
    glTexCoord2f(0.0, 9.0); glVertex3f(x1, y2, z1);
    glTexCoord2f(3.0, 9.0); glVertex3f(x1, y2, z2);
  
    
    }
  
    glEnd();
    glDisable(GL_TEXTURE_2D);	
    glPopMatrix();
    
  
    
}

//This function draws the brick wall 
void draw_wall(float x1, float x2, float y1, float y2,float z1)

{
    
    
    glPushMatrix();
    // glRotatef(90, 0, 0, 1);
    glEnable(GL_TEXTURE_2D);	
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, brick_ambdiff);
    glMaterialf(GL_FRONT, GL_SHININESS, 0);
    
  
    
    //Brick
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    for(i=0;i<5;i++){  
    glTexCoord2f(0.0, 0.0); glVertex3f(x2, y2, z1);
    glTexCoord2f(0.0, 2.0); glVertex3f(x1, y2, z1);
    glTexCoord2f(2.0, 2.0); glVertex3f(x1, y1, z1);
    glTexCoord2f(2.0, 0.0); glVertex3f(x2, y1, z1);

    //pastes five walls in one iteration to create the prospective distance of the wall
      }
    
    
    glEnd();			
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    
 }


// list all texture images we want to use in the program here
void LoadGLTextures()
{

	LoadTextureRAW("marble1.raw", true, 128, 128, 0);//texture on the floor X and Z axis
 
	LoadTextureRAW("brick.raw", true, 256,256, 1);//texture used along X axis
    
	LoadTextureRAW("wood.raw", true, 512, 256, 2);//texture used along the Z axis
    
    
    }
void init()//putting everything in place such as lights, camera, and rendering colors and depth (App-level)
{
    pObj = gluNewQuadric();

       
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 2.0, 2.0, 2.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };

	LoadGLTextures();
	
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);		
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    
	glEnable(GL_DEPTH_TEST);						
	glDepthFunc(GL_LESS);								// The Type Of Depth Test To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
    
	gluQuadricTexture(pObj, true);		// This turns on texture coordinates for our Quadrics
}


void orientMe(float ang) { //initial setup for camera perspective
    
    
	lx = sin(ang);
	lz = -cos(ang);
	glLoadIdentity();
	gluLookAt(x, y, z, x + lx,y + ly +deltaVAngle,z + lz,0.0f,1.0f,0.0f);
}


void moveMeFlat(float i) {//this function setup the camera perspective while moving
	
    x = x + i*(lx)*0.1;
	z = z + i*(lz)*0.1;
 
	glLoadIdentity();
	gluLookAt(x, y, z, x + lx,y + ly+deltaVAngle,z + lz, 0.0f,1.0f,0.0f);
}


void reshape (int w, int h)// the function reshapes the object according to the camera adjusted at the current position
{

	if(h == 0)
		h = 1;
	if(y<=1)
		y==1;
    
	ratio = 1.0f * w / h;
    
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    
	gluPerspective(45,ratio,1,10000);
	glMatrixMode(GL_MODELVIEW);
	
    glLoadIdentity();
	gluLookAt(x, y, z, x + lx,y + ly,z + lz, 0.0f,1.0f,0.0f);
}

void display(void)
{
    srand(time(NULL));
    static float rot = 0.0;
	rot += 0.5;
    GLfloat mat_noemis[] = { 0.0, rand()%256,rand()%256, rand()%256};//random colors on marble floor and  the wire sphere 

	//GLfloat mat_red[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat mat_white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
    
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();
    
    
	moveMeFlat(deltaMove);//call this function in order to move camera position
    
	angle += deltaAngle;
	orientMe(angle);
    
	glRotatef(angle, 0, 1, 0);
	glPushMatrix();
    
 
    //sending coordinates to the function to draw
    
    float xx1=0,xx2=120,yy1=-5,yy2=5,zz=0,zz2=100;
    int i,j;

    
    for (i=0; i<3; i++) {
        
        draw_wall(xx1,xx2,yy1,yy2,zz);//draw 3 brick walls from the top
        draw_side(xx2,yy1,yy2,zz,zz2); //draw 3 wood walls from left  
       
        xx1+=20;
        zz+=20;
        draw_wall(xx1,xx2,yy1,yy2,zz2); //draw 3 brick walls from bottom
        draw_side(xx1,yy1,yy2,zz,zz2); //draw 3 wood walls from right   
        
        xx2-=20;    
        zz2-=20;
    
    }
    

    
    // marble floor that covers the entire area of the maze
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);	
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_white);
    glMaterialf(GL_FRONT, GL_SHININESS, 100);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.0, 0.0); glVertex3f(120, -5.0, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.0, -5.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.0, -5.0, 100.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(120, -5.0, 100.0);
       
    glEnd();
    glDisable(GL_TEXTURE_2D);	
    glPopMatrix();
    
 
    
    
    //wire sphere : position of the end of this maze
    glPushMatrix();
    glTranslatef(70,1.75,50);
    glRotatef(rot,50 ,1,0);	// rotate around itself			
    glRotatef(90,1,0,0);	// rotate so that the earth equation is along x-axis
    glRotatef(90,0,0,1);	// rotate so that the earth equation is along x-axis
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1, 1, 1);
     
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_noemis);   
   // glutWireSphere(2, 20, 16);
  							// Disable Texture Mapping
     glutSolidSphere(1.0, 20, 20);
    
    glPopMatrix();
 
 
  
   
    
	glutSwapBuffers();
    
    

}

void idle() //This function checks for collision and reposition of the camera
{
	float time;
    time = clock();// clock function is included to count the time used in solving the maze.
    
    //collision check
    if ((x>0 && x<120)&& z<5)//brick wall start(top)
    {z=5;}
       
    if((x>20 && x<100)&& (z>15&&z<24)) // brick wall 2 outside
    {z=15;}
    if((x>20 && x<100)&& z<25&&z>20) // brick wall 2 inside
    {z=25;}
    if((x>40&&x<80)&&(z>28&&z<30)){z=27;}//brick wall 3 outside
    if((x>40 && x<80)&& (z<30&&z>28)){z=30;}
    
    if((x>70&&x<90)&& z<49&&z>45){z=49;} //brick wall 4 outside
    if((x>40 && x<100)&& z<79 && z>75){z=79;}//brick wall 5 outside
    if((x>40 && x<100)&& z>61 && z<77){z=61;} //brick wall 5 inside   
    
    if((x>20&&x<120)&& z>89)//brick wall 6
    {z=89;}
    

    if((z<100 && z>0) && x>115) //wood wall 1(from left)
    {x=115;}

    if((z>20&&z<75) && (x<110 && x>100)){x=110;}//wood wall 2(from left)
    if((z>20 &&z<75)&& (x>92&&x<108)){x=92;}
    if((z>30 && z<50)&&(x<89&&x>86)){x=89;}//wood wall 3 outside
    if((z>30 && z<50)&&(x>86&&x<88)){x=86;}//wood wall 3 inside
    
    if((z>38 &&z<80)&& (x>40&&x<45)){x=40;}//wood wall 4 outside
    if((z>38&&z<80)&&(x<60&&x>41)){x=60;}
    if((z>20&&z<100)&& x<31) {x=31;}//wood wall 5 from left
       
    
    float timer = time/CLOCKS_PER_SEC;// in order to get the time in second we have to divide the obtained value with CLOCKS_PER_SEC which is included in the header time.h
    
    int t;//since the value pass over to the pipe is assigned as integer, we need a buffer value as integer to pass back to the pipe.
    //check win
    if((z>30 &&z<40)&& x>80 && x<85){
        
      
        printf("You found the exit !!!!\n Your score after collided is %d secs\n ",(int)timer);
        t= (int) timer;
        //print the score as number of collision
        write(fd,&t, sizeof(int));	//write the score to fd which is pipe
  
        sleep(2); exit(0);}
    
    
 
   // printf("[x]=%f [z]=%f\n",x,z);   
    
	
    glutPostRedisplay();
}

//controlling the game using the arrow bar

void pressKey(int key, int x, int y) {
    
	switch (key) {
		case GLUT_KEY_LEFT : //move left
			deltaAngle = -lrspd;break; //position of the camera also turns left with the angle
		case GLUT_KEY_RIGHT : //move right
			deltaAngle = lrspd;break;
		case GLUT_KEY_UP : //use UP arrow to move forward
			deltaMove = udspd;break;
		case GLUT_KEY_DOWN : //move backward
			deltaMove = -udspd;break;
			deltaAngle = -lrspd;break;
              
    
	}
}


void keyb(unsigned char key, int x, int y){
    
    
    printf("%d",key );
	switch (key){
		case 27:  //press esc to view fullscreen
            fullScreen = !fullScreen;
            if (fullScreen) {
                glutFullScreen();
            }
            else {
                glutReshapeWindow(640, 480);
                glutPositionWindow(100, 100);
            }
            break;
            
        case 113: //press q to quit
            
            
            glutDestroyWindow(windowID);
            
            exit(0);
            break;
            
            
	}
    
    display();
}

//check key debouncing
void releaseKey(int key, int x, int y) {
    
	switch (key) {
		case GLUT_KEY_LEFT : 
		case GLUT_KEY_RIGHT :
			deltaAngle = 0;break;
		case GLUT_KEY_UP : 
		case GLUT_KEY_DOWN : 
			deltaMove = 0;break;
            
	}
}



int main(int argc, char* argv[])
{
 
 //  sscanf(argv[1], "%d", &fd);	//scan the second argument to fd

 
	glutInit(&argc, argv);//initializing GL window
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//display mode
	glutInitWindowSize (ww, hh);//initialize the window size
	windowID=glutCreateWindow("Maze 3D");//window name 
   
    
	init();
    
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);
    
    glutKeyboardFunc(keyb);
    
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMainLoop();
    
       

 

	return 0;
    
	
    
    
    
}

void bitmap_output(int x,int y, void *font, char *s) 
{
	int len,i;
    
	glRasterPos2f(x,y);
	len = (int) strlen(s);
	for (i = 0; i < len; i++) 
		glutBitmapCharacter(font,s[i]); 
}

int LoadTextureRAW (const char * filename, int isWrap, int width, int height, int iTextures)
{
    char * data;
    FILE * file;
    
    file = fopen( filename, "rb" );				// open texture data
    if ( file == NULL ) 
		return 0;
	data = (char *) malloc(width * height * 3); // allocate buffer
    fread( data, width * height * 3, 1, file ); // read texture data
    fclose(file);
    
    glGenTextures( 1, &texture[iTextures] );	// allocate a texture name
    
    // select our current texture
    glBindTexture( GL_TEXTURE_2D, texture[iTextures] );
    
    // select modulate to mix texture with the color of the object
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    
    // when texture area is small, bilinear filter the closest mipmap
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    isWrap ? GL_REPEAT : GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    isWrap ? GL_REPEAT : GL_CLAMP );
    
    // build our texture mipmaps
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,
                      GL_RGB, GL_UNSIGNED_BYTE, data );
    
    // free buffer
    free( data );
    
    return iTextures;
}

void FreeTexture()
{
	int i;
	for (i=0; i<numTextures; i++)
		glDeleteTextures( 1, &texture[i] );
}