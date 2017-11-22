#include "glad/glad.h"  //Include order can matter here
#ifdef __APPLE__
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
#endif
#include <cstdio>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int screenWidth = 1280; 
int screenHeight = 1024;  
bool saveOutput = false;
float timePast = 0;

enum cell_type { DOOR, KEY, WALL, START, GOAL, OPEN };
enum walledge {LEFT, RIGHT, FAR, NEAR};
int numTris1, numTris2;
bool key_found = false;
int num_key_found = 0;
int start_h, start_w, end_h, end_w;

float objx=2.0f, objy=0.0f, objz=-2.0f;
float dx = 0.0f, dy = 0.0f, dz = -1.0f;
float step = 0.25f;
bool jump = false;
//float dx = 0.2f, dy = 0.0f, dz = -1.0f;

bool DEBUG_ON = true;
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
bool fullscreen = false;
bool overview = false;
void Win2PPM(int width, int height);

GLint uniModel;	// = glGetUniformLocation(shaderProgram, "model");
GLint uniView;	// = glGetUniformLocation(shaderProgram, "view");
GLint uniProj; 	// = glGetUniformLocation(shaderProgram, "proj");
GLint uniTextureId;

//enum maplayout {TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT, }

cell_type map[5][5];
int map_location_z[5];
int map_location_x[5];

void init_map() {

	int width, height;
	char c;
	ifstream mapFile;
	mapFile.open("map.txt");
	mapFile >> width >> height;

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j< 5 ; j++) {
			mapFile >> c;
			if (c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e')
				map[i][j] = KEY;
			else if (c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E')	
				map[i][j] = DOOR;
			else if (c == 'W')	
				map[i][j] = WALL;		
			else if (c == 'S')	{
				map[i][j] = START;	
				start_w = i;
				start_h = j;		
			}
			else if (c == 'G')	{
				map[i][j] = GOAL;		
				end_w = i;
				end_h = j;
			}
			else if (c == 'O')	
				map[i][j] = OPEN;			
		}
	}

/*
	map[0][0] = OPEN;
	map[0][1] = OPEN;
	map[0][2] = KEY;
	map[0][3] = WALL;
	map[0][4] = OPEN;

	map[1][0] = OPEN;
	map[1][1] = WALL;
	map[1][2] = WALL;
	map[1][3] = WALL;
	map[1][4] = OPEN;

	map[2][0] = OPEN;
	map[2][1] = OPEN;
	map[2][2] = DOOR;
	map[2][3] = OPEN;
	map[2][4] = OPEN;

	map[3][0] = OPEN;
	map[3][1] = WALL;
	map[3][2] = WALL;
	map[3][3] = WALL;
	map[3][4] = OPEN;

	map[4][0] = OPEN;
	map[4][1] = WALL;
	map[4][2] = OPEN;
	map[4][3] = OPEN;
	map[4][4] = OPEN;
*/	

	map_location_z[0] = 4;
	map_location_z[1] = 8;
	map_location_z[2] = 12;
	map_location_z[3] = 16;
	map_location_z[4] = 20;

	map_location_x[0] = 3;
	map_location_x[1] = 7;
	map_location_x[2] = 11;
	map_location_x[3] = 15;
	map_location_x[4] = 19;	
}

int cell(float x) {
	float floor_x = floor(x);
	int cell_x = 0;

	if (floor_x < 1)
		return -1;

	if (floor_x < 3)
		return 0;
	else if (floor_x < 7)
		return 1;
	else if (floor_x < 11)
		return 2;
	else if (floor_x < 15)
		return 3;
	else if (floor_x < 19)
		return 4;

	return -1;

}


bool collision(bool up) {

	//return false;

	float t_objx, t_objz;

	if (up) {
		t_objx = objx + dx*step;
		t_objz = objz + dz*step;
	} else {
		t_objx = objx - dx*step;
		t_objz = objz - dz*step;
	}

	if (cell(-1*t_objz) == -1 || cell(t_objx) == -1) {
		printf("Return -1 \n");
		return true;
	} if (map_location_z[cell(-1*objz)] != map_location_z[cell(-1*t_objz)]) {
		if (map[cell(objx)][cell(-1*t_objz)] == WALL || map[cell(objx)][cell(-1*t_objz)] == DOOR)
			return true;
	} else if (map_location_x[cell(objx)] != map_location_x[cell(t_objx)]) {
		if (map[cell(t_objx)][cell(-1*objz)] == WALL || map[cell(t_objx)][cell(-1*objz)] == DOOR)
			return true;
	}
		return false;
}


