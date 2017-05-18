/* Program 4
 * Santi Pierini
*/
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"

#include <algorithm>
#define NUM_TREES 50
#define NUM_COINS 10
#define NUM_HELI 30


GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
shared_ptr<Program> prog;

shared_ptr<Shape> shape;
shared_ptr<Shape> bunnyshape;
shared_ptr<Shape> floorShape;
shared_ptr<Shape> cone;
shared_ptr<Shape> raptor;
shared_ptr<Shape> tank;
shared_ptr<Shape> coin;
shared_ptr<Shape> heli;
shared_ptr<Shape> rotors;
shared_ptr<Shape> spiral;



int g_width, g_height;
float lightPos;
int currMaterial;
float theta, phi;
float throttle;
float addColor;
float planeSquish;
vec3 Eye;
vec3 LA;
vec3 w;
vec3 u;
vec3 v;

bool wFlag = false, sFlag = false;
bool aFlag = false, dFlag = false;
bool upFlag = true, downFlag = true;//stop forward button after crash, stop side after crash

int treeCol[NUM_TREES];
vec3 treeArr[NUM_TREES];
vec3 coinArr[NUM_COINS];
vec2 heliRot[NUM_HELI];//x is theta, y is motion flag
vec3 heliArr[NUM_HELI];
vec3 spherePos;

static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

float randFloat(float l, float h)
{
    float r = rand() / (float)RAND_MAX;
    return (1.0f - r) * l + r * h;
}

static void genSphere() {
    spherePos.x = randFloat(-2, 2);
    spherePos.y = 2;
    spherePos.z = randFloat(15, 40);


}

static void genHeli() {
    for(int i = 0; i < NUM_HELI; i++) {
        heliArr[i].x = randFloat(-7.f,7.f);
        heliArr[i].y = randFloat(2, 4);;//spawn in upper area
        heliArr[i].z = randFloat(10, 58);//length of board
        
        heliRot[i].x = randFloat(-.36, .36);//theta
        heliRot[i].y = randFloat(0, 10);//motion flag
        
    }
}

static void genTrees() {
    for(int i = 0; i < NUM_TREES; i++) {
        treeArr[i].x = randFloat(-7.f,7.f);
        treeArr[i].y = .1;//middle of mass?
        treeArr[i].z = randFloat(5, 58);//length of board
        treeCol[i] = randFloat(4, 7);//color

    }
}

static void genCoins() {
    for(int i = 0; i < NUM_COINS/5; i++) {
        coinArr[i].x = randFloat(-7.f,7.f);
        coinArr[i].y = randFloat(3, 4.5);
        coinArr[i].z = randFloat(10, 55);//length of board
    }
    
    for(int i = NUM_COINS/5; i < NUM_COINS; i++) {
        coinArr[i].x = randFloat(-7.f,7.f);
        coinArr[i].y = randFloat(-.6, .8);
        coinArr[i].z = randFloat(10, 55);//length of board
    }
}

//returns true if coin array contains one at this position
static bool containsHeli(float x, float y, float z) {
    
    vec3 check = vec3(x, y, z);
    
    for(int i = 0; i < NUM_HELI; i++){
        
        if(distance(vec3(heliArr[i].x, heliArr[i].y+.1, heliArr[i].z+.1), check) < .4) {
            //move coin out of view
            return true;
        }
    }
    return false;
}

//returns true if coin array contains one at this position
static bool containsCoin(float x, float y, float z) {
    
    vec3 check = vec3(x, y, z);
    
    for(int i = 0; i < NUM_COINS; i++){
        
        if(distance(coinArr[i], check) < .5) {
            //move coin out of view
            coinArr[i].z = -20;
            return true;
        }
    }
    return false;
}

//returns true if tree array contains one at this position
static bool containsTree(float x, float y, float z) {
    
    vec3 check = vec3(x, y, z);
    
    for(int i = 0; i < NUM_TREES; i++){
        
        
        //if(treeArr[i].x >= x -.4 && treeArr[i].x <= x+.4 && treeArr[i].z >= z -.4 && treeArr[i].z <= z+.4){
        if(distance(treeArr[i], check) < .7) {
                return true;
        }
    }
    return false;
}


static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
    else if(key == GLFW_KEY_W && action == GLFW_PRESS) {
//        sFlag = false;
//        wFlag = !wFlag;
        
        throttle += .01;
        addColor += .05;
        
    }
