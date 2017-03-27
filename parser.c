#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


/*======== void parse_file () ==========
Inputs:   char * filename 
          struct matrix * transform, 
          struct matrix * pm,
          screen s
Returns: 

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         sphere: add a sphere to the edge matrix - 
	    takes 4 arguemnts (cx, cy, cz, r)
         torus: add a torus to the edge matrix - 
	    takes 5 arguemnts (cx, cy, cz, r1, r2)
         box: add a rectangular prism to the edge matrix - 
	    takes 6 arguemnts (x, y, z, width, height, depth)	    

	 circle: add a circle to the edge matrix - 
	    takes 4 arguments (cx, cy, cz, r)
	    
	 hermite: add a hermite curve to the edge matrix -
	    takes 8 arguments (x0, y0, x1, y1, rx0, ry0, rx1, ry1)
	 bezier: add a bezier curve to the edge matrix -
	    takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
         line: add a line to the edge matrix - 
	    takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
	 ident: set the transform matrix to the identity matrix - 
	 scale: create a scale matrix, 
	    then multiply the transform matrix by the scale matrix - 
	    takes 3 arguments (sx, sy, sz)
	 translate: create a translation matrix, 
	    then multiply the transform matrix by the translation matrix - 
	    takes 3 arguments (tx, ty, tz)
	 rotate: create a rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 2 arguments (axis, theta) axis should be x, y or z
	 apply: apply the current transformation matrix to the 
	    edge matrix
	 display: draw the lines of the edge matrix to the screen
	    display the screen
	 save: draw the lines of the edge matrix to the screen
	    save the screen to a file -
	    takes 1 argument (file name)
	 quit: end parsing

See the file script for an example of the file format

IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
  
void parse_file ( char * filename, 
                  struct matrix * transform, 
                  struct matrix * edges,
                  screen s) {

   color c;
 
  
  c.red = 0;
  c.green = MAX_COLOR;
  c.blue = 0;

  clear_screen(s);
  
  FILE *f;
  char line[256];
  clear_screen(s);
  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");


  int prev = 0;

  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    //printf(":%s:\n",line);
    int skip = 0;
    if(strcmp("line", line) == 0){
      prev = 1;
      skip = 1;
    }
    if(strcmp("ident", line) ==0){
      prev = 2;
      ident(transform);
      skip = 1;
    }
    if(strcmp("scale", line) ==0){
      prev = 3;
      skip = 1;
    }
    if(strcmp("move", line)== 0){
      prev = 4;
      skip = 1;
    }
    if(strcmp("rotate", line) == 0){
      prev = 5;
      skip = 1;
    }
    if(strcmp("apply", line) == 0){
      prev = 6;
      matrix_mult(transform, edges);
      skip = 1;
    }
    if(strcmp("display", line) == 0){
      clear_screen(s);
      draw_lines(edges,s,c);
      display(s);
      skip = 1;
      prev = 7;
    }
    if(strcmp("save", line) == 0){
      skip = 1;
      prev = 8;
    }
    if(strcmp("circle", line) == 0){
      skip = 1;
      prev = 10;
    }
    if(strcmp("hermite", line) == 0){
      skip = 1;
      prev = 11;
    }
    if(strcmp("bezier", line) == 0){
      skip = 1;
      prev = 12;
    }
    if(strcmp("box", line) == 0){
      skip = 1;
      prev = 13;
    }
    if(strcmp("quit", line) == 0){
      skip = 1;
      prev = 9;
      exit(0);
    }
    if(skip != 1){
      if(prev == 1){
	char * a = line;
	int x0 = atoi(strsep(&a, " "));
	int y0 = atoi(strsep(&a, " "));
	int z0 = atoi(strsep(&a, " "));
	int x1 = atoi(strsep(&a, " "));
	int y1 = atoi(strsep(&a, " "));
	int z1 = atoi(strsep(&a, " "));
	add_edge(edges,x0,y0,z0,x1,y1,z1);
      }
      if(prev == 3){
	char * a = line;
	int mx = atoi(strsep(&a, " "));
	int my = atoi(strsep(&a, " "));
	int mz = atoi(strsep(&a, " "));
	matrix_mult(make_scale(mx,my,mz), transform);

      }
      if(prev == 4){
	char * a = line;
	int tx = atoi(strsep(&a, " "));
	int ty = atoi(strsep(&a, " "));
	int tz = atoi(strsep(&a, " "));
	matrix_mult(make_translate(tx,ty,tz), transform);
      }
      if(prev == 5){
	char * a = line;
	char * which= strsep(&a, " ");
	int angle = atoi(strsep(&a, " "));
	if(strcmp(which, "x") == 0){
	  matrix_mult( make_rotX(angle), transform);
	}
	if(strcmp(which, "y") == 0){
	  matrix_mult( make_rotY(angle), transform);
	}
	if(strcmp(which, "z") == 0){
	  matrix_mult( make_rotZ(angle), transform);
	}
      }
      if(prev == 8){
	draw_lines(edges,s,c);
	char * a = line;
	char * name= strsep(&a, " ");
	save_extension(s, name);
      }
      if(prev == 10){
	char * a = line;
	int cx = atoi(strsep(&a, " "));
	int cy = atoi(strsep(&a, " "));
	int cz = atoi(strsep(&a, " "));
	int r = atoi(strsep(&a, " "));
	add_circle(edges,cx,cy,cz,r,0);
      }
      if(prev == 11){
	char * a = line;
	int x0 = atoi(strsep(&a, " "));
	int y0 = atoi(strsep(&a, " "));
	int x1 = atoi(strsep(&a, " "));
	int y1 = atoi(strsep(&a, " "));
	int rx0 = atoi(strsep(&a, " "));
	int ry0 = atoi(strsep(&a, " "));
	int rx1 = atoi(strsep(&a, " "));
	int ry1 = atoi(strsep(&a, " "));
	add_curve(edges,x0,y0,x1,y1,rx0,ry0,rx1,ry1,0,1);
      }
      if(prev == 12){
	char * a = line;
	int x0 = atoi(strsep(&a, " "));
	int y0 = atoi(strsep(&a, " "));
	int x1 = atoi(strsep(&a, " "));
	int y1 = atoi(strsep(&a, " "));
	int x2 = atoi(strsep(&a, " "));
	int y2 = atoi(strsep(&a, " "));
	int x3 = atoi(strsep(&a, " "));
	int y3 = atoi(strsep(&a, " "));
	add_curve(edges,x0,y0,x1,y1,x2,y2,x3,y3,0,0);
      }
      if(prev == 13){
	char * a = line;
	int x = atoi(strsep(&a, " "));
	int y = atoi(strsep(&a, " "));
	int z = atoi(strsep(&a, " "));
	int w = atoi(strsep(&a, " "));
	int h = atoi(strsep(&a, " "));
	int d = atoi(strsep(&a, " "));
	add_box(edges,x,y,z,w,h,d);
      }
    }
  }
}