bool goal_reached() {
	if (cell(objx) == end_w && cell(-1*objz) == end_h)
		return true;
	else 
		return false;
}

void update_key_status(bool found) {
	key_found = found;
}

bool key_status() {

	if (map[cell(objx)][cell(-1*objz)] == KEY) {
		//printf("************* found key cell %d %d\n", cell(objx), cell(-1*objz));
		update_key_status(true);
		num_key_found = num_key_found + 1;
		return true;
	} else {
		return false;
	}
}

void unlock_door() {
	num_key_found = num_key_found - 1;
	// FIXME if (num_key_found == 0)
		update_key_status(false);
}

void remove_door(bool right, int w, int h) {
	unlock_door();
	if (right) {
		map[w+1][h] = OPEN;
	} else  { // left
		map[w][h+1] = OPEN;
	}
}

void remove_key(int w, int h) {
	//printf("|||||||||||||| removing key \n");
	map[w][h] = OPEN;
}

void draw_wall(float x, float y, float z, walledge dir, GLint textureNum) {
	glm::mat4 side;

	side = glm::mat4(); 
	side = glm::translate(side,glm::vec3(x,y,z));	
	
	if (dir == LEFT || dir == RIGHT) {
		side = glm::translate(side,glm::vec3(0,0,-3.0));
		side = glm::scale(side,glm::vec3(.1f,4.0f,4.0f));
	} else {
		side = glm::translate(side,glm::vec3(2,0,-1));
		side = glm::scale(side,glm::vec3(4.0f,4.0f,.1f));	
	}
	glUniform1i(uniTextureId, textureNum); //Set texture ID to use
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(side));
	glDrawArrays(GL_TRIANGLES, 0, numTris1); 	
}


void draw_1wall(int numTris1, float x, float y, float z, walledge dir, int scale, int w, int h, bool unlock) {

	glm::mat4 side;
	GLint textureNum;
	scale = 1;

	textureNum = 1; // default to wall
	bool unlock_right_door = false;
	bool unlock_top_door = false;

	int t_w, t_h;

	t_w = dir == RIGHT ? w + 1 : w;
	t_h = dir == FAR ? h + 1 : h;

	if (((w > 0 && w < 4) && (h > 0 && h < 4)) || 
		 (w == 0 && (dir == RIGHT || dir == FAR) && h != 4) 	|| 
		 (w == 4 && (dir == FAR) && h != 4)  ||
		 (h == 0 && (dir == RIGHT || dir == FAR) && w != 4) || 
		 (h == 4 && (dir == RIGHT) && w != 4)) {	// Not right edge 
		if (map[t_w][t_h] == WALL) {
			textureNum = 1;
		} else if (map[t_w][t_h] == DOOR) {
			textureNum = 0;
		}
	}


	if (w == cell(objx) && h == cell(-1*objz) && unlock == true) {
		//printf(" ||||||||||||||||| removeing door \n");
		if (map[w+1][h] == DOOR) {	
			remove_door(true, w, h);
			unlock_right_door = true;
		} else if (map[w][h+1] == DOOR) {
			unlock_top_door = true;
			remove_door(false, w, h);
		}
		//remove_door()
	}

	//if (w == end_w && h == end_h)
	if (map[w][h] == GOAL)
		textureNum = 2;

	if (dir == LEFT) {
		draw_wall(4*x,4*y,4*z, LEFT, textureNum);
	} 
	if (dir == RIGHT && unlock_right_door == false) {
		draw_wall(4+4*x,4*y,4*z, RIGHT, textureNum);
	} 
	if (dir == NEAR) {
		draw_wall(4*x,4*y,4*z, NEAR, textureNum);
	}
	if (dir == FAR && unlock_top_door == false) {
		draw_wall(4*x,4*y,-4+4*z, FAR, textureNum);
	}
}