//    else if(key == GLFW_KEY_S && action == GLFW_PRESS) {
////        wFlag = false;
////        sFlag = !sFlag;
//        
//        if(throttle >.02)
//            throttle -= .01;
//        addColor -= .05;
//
//        
//    }
    else if(key == GLFW_KEY_A && action == GLFW_PRESS) {
        if(downFlag) {
            dFlag = false;
            aFlag = !aFlag;
        }
        
        

    }
    else if(key == GLFW_KEY_D && action == GLFW_PRESS) {
        if(downFlag) {
            aFlag = false;
            dFlag = !dFlag;
        }
        
        

    }
    else if(key == GLFW_KEY_UP && action == GLFW_PRESS) {
        sFlag = false;
        if(upFlag) {
           wFlag = true;
            upFlag = false;
        }
        
    }
//    else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
//        wFlag = false;
//        sFlag = !sFlag;
//        
////        if(throttle >.02)
////            throttle -= .01;
//        
//    }
    else if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        //stop all motion with spacebar
        wFlag = false;
        aFlag = false;
        dFlag = false;
        upFlag = true;
        downFlag = true;
        
        Eye = vec3(0, 0, 0);
        throttle = .04;
        addColor = .15;
        planeSquish = 0;
        genTrees();
        genCoins();
        genHeli();
        genSphere();

    }
}


static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    float phiOff = ((yoffset/70)/normalize((double)g_height)) * M_PI;
    float thetaOff = ((xoffset/70)/normalize((double)g_width)) * M_PI;
    
    //limit lookAt to avoid parallel upVector
    if((phi + phiOff) < .6 && (phi + phiOff) > -.6)
        phi += phiOff;
    
    if((theta + thetaOff) < 2.5 && (theta + thetaOff) > .6)
        theta += thetaOff;
    
    
//    cout << "Pos X " << theta <<  " Pos Y " << phi << endl;
}

static void resize_callback(GLFWwindow *window, int width, int height) {
   g_width = width;
   g_height = height;
   glViewport(0, 0, width, height);
}

//helper function to set materials
void SetMaterial(int i) {
    
    switch (i) {
        case 0: //engines
            glUniform3f(prog->getUniform("MatAmb"), 0.6, 0.05, 0.0);
            glUniform3f(prog->getUniform("MatDif"), 1, 0.9 - addColor, 0.2 -addColor);
            glUniform3f(prog->getUniform("MatSpec"), 1, 0.7 - addColor, 0.4 - addColor);
            glUniform1f(prog->getUniform("shine"), 50);
            break;
        case 1: // flat grey
            glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
            glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
            glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
            glUniform1f(prog->getUniform("shine"), 4.0);
            break;
        case 2: //brass
            glUniform3f(prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
            glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
            glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
            glUniform1f(prog->getUniform("shine"), 27.9);
            break;
        case 3: //copper
            glUniform3f(prog->getUniform("MatAmb"), 0.1913, 0.0735, 0.0225);
            glUniform3f(prog->getUniform("MatDif"), 0.7038, 0.27048, 0.0828);
            glUniform3f(prog->getUniform("MatSpec"), 0.257, 0.1376, 0.08601);
            glUniform1f(prog->getUniform("shine"), 12.8);
            break;
        case 4: //other tree color
            glUniform3f(prog->getUniform("MatAmb"), 0.0913, 0.1735, 0.0225);
            glUniform3f(prog->getUniform("MatDif"), 0.6038, 0.70048, 0.1828);
            glUniform3f(prog->getUniform("MatSpec"), 0.757, 0.3076, 0.18601);
            glUniform1f(prog->getUniform("shine"), 12.8);
            break;
        case 5: //grass
            glUniform3f(prog->getUniform("MatAmb"), 0.0913, 0.1735, 0.0225);
            glUniform3f(prog->getUniform("MatDif"), 0.2038, 0.77048, 0.0828);
            glUniform3f(prog->getUniform("MatSpec"), 0.057, 0.3376, 0.08601);
            glUniform1f(prog->getUniform("shine"), 12.8);
            break;
        case 6: //red/purple trees
            glUniform3f(prog->getUniform("MatAmb"), 0.1294, 0.0235, 0.02745);
            glUniform3f(prog->getUniform("MatDif"), .901, .631, .208);
            glUniform3f(prog->getUniform("MatSpec"), 0.598, 0, 0.4137);
            glUniform1f(prog->getUniform("shine"), 20);
            break;
        case 7: //plane material
            glUniform3f(prog->getUniform("MatAmb"), 0.117647, 0.117647, 0.117647);
            glUniform3f(prog->getUniform("MatDif"), .71, .71, .71);
            glUniform3f(prog->getUniform("MatSpec"), 0.652941, 0.652941, 0.652941);
            glUniform1f(prog->getUniform("shine"), 8);
            break;
        case 8: //shadow
            glUniform3f(prog->getUniform("MatAmb"), 0.1, 0.2, 0.1);
            glUniform3f(prog->getUniform("MatDif"), 0, 0, 0);
            glUniform3f(prog->getUniform("MatSpec"), 0, 0, 0);
            glUniform1f(prog->getUniform("shine"), 0);
            break;
        case 9: //portal black/blue
            glUniform3f(prog->getUniform("MatAmb"), 0.05, 0.0, 0.20);
            glUniform3f(prog->getUniform("MatDif"), 0.15, 0.15, 0.30);
            glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.6);
            glUniform1f(prog->getUniform("shine"), 50);
            break;
    }
}



