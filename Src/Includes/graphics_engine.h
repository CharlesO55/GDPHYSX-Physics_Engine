#ifndef GRAPHICS_ENGINE_FILE
#define GRAPHICS_ENGINE_FILE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#define TINYOBJLOADER_IMPLEMENTATION
//#include "tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

#include <string>
#include <iostream>
#include <vector>


/* CLASSES:
* Model
* Light
* Shader
* Camera
* Player
*/
/*
class Model {
public:
    float position[3];
    float scale;
    float rotation[3];
    float revolution[3];

    std::vector<GLfloat> fullVertexData;    //Full vertex data array
    GLuint VAO, VBO;                        //VAO & VBO
    glm::mat4 transform;                    //Transform matrix

    GLuint texBase, texNorm, texOverlay;    //3 slots for texture and normal map

    Model();

    //Loads 3D object's properties
    // @param fileAddress - String of the 3D obj's location
    void loadObj(std::string fileAddress);


    //Loads and generates opengl textures
    // @param fileAddress (Only use PNG for consistency) - String of the texture file location
    // @param textureIdentifier (0-Base tex, 1-Normal map, 2-Additional) - Identifies which texture is being loaded
    void loadTexture(std::string fileAddress, int textureIdentifier);

    //Generates the VAO and VBO
    void generateVAO();

    //Calculate transform matrix for revolving around a point
    // @param radius (>0) - Distance of how far the object should be from its origin when revolved
    void revolve(float radius);
    void rotate();
    void translate(glm::vec3 inputPos);
};*/

class Light {
public:
    glm::vec3 lightPos;
    glm::vec3 lightColor;

    float ambientStr = 0.5f;
    glm::vec3 ambientColor;

    float specStr;  //Shine brightness
    float specPhong; //(Smallest is 10.f) Shine size. Lower = Shinier & Bigger

    Light();
};



class Shader {
public:
    GLuint shaderProgram;

    //Create shader program
    void generateShaderProgram(const char* vertSrc, const char* fragSrc);

    //Pass the MVP into the shader
    void passMVP(glm::mat4 transform, glm::mat4 projection, glm::mat4 view);

    //Pass the textures into the shader
    // @param blendMode - (0-No overlay), (1-Overlay), (2-Multiply)
    void passTextures(GLuint texture, GLuint norm_tex, GLuint tex2, int blendMode);

    //Pass lighting properties to shader
    // @param isPointLight - Switches shader lighting mode (-1 Spotlight, 0 Unlit, 1 Point light)
    void passLight(glm::vec3 lightPos, glm::vec3 lightColor, glm::vec3 ambientColor, float ambientStr, float specStr, float specPhong, glm::vec3 cameraPos, int isPointLight, glm::vec3 playerFacing);

    //Render the object
    void draw(GLuint VAO, std::vector<GLfloat> fullVertexData);
};



class Camera {
public:
    glm::mat4 projection;   //The projection matrix
    glm::mat4 view;         //The view matrix
    glm::vec3 centerPos;    //Eye of the camera

    float screenW, screenH, perspMode;
    glm::vec3 camPosAdjustment;

    Camera(int isPerspective, float screenWidth, float screenHeight, float maxCamRange);


    double xPos, yPos;              //Cursor position
    float camPan[2];
    float orthoPos[2];
    glm::vec3 cameraPos;     //Initialize camera position


    float dirX, dirY, dirZ;

    void updateCamera(GLFWwindow* window, float deltaTime, glm::vec3 playerPosition, int WSkey, int ADkey);
};



class Player {
public:
    float position[3];
    float startingRotation;
    float orientation[3];
    float spaceshipSpeed;
    float turningSpeed;
    float maxPitch;
    float autoLevelPitch;     //Speed of returning ship back to normal pitch. Higher values = Slower restabilization

    Player();   //Constructor
    //Moves forward according to direction ship is facing
    // @param input = Set to 1 for forward calculations
    void moveToDirection(int forward, float deltaTime);

    //Orients and moves the player
    void movePlayer(int movement[], float deltaTime);
};

#endif