void draw_key( int w, int h) {

	glm::mat4 side;

	float x = 4*w + 1.5;
	float z = 4*h + 1.5;

	//printf("KEY\n");

		// Left side / right side
		side = glm::mat4(); 
		
		//side = glm::translate(side,glm::vec3(1.6 * (w+1), 0, -2.3 * (h+1)));
		side = glm::translate(side,glm::vec3(x, 0, -z-3));
		side = glm::scale(side,glm::vec3(.1f, .1f, .05f));
		side = glm::rotate(side,timePast * .5f * 3.14f/4,glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(side));
		glDrawArrays(GL_TRIANGLES, 0, numTris2);  

	// FIXME texture for wall and door based on cell
}

void draw_cube() {

	glm::mat4 side;
	float D;

	D = 50;

	side = glm::mat4(); 	
	side = glm::translate(side,glm::vec3(0,0,0));
	side = glm::scale(side,glm::vec3(D,D,D));	
	glUniform1i(uniTextureId, 2); 
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(side));
	glDrawArrays(GL_TRIANGLES, 0, numTris1); 
}


void draw_ground() {

	draw_cube();

	glm::mat4 side;

	side = glm::mat4(); 	
	side = glm::translate(side,glm::vec3(10,-2,-11));
	side = glm::scale(side,glm::vec3(20.0f,0.1f,20.0f));	
	glUniform1i(uniTextureId, 1); 
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(side));
	glDrawArrays(GL_TRIANGLES, 0, numTris1); 
}

void draw_wall_right (int w, int h, int scale, bool unlock) {
	draw_1wall(numTris1,w,0,-h,RIGHT, scale,  w, h, unlock);  
}

void draw_wall_top (int w, int h, int scale, bool unlock) {
	draw_1wall(numTris1,w,0,-h,FAR, scale,  w, h, unlock); 
}

void draw_wall_left (int w, int h, int scale, bool unlock) {
	draw_1wall(numTris1,w,0,-h,LEFT, scale,  w, h, unlock);  
}

void draw_wall_bottom (int w, int h, int scale, bool unlock) {
	draw_1wall(numTris1,w,0,-h,NEAR, scale, w, h, unlock); 
}

void draw_cell (int numTris1, int w, int h, int scale, bool unlock) {

	// enum cell_type { DOOR, KEY, WALL, START, GOAL, OPEN };
				
	if(map[w][h] == KEY) { // || map[w][h] == GOAL || map[w][h] == START) { 
		draw_key(w, h);
	}

	// Inner cells
	if (w>0 && w<4 && h>0 && h<4) {
		// RIGHT
		switch (map[w+1][h]) {
			case WALL : case DOOR : 
			{
				draw_wall_right(w,h,scale,unlock); 
			}
			case KEY : case GOAL : case OPEN : case START : 
			;
		}
		// TOP
		switch (map[w][h+1]) {
			case WALL : case DOOR : 
			{
				draw_wall_top(w,h,scale,unlock); 
			}
			case KEY : case GOAL : case OPEN : case START : 
			;
		}
	}


	if (w==0) {
		draw_wall_left(w,h,scale,unlock);
		if (h==0)
			draw_wall_bottom(w,h,scale,unlock);
		if (h==4)
			draw_wall_top(w,h,scale,unlock);

		// RIGHT
		switch (map[w+1][h]) {
				case WALL : case DOOR : 
			{
				draw_wall_right(w,h,scale,unlock); 
			}
			case KEY : case GOAL : case OPEN : case START : 
			;
		}
		// TOP
		switch (map[w][h+1]) {
			case WALL : case DOOR : 
			{
				draw_wall_top(w,h,scale,unlock); 
			}
			case KEY : case GOAL : case OPEN : case START : 
			;
		}
	} else if (w == 4) {
		draw_wall_right(w,h,scale,unlock);
		if (h==0)
			draw_wall_bottom(w,h,scale,unlock);
		if (h==4)
			draw_wall_top(w,h,scale,unlock);

		// TOP
		switch (map[w][h+1]) {
			case WALL : case DOOR : 
			{
				draw_wall_top(w,h,scale,unlock); 
			}
			case KEY : case GOAL : case OPEN : case START : 
			;
		}
	} else if (w > 0 && w < 4 && h == 0) {	// bottom edge
		draw_wall_bottom(w,h,scale, unlock);

		// RIGHT
		switch (map[w+1][h]) {
				case WALL : case DOOR : 
			{
				draw_wall_right(w,h,scale,unlock); 
			}
			case KEY : case GOAL : case OPEN : case START : 
			;
		}
		// TOP
		switch (map[w][h+1]) {
			case WALL : case DOOR : 
			{
				draw_wall_top(w,h,scale,unlock); 
			}
			case KEY : case GOAL : case OPEN : case START : 
			;
		}
	} else if (w > 0 && w < 4 && h == 4) {	// bottom edge
		draw_wall_top(w,h,scale,unlock);

		// RIGHT
		switch (map[w+1][h]) {
				case WALL : case DOOR : 
			{
				draw_wall_right(w,h,scale,unlock); 
			}
			case KEY : case GOAL : case OPEN : case START : 
			;
		}
	}
}