static void init()
{
	GLSL::checkVersion();

    lightPos = 0;
    Eye = vec3(0, 0, 0);
    throttle = .04;
    addColor = .15;
    planeSquish = 0;
    
    
	// Set background color.
	glClearColor(.32f, .54f, .76f, 1.0f);
//  glClearColor(0, 0, 0, 1.0f);

	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	// Initialize sphere mesh.
	shape = make_shared<Shape>();
	shape->loadMesh(RESOURCE_DIR + "smoothsphere.obj");
	shape->resize();
	shape->init();
    
    // Initialize cone mesh.
    cone = make_shared<Shape>();
    cone->loadMesh(RESOURCE_DIR + "cone.obj");
    cone->resize();
    cone->init();
    
    // Initialize bunny mesh.
    bunnyshape = make_shared<Shape>();
    bunnyshape->loadMesh(RESOURCE_DIR + "bunny.obj");
    bunnyshape->resize();
    bunnyshape->init();
    
    
    // Initialize cube (floor) mesh.
    floorShape = make_shared<Shape>();
    floorShape->loadMesh(RESOURCE_DIR + "cube.obj");
    floorShape->resize();
    floorShape->init();
    
    // Initialize Airplane mesh.
    raptor = make_shared<Shape>();
    raptor->loadMesh(RESOURCE_DIR + "FA-22_Raptor.obj");
    raptor->resize();
    raptor->init();
    
    // Initialize Car mesh.
    tank = make_shared<Shape>();
    tank->loadMesh(RESOURCE_DIR + "police_car.obj");
    tank->resize();
    tank->init();
    
    // Initialize Coin mesh.
    coin = make_shared<Shape>();
    coin->loadMesh(RESOURCE_DIR + "Coin.obj");
    coin->resize();
    coin->init();
    
    // Initialize Helicopter mesh.
    heli = make_shared<Shape>();
    heli->loadMesh(RESOURCE_DIR + "heli2.obj");
    heli->resize();
    heli->init();
    
    // Initialize Rotor mesh.
    rotors = make_shared<Shape>();
    rotors->loadMesh(RESOURCE_DIR + "rotors.obj");
    rotors->resize();
    rotors->init();

    // Initialize Spiral mesh.
    spiral = make_shared<Shape>();
    spiral->loadMesh(RESOURCE_DIR + "spiral.obj");
    spiral->resize();
    spiral->init();
    
	// Initialize the GLSL program.
	prog = make_shared<Program>();
	prog->setVerbose(true);
	prog->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	prog->init();
	prog->addUniform("P");
	prog->addUniform("MV");
    prog->addUniform("V");
	prog->addAttribute("vertPos");
	prog->addAttribute("vertNor");
    prog->addAttribute("vertTex");

    prog->addUniform("lightPos");
    prog->addUniform("MatAmb");
    prog->addUniform("MatDif");
    prog->addUniform("MatSpec");
    prog->addUniform("shine");
}

static void createSphere(shared_ptr<MatrixStack> MV, mat4 V, float tx, float ty, float tz, float sx, float sy, float sz, int material) {
    
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(tx, ty, tz));
    MV->rotate(glfwGetTime(), vec3(0, 1, 0));
//    MV->rotate(1.5708, vec3(1, 0, 0));//for smoothsphere orientation
    MV->scale(vec3(sx, sy, sz));
    SetMaterial(material);//set material color look
    glUniform1f(prog->getUniform("lightPos"), lightPos);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    shape->draw(prog);
    MV->popMatrix();
    
    //shadow
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(tx, -.78, tz));
    MV->rotate(glfwGetTime(), vec3(0, 1, 0));
    MV->scale(vec3(sx, 0, sz));
    SetMaterial(8);//set material color look
    glUniform1f(prog->getUniform("lightPos"), lightPos);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    shape->draw(prog);
    MV->popMatrix();
    
}

static void createTree(shared_ptr<MatrixStack> MV, mat4 V, float x, float z, float material) {
    //trunk
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(x, -.1, 0 + z));
    MV->scale(vec3(.1, 1, .1));
    SetMaterial(currMaterial+3);//set material color look
    glUniform1f(prog->getUniform("lightPos"), lightPos);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    shape->draw(prog);
    MV->popMatrix();
    //cone
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(x, .5, -.2 + z));
    MV->scale(vec3(.7, .75, .7));
    MV->rotate(1.3, vec3(1, 0, 0));
    SetMaterial(material);//set material color look: std::max((int)z%7, 4)
    glUniform1f(prog->getUniform("lightPos"), lightPos);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    cone->draw(prog);
    MV->popMatrix();
    
    //shadow
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(x, -.78, -.2 + z));
    MV->scale(vec3(1, 0, 1));
    MV->rotate(1.3, vec3(1, 0, 0));
    SetMaterial(currMaterial+8);//set material color look
    glUniform1f(prog->getUniform("lightPos"), lightPos);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    cone->draw(prog);
    MV->popMatrix();
    
}

static void createCoin(shared_ptr<MatrixStack> MV, mat4 V, float x, float y, float z) {
    
    /* draw Coin*/
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(x, y, z));
    MV->rotate(glfwGetTime()*2, vec3(0, 1, 0));
    MV->scale(vec3(.2, .2, .2));
    SetMaterial(currMaterial+2);//set material color look
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    coin->draw(prog);
    MV->popMatrix();
    
    /* draw shadow*/
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(x, -.78, z));
    MV->rotate(glfwGetTime()*2, vec3(0, 1, 0));
    MV->scale(vec3(.2, 0, .2));
    SetMaterial(currMaterial+8);//set material color look
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    coin->draw(prog);
    MV->popMatrix();
    
}


static void createPlanes(shared_ptr<MatrixStack> MV, mat4 V, float x, float z) {
    /* plane model */
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(x, 0, z));//where to orbit around
    MV->rotate(glfwGetTime()*2, vec3(0, 1, 0));//orbit speed
    MV->translate(vec3(3, 3, 0));//size of orbit and verticality
    MV->rotate(1.5708, vec3(-1, 0, 0)); //adjust pitch
    MV->rotate(.8, vec3(0, -1, 0)); //adjust initial tilt inward
    MV->scale(vec3(.5, .5, .5));
    SetMaterial(currMaterial+7);//set material color look
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    raptor->draw(prog);
    MV->popMatrix();
    
    //plane shadow
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(x, 0, z));//where to orbit around
    MV->rotate(glfwGetTime()*2, vec3(0, 1, 0));//orbit speed
    MV->translate(vec3(3, -.78, 0));//size of orbit and verticality
    MV->rotate(1.5708, vec3(-1, 0, 0)); //adjust pitch
    MV->scale(vec3(.5, .5, 0));
    SetMaterial(currMaterial+8);//set material color look
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    raptor->draw(prog);
    MV->popMatrix();
    
    
    /* plane model 2*/
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(x, 2, z));//where to orbit around
    MV->rotate(glfwGetTime(), vec3(0, 0, 1));//orbit speed
    MV->translate(vec3(0, 2.5, 0));//size of orbit
    MV->rotate(1.58, vec3(0, 0, 1)); //adjust initial pitch
    MV->rotate(1.5708, vec3(0, 1, 0)); //adjust direction facing
    MV->scale(vec3(.5, .5, .5));
    SetMaterial(currMaterial+7);//set material color look
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    raptor->draw(prog);
    MV->popMatrix();
}