int main(int argc, char *argv[]){

	init_map();

	// set camera to start position
	objx = 4*start_w + 1.5;
	objz = -(4*start_h + 1.5);
	objy = 0.0f;	
	//objx = objz = 0.0f;

    SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
    
    //Ask SDL to get a recent version of OpenGL (3.2 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	
	SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	float aspect = screenWidth/(float)screenHeight; //aspect ratio (needs to be updated if the window is resized)
  
	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);
	
	//Load OpenGL extentions with GLAD
	if (gladLoadGLLoader(SDL_GL_GetProcAddress)){
		printf("\nOpenGL loaded\n");
		printf("Vendor:   %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version:  %s\n\n", glGetString(GL_VERSION));
	}
	else {
		printf("ERROR: Failed to initialize OpenGL context.\n");
		return -1;
	}
	
	/***********************************************************************************/
	//Load Model 1
	/***********************************************************************************/
	ifstream modelFile;
	modelFile.open("models/cube.txt");
	int numLines = 0;
	modelFile >> numLines;
	float* model1 = new float[numLines];
	for (int i = 0; i < numLines; i++){
		modelFile >> model1[i];
	}
	printf("%d\n",numLines);
	numTris1 = numLines/8;
	modelFile.close();

	/***********************************************************************************/	
	//Load Model 2
	/***********************************************************************************/
	modelFile.open("models/mykey.txt");
	numLines = 0;
	modelFile >> numLines;
	float* model2 = new float[numLines];
	for (int i = 0; i < numLines; i++){
		modelFile >> model2[i];
	}
	printf("%d\n",numLines);
	numTris2 = numLines/8;
	modelFile.close();
	

	/***********************************************************************************/	
	//SJG: I load each model in a different array, then concatenate everything in one big array
	//     There are better options, but this works.
	//Concatenate model arrays
	/***********************************************************************************/	
	float* modelData = new float[(numTris1+numTris2)*8];
	copy(model1, model1+numTris1*8, modelData);
	copy(model2, model2+numTris2*8, modelData+numTris1*8);
	int totalNumTris = numTris1+numTris2;
	
	/***********************************************************************************/	
	//Load texture
	/***********************************************************************************/	
	SDL_Surface* surface = SDL_LoadBMP("gate.bmp");
	if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }

    GLuint tex0;
    glGenTextures(1, &tex0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    
    
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);   
    SDL_FreeSurface(surface);


	/***********************************************************************************/	
	//Load texture
	/***********************************************************************************/	
    SDL_Surface* surface2 = SDL_LoadBMP("steel2.bmp");
    if (surface2==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;}
    GLuint tex1;
    glGenTextures(1, &tex1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);

    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface2->w,surface2->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface2->pixels);


    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface2);
	
	/***********************************************************************************/	
	//Load texture
	/***********************************************************************************/	
    SDL_Surface* surface3 = SDL_LoadBMP("sky1.bmp");
    if (surface3==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;}
    GLuint tex2;
    glGenTextures(1, &tex2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex2);

    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface3->w,surface3->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface3->pixels);

    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface3);


	/***********************************************************************************/	
	//Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
	/***********************************************************************************/	
	GLuint vao;
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context

	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	GLuint vbo[1];
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	//GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
	//GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used
	glBufferData(GL_ARRAY_BUFFER, totalNumTris*8*sizeof(float), modelData, GL_STATIC_DRAW);

	/***********************************************************************************/
	//Tell OpenGL how to map shader inputs to data
	/***********************************************************************************/
	int shaderProgram = InitShader("vertexTex.glsl", "fragmentTex.glsl");
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
		//Attribute, vals/attrib., type, normalized?, stride, offset
		//Binds to VBO current GL_ARRAY_BUFFER 
	glEnableVertexAttribArray(posAttrib);

    glUniform1i(glGetUniformLocation(shaderProgram, "tex0"), 0); 
    glUniform1i(glGetUniformLocation(shaderProgram, "tex1"), 1);     
    glUniform1i(glGetUniformLocation(shaderProgram, "tex2"), 2); 

	//GLint colAttrib = glGetAttribLocation(shaderProgram, "inColor");
	//glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	//glEnableVertexAttribArray(colAttrib);

	GLint texAttrib = glGetAttribLocation(shaderProgram, "inTexcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

	GLint normAttrib = glGetAttribLocation(shaderProgram, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(normAttrib);

	uniModel = glGetUniformLocation(shaderProgram, "model");
	uniView = glGetUniformLocation(shaderProgram, "view");
	uniProj = glGetUniformLocation(shaderProgram, "proj");
    uniTextureId = glGetUniformLocation(shaderProgram, "textureId");

	glBindVertexArray(0); //Unbind the VAO in case we want to create a new one

	glEnable(GL_DEPTH_TEST);  

	float rotate_angle=0;

	//Event Loop (Loop forever processing each event as fast as possible)
	SDL_Event windowEvent;
	bool quit = false;
	bool unlock = false;
	float jump_objy_lookat; // = 0.0f;
	while (!quit){
		unlock = false;
		//jump = false;
		while (SDL_PollEvent(&windowEvent)){  //inspect all events in the queue
			if (windowEvent.type == SDL_QUIT) quit = true;
			//List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
			//Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) 
			quit = true; //Exit event loop

			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f){ //If "f" is pressed
				fullscreen = !fullscreen;
				SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Toggle fullscreen 
			}
			
			//SJG: Use key input to change the state of the object
			//     We can use the ".mod" flag to see if modifiers such as shift are pressed
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP){ //If "up key" is pressed
				if (!collision(true)) {
					objx += dx*step;
					objz += dz*step;
				}
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN){ //If "down key" is pressed
				if (!collision(false)) {
					objx -= dx*step;
					objz -= dz*step;
				}
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT){ //If "left key" is pressed
				rotate_angle -= step * M_PI/2; 
				dx = sin(rotate_angle);
				dz = -cos(rotate_angle);
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT){ //If "right key" is pressed
				rotate_angle += step * M_PI/2; 
				dx = sin(rotate_angle);
				dz = -cos(rotate_angle);
			}

			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_l) { //If "l" is pressed
				overview = !overview;
			}

			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_u) { //If "u" is pressed to unlock a door
				unlock = key_found;
			}
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_j)
            {
                jump = true;
            }

		}

		if (!saveOutput) timePast = SDL_GetTicks()/1000.f; // Returns an unsigned 32-bit value representing the number of milliseconds since the SDL library initialized. 
		if (saveOutput) timePast += .07; //Fix framerate at 14 FPS

		glUseProgram(shaderProgram); 
	
		// Clear the screen to default color
		glClearColor(.2f, 0.4f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
					
		/***********************************************************************************/		
		// View matrix		
		/***********************************************************************************/			
		bool status;
		status = key_status();		

		if(status) // FIXME
			remove_key(cell(objx), cell(-1*objz));

		glm::mat4 view;

		int scale = 1;

		if (!overview) {

		view = glm::lookAt(
			glm::vec3(objx, objy, objz),
			glm::vec3(objx+dx, 0.0f, (objz+dz)),  	//Look at point
			glm::vec3(0.0f, 1.0f, 0.0f)); 			//Up
		
		} else {

		view = glm::lookAt(
			glm::vec3(2*scale, 40*scale, -2*scale),
			glm::vec3(-1.5,-1*scale,-4.5),  	//Look at point
			glm::vec3(0.0f, 1.0f, 0.0f)); 			//Up
		}

		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
	
		/***********************************************************************************/		
		// Projection matrix		
		/***********************************************************************************/	
		
		glm::mat4 ortho = glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 10.f);	
		glm::mat4 proj = glm::perspective(3.14f/4, aspect, 1.0f, 100.0f); //FOV, aspect, near, far
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
	
		/***********************************************************************************/		
		// Model matrix		
		/***********************************************************************************/	

		glm::mat4 model;
	
		glBindVertexArray(vao);	

		printf("rotate_angle %f \n", rotate_angle);
		printf("objx, objz, objy %f %f %f\n", objx, objz, objy);

		draw_ground();

		for (int w=0; w<5; w++) {			// width
			for (int h=0; h<5; h++) {		// height
				draw_cell(numTris1, w, h, scale, unlock);
			}
		}

		if (goal_reached())
			quit = true;

		if (saveOutput) Win2PPM(screenWidth,screenHeight);
		SDL_GL_SwapWindow(window); //Double buffering
	}
	
	//Clean Up
	glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile)
{
	FILE *fp;
	long length;
	char *buffer;

	// open the file containing the text of the shader code
	fp = fopen(shaderFile, "r");

	// check for errors in opening the file
	if (fp == NULL) {
		printf("can't open shader source file %s\n", shaderFile);
		return NULL;
	}

	// determine the file size
	fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
	length = ftell(fp);  // return the value of the current position

	// allocate a buffer with the indicated number of bytes, plus one
	buffer = new char[length + 1];

	// read the appropriate number of bytes from the file
	fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
	fread(buffer, 1, length, fp); // read all of the bytes

	// append a NULL character to indicate the end of the string
	buffer[length] = '\0';

	// close the file
	fclose(fp);

	// return the string
	return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName)
{
	GLuint vertex_shader, fragment_shader;
	GLchar *vs_text, *fs_text;
	GLuint program;

	// check GLSL version
	printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Create shader handlers
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read source code from shader files
	vs_text = readShaderSource(vShaderFileName);
	fs_text = readShaderSource(fShaderFileName);

	// error check
	if (vs_text == NULL) {
		printf("Failed to read from vertex shader file %s\n", vShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("Vertex Shader:\n=====================\n");
		printf("%s\n", vs_text);
		printf("=====================\n\n");
	}
	if (fs_text == NULL) {
		printf("Failed to read from fragent shader file %s\n", fShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("\nFragment Shader:\n=====================\n");
		printf("%s\n", fs_text);
		printf("=====================\n\n");
	}

	// Load Vertex Shader
	const char *vv = vs_text;
	glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
	glCompileShader(vertex_shader); // Compile shaders
	
	// Check for errors
	GLint  compiled;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		printf("Vertex shader failed to compile:\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}
	
	// Load Fragment Shader
	const char *ff = fs_text;
	glShaderSource(fragment_shader, 1, &ff, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
	
	//Check for Errors
	if (!compiled) {
		printf("Fragment shader failed to compile\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}

	// Create the program
	program = glCreateProgram();

	// Attach shaders to program
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	// Link and set program to use
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		printf("Link failed\n");
	} 

	glUseProgram(program);

	return program;
}

void Win2PPM(int width, int height){
	char outdir[10] = "out/"; //Must be defined!
	int i,j;
	FILE* fptr;
    static int counter = 0;
    char fname[32];
    unsigned char *image;
    
    /* Allocate our buffer for the image */
    image = (unsigned char *)malloc(3*width*height*sizeof(char));
    if (image == NULL) {
      fprintf(stderr,"ERROR: Failed to allocate memory for image\n");
    }
    
    /* Open the file */
    sprintf(fname,"%simage_%04d.ppm",outdir,counter);
    if ((fptr = fopen(fname,"w")) == NULL) {
      fprintf(stderr,"ERROR: Failed to open file for window capture\n");
    }
    
    /* Copy the image into our buffer */
    glReadBuffer(GL_BACK);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);
    
    /* Write the PPM file */
    fprintf(fptr,"P6\n%d %d\n255\n",width,height);
    for (j=height-1;j>=0;j--) {
      for (i=0;i<width;i++) {
         fputc(image[3*j*width+3*i+0],fptr);
         fputc(image[3*j*width+3*i+1],fptr);
         fputc(image[3*j*width+3*i+2],fptr);
      }
    }
    
    free(image);
    fclose(fptr);
    counter++;
}