static void createHeli(shared_ptr<MatrixStack> MV, mat4 V, float x, float y, float z, float theta) {
 
    /* draw helicopter*/
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(x, y, z));
    MV->rotate(theta, vec3(0, 1, 0));
        //rotors
        MV->pushMatrix();
        MV->translate(vec3(.01, .24, -.05));
        MV->rotate(glfwGetTime()*10, vec3(0, 1, 0));
        MV->translate(vec3(-.00005, 0, 0));
        MV->scale(vec3(.8,.8,.8));
        SetMaterial(currMaterial+1);//set material color look
        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
        rotors->draw(prog);
        MV->popMatrix();
    MV->scale(vec3(.8,1,.8));
    SetMaterial(currMaterial+1);//set material color look
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    heli->draw(prog);
    MV->popMatrix();
    
    
    
    /* draw shadow*/
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(x, -.75, z));
    MV->rotate(theta, vec3(0, 1, 0));
    //rotors
    MV->pushMatrix();
    MV->translate(vec3(.01, 0, -.05));
    MV->rotate(glfwGetTime()*10, vec3(0, 1, 0));
    MV->translate(vec3(-.00005, 0, 0));
    MV->scale(vec3(.8,0,.8));
    SetMaterial(currMaterial+8);//set material color look
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    rotors->draw(prog);
    MV->popMatrix();
    MV->scale(vec3(1,0,1));
    SetMaterial(currMaterial+8);//set material color look
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    heli->draw(prog);
    MV->popMatrix();
    
}

static void moveHeli() {
    
    for(int i = 0; i < NUM_HELI; i++) {
        if(heliRot[i].y < 3 ) {
            if(heliArr[i].y + sin(glfwGetTime()*2)/20 >.5 && heliArr[i].y +  sin(glfwGetTime()*2)/20 <=4.5)
                heliArr[i].y += sin(glfwGetTime()*2)/20;
        }
        else if(heliRot[i].y >= 3 && heliRot[i].y < 6) {
            if(heliArr[i].y + sin(glfwGetTime())/40 > .5 && heliArr[i].y + sin(glfwGetTime())/40 <= 4.5)
                heliArr[i].y += sin(glfwGetTime())/40;
        }
        else {
            if(heliArr[i].y + cos(glfwGetTime()/2)/40>.2 && heliArr[i].y + cos(glfwGetTime()/2)/40<=4.5)
                heliArr[i].y += cos(glfwGetTime()/2)/40;
            
        }
    }
    
}


static void render()
{
    float radius = 1.0f;
    
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the matrix stack for Lab 6
   float aspect = width/(float)height;

   // Create the matrix stacks - please leave these alone for now
   auto P = make_shared<MatrixStack>();
   auto MV = make_shared<MatrixStack>();
   mat4 V;
   
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 100.0f);

	// Draw a stack of cubes with indiviudal transforms
	prog->bind();
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    
    //create View matrix. modify lookAt vector
    vec3 gaze;
    vec3 upVector =vec3(0.0, 1.0, 0.0);

    
    //reset position, trees, coins
    if(LA.z > 65) {
        genTrees();
        genCoins();
        genHeli();
        genSphere();
        Eye.z = 0;

    }
    
    //*** add NEW EYE offset to the OLD LOOKAT point***
    //calculate PITCH and YAW
    LA.x = (radius * cos(phi) * cos(theta))            + Eye.x;
    LA.y = (radius * sin(phi))                         + Eye.y;
    LA.z = (radius * cos(phi) * cos((M_PI/2) - theta)) + Eye.z;
    //calculate gaze and camera vectors: w,u,v
    gaze = LA - Eye;
    w = -gaze / length(gaze);
    u = cross(upVector, w)/ length(cross(upVector, w));
    v = cross(w, u);
    //set current view
    V = lookAt(Eye, LA, upVector);
    
    
    
    //(Camera) PLANE along track of +Z direction
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(LA.x, LA.y -.15, LA.z + throttle));//speed could increase LA.z value and speed coeficient??
    //PLANE ORIENTATION
    //if(gaze.z >0)
        MV->rotate(phi*1.2, vec3(-1, 0, 0)); //adjust plane PITCH - x
    MV->rotate((theta-M_PI/2)/3, vec3(0, 0, 1)); //plane ROLL amount - z   (theta-M_PI/2)/6
    MV->rotate(theta, vec3(0, -1, 0)); // plane direction - y
    //PLANE SETUP ROTATIONS
    MV->rotate(1.5708, vec3(0, -1, 0)); //adjust direction
    MV->rotate(1.5708, vec3(-1, 0, 0)); //adjust pitch
    
    if(planeSquish == 0) {
        //engine fire RIGHT
        MV->pushMatrix();
        MV->translate(vec3(.04, -.38, -.0556));//horizontal, forward/back, vertical
        MV->scale(vec3(.032,.032,.032));
        SetMaterial(currMaterial+0);//set material color look
        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
        shape->draw(prog);
        MV->popMatrix();
        //engine fire LEFT
        MV->pushMatrix();
        MV->translate(vec3(-.04, -.38, -.0556));//horizontal, forward/back, vertical
        MV->scale(vec3(.0315,.0315,.0315));
        SetMaterial(currMaterial+0);//set material color look
        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
        shape->draw(prog);
        MV->popMatrix();
    
        //yaw LEFT wing
        MV->pushMatrix();
        MV->translate(vec3(-.1, -.38, 0));//horizontal, forward/back, vertical
        MV->rotate(.48, vec3(0, -1, 0)); //adjust angle
        if(aFlag)
            MV->rotate(-.3, vec3(0, 0, 1)); //reactive yaw
        if(dFlag)
            MV->rotate(.3, vec3(0, 0, 1)); //reactive yaw
        MV->scale(vec3(.002,.01,.05));
        SetMaterial(currMaterial+1);//set material color look
        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
        floorShape->draw(prog);
        MV->popMatrix();
        //yaw RIGHT wing
        MV->pushMatrix();
        MV->translate(vec3(.1, -.38, 0));//horizontal, forward/back, vertical
        MV->rotate(.48, vec3(0, 1, 0)); //adjust pitch
        if(aFlag)
            MV->rotate(-.3, vec3(0, 0, 1)); //reactive yaw
        if(dFlag)
            MV->rotate(.3, vec3(0, 0, 1)); //reactive yaw
        MV->scale(vec3(.002,.01,.05));
        SetMaterial(currMaterial+1);//set material color look
        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
        floorShape->draw(prog);
        MV->popMatrix();
        
        //pitch LEFT wing
        MV->pushMatrix();
        MV->translate(vec3(-.2, -.29, -.055));//horizontal, forward/back, vertical
        MV->rotate(-phi, vec3(1, 0, 0)); //adjust pitch
        MV->rotate(1.55, vec3(0, 1, 0)); //adjust initial position
        MV->scale(vec3(.001,.015,.06));
        SetMaterial(currMaterial+1);//set material color look
        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
        floorShape->draw(prog);
        MV->popMatrix();
        
        //pitch Right wing
        MV->pushMatrix();
        MV->translate(vec3(.2, -.29, -.055));//horizontal, forward/back, vertical
        MV->rotate(-phi, vec3(1, 0, 0)); //adjust pitch
        MV->rotate(1.59, vec3(0, 1, 0)); //adjust initial position
        MV->scale(vec3(.001,.01,.06));
        SetMaterial(currMaterial+1);//set material color look
        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
        floorShape->draw(prog);
        MV->popMatrix();
        
    }
    
    MV->scale(vec3(.5, .5 - planeSquish, .5));
    SetMaterial(currMaterial+7);//set material color look
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    raptor->draw(prog);
    MV->popMatrix();
    //END OF PLANE
    
    //plane shadow
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(LA.x, -.7, LA.z + throttle));
    MV->rotate(theta, vec3(0, -1, 0)); // plane direction - y
    //PLANE SETUP ROTATIONS
    MV->rotate(1.5708, vec3(0, -1, 0)); //adjust direction
    MV->rotate(1.5708, vec3(-1, 0, 0)); //adjust pitch
    MV->scale(vec3(.5, .5 - planeSquish, .5));
    SetMaterial(currMaterial+8);//set material color look
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    raptor->draw(prog);
    MV->popMatrix();
    
    
    //move Heli locations
    moveHeli();
    /* draw Helicopters*/
    for(int i = 0; i < NUM_HELI; i ++) {
        createHeli(MV, V, heliArr[i].x, heliArr[i].y, heliArr[i].z, heliRot[i].x);
    }
    //detect collision for helicopter
    if(containsHeli(LA.x, LA.y, LA.z)) {
        wFlag = false;
        downFlag = false;
        if(planeSquish <= .4)
            planeSquish += .05;
    }
    
    /* draw Coins*/
    for(int i = 0; i < NUM_COINS; i++) {
        createCoin(MV, V, coinArr[i].x, coinArr[i].y, coinArr[i].z);
    }
    //detect collision and remove coin
    if(containsCoin(LA.x, LA.y, LA.z + throttle)) {
        
        throttle += .01;
        addColor += .05;
    }
    

	/* draw trees (will always be ground level)*/
    for(int i =0; i < NUM_TREES; i++)
        createTree(MV, V, treeArr[i].x, treeArr[i].z, treeCol[i]);
    
    //detect collision
    if(containsTree(LA.x, LA.y, LA.z + throttle)) {
        wFlag = false;
        downFlag = false;
        if(planeSquish <= .4)
            planeSquish += .05;

    }
    
    /*create both airplanes*/
    createPlanes(MV, V , spherePos.x,spherePos.z);
    /* draw plane (z) sphere */
    createSphere(MV, V, spherePos.x, spherePos.y, spherePos.z, 2, 2, 2, currMaterial);
    //collision with sphere
    if(distance(spherePos, LA) < 2) {
        wFlag = false;
        downFlag = false;

        if(planeSquish <= .4)
            planeSquish += .05;
    }
    
    
    /* draw FLOOR*/
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(0, -1, 20));
    MV->scale(vec3(8, .2, 40));
    SetMaterial(currMaterial+5);//set material color look
    glUniform1f(prog->getUniform("lightPos"), lightPos);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    floorShape->draw(prog);
    MV->popMatrix();
    
    
    
    //draw LEFT WALL
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(8, 0, 20));
    MV->scale(vec3(.2, 5, 40));
    SetMaterial(currMaterial+8);//set material color look
    glUniform1f(prog->getUniform("lightPos"), lightPos);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    floorShape->draw(prog);
    MV->popMatrix();
    
    
    //draw RIGHT WALL
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(-8, 0, 20));
    MV->scale(vec3(.2, 5, 40));
    SetMaterial(currMaterial+8);//set material color look
    glUniform1f(prog->getUniform("lightPos"), lightPos);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    floorShape->draw(prog);
    MV->popMatrix();
    
    //draw ENDING
    MV->pushMatrix();
    MV->loadIdentity();
    MV->translate(vec3(0, 1, 70));
    MV->scale(vec3(25, 15, 15));

    MV->rotate(glfwGetTime(), vec3(0,0,1));//spin
    MV->rotate(1.57, vec3(0,1,0));//orient towards level
    SetMaterial(currMaterial+9);//set material color look
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    spiral->draw(prog);
    MV->popMatrix();
    
    
	prog->unbind();

   // Pop matrix stacks.
   P->popMatrix();

}

//keep camera motion going in corresponding direction until flag is switched again
static void motionFlags() {
    if(wFlag) {
        //bound bottom and top
        if((Eye.y - throttle * w.y) < 0 || (Eye.y - throttle * w.y) > 4.5) {
            Eye.x -= throttle * w.x;
            LA.x -= throttle * w.x;
            Eye.z -= throttle * w.z;
            LA.z -= throttle * w.z;
        }
        //bound sides
        else if((Eye.x - throttle * w.x) > 7 || (Eye.x - throttle * w.x) < -7) {
            Eye.y -= throttle * w.y;
            LA.y -= throttle * w.y;
            Eye.z -= throttle * w.z;
            LA.z -= throttle * w.z;
        }
        else {
            Eye -= throttle * w;
            LA -= throttle * w;
        }
        
    }
//    
//    if(sFlag) {
//        Eye += .02f * w;
//        LA += .02f * w;
//    }
//    
    if(aFlag) {
        
        if(Eye.x < 7 && Eye.x > -7) {
            Eye -= .03f * u;
            LA -= .03f * u;
        }
        
    }
    
    if(dFlag) {
        if(Eye.x < 7 && Eye.x > -7) {
        Eye += .03f * u;
        LA += .03f * u;
        }
    }
    
}

int main(int argc, char **argv)
{
    
    theta = M_PI/2;
    phi = 0;
    
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
   //request the highest possible version of OGL - important for mac
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(960, 720, "Portal Plane", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	//weird bootstrap of glGetError
    glGetError();
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
    //set the scroll call back ***
    glfwSetScrollCallback(window, scroll_callback);
    //set the window resize call back
    glfwSetFramebufferSizeCallback(window, resize_callback);
	// Initialize scene. Note geometry initialized in init now
	init();
    //generate positions
    genTrees();
    genCoins();
    genHeli();
    spherePos = vec3(0, 2, 20);
    
    
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
        //allow continuous motion with single key press
        motionFlags();
        // Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
