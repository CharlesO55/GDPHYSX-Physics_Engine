#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "Includes/tiny_obj_loader.h"

//OpenGL Mathematics Headers
#define GLM_FORCE_CTOR_INIT     //[IMPORTANT] Initialize mats and vecs as 0 or NULL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>


#define STB_IMAGE_IMPLEMENTATION
#include "Includes/stb_image.h"

#include "Includes/config.h"         //Configurations
#include "Includes/particle.h"       //Particle settings and constants
#include "controls.h"       //Controls for keyboard and mouse
#include "Includes/graphics_engine.h"   //Light, Shader, Camera, Player
#include "Includes/physics_engine.h"


class Model {
public:
    glm::vec3 posVec, rotVec, revVec;   //Vectors for position, rotation, revolution
    float scale;

    Model();    //Default Constructor
    Model(std::string fileAddress, glm::vec3 startPos);     //Constructor 2
    
    std::vector<GLfloat> fullVertexData;    //Full vertex data array
    GLuint VAO, VBO;                        //VAO & VBO
    glm::mat4 transform;                    //Transform matrix
    GLuint texBase, texNorm, texOverlay;    //3 slots for texture and normal map

    //Loads 3D object's properties
    // @param fileAddress - String of the 3D obj's location
    void loadObj(std::string fileAddress);  //loadObj can be handled by Model constructor on init but not loadTexture and generateVAO.

    //Loads and generates opengl textures
    // @param fileAddress (Only use PNG for consistency) - String of the texture file location
    // @param textureIdentifier (0-Base tex, 1-Normal map, 2-Additional) - Identifies which texture is being loaded
    void loadTexture(std::string fileAddress, int textureIdentifier);
    void generateVAO();

    void copyTextures(Model sourceModel);
    //Calculate transform matrix for revolving around a point
    // @param radius (>0) - Distance of how far the object should be from its origin when revolved
    void revolve(float radius);
    void rotate();
    void translate(glm::vec3 inputPos);
};



int main(void)
{
    GLFWwindow* window;
    srand((unsigned) time(NULL));        //RNG seed

    //INITIALIZE AND LOAD OBJ FILES
    Model bullets[MAX_PARTICLES] = {
        Model("3D/planet.obj", ORIGIN),   //1
        Model("3D/planet.obj", ORIGIN),   //2
        Model("3D/planet.obj", ORIGIN),   //3
        Model("3D/planet.obj", ORIGIN),   //4
        Model("3D/planet.obj", ORIGIN),   //5
        Model("3D/planet.obj", ORIGIN),   //6
        Model("3D/planet.obj", ORIGIN),   //7
        Model("3D/planet.obj", ORIGIN)    //Last model
    };

    //THE MASS AGGREGATE MODELS
    float cubeSideHalfLength = 5.f;
    glm::vec3 cubeOrigin = { 10.f, 0.f, 0.f };
    cubeOrigin += glm::vec3(cubeSideHalfLength, -cubeSideHalfLength, cubeSideHalfLength);
    Model massAggregateModels[8] = {   //Eight cube points
                                                                                                        //FRONT
        Model("3D/planet.obj", cubeOrigin + glm::vec3(cubeSideHalfLength, cubeSideHalfLength, 0.f - cubeSideHalfLength)),   //0 Top Left
        Model("3D/planet.obj", cubeOrigin + glm::vec3(cubeSideHalfLength, -cubeSideHalfLength, 0.f - cubeSideHalfLength)),  //1 Bot Left
        Model("3D/planet.obj", cubeOrigin + glm::vec3(-cubeSideHalfLength, cubeSideHalfLength, 0.f - cubeSideHalfLength)),  //2 Top Right
        Model("3D/planet.obj", cubeOrigin + glm::vec3(-cubeSideHalfLength, -cubeSideHalfLength, 0.f - cubeSideHalfLength)), //3 Bot Right
                                                                                                        //BACK
        Model("3D/planet.obj", cubeOrigin + glm::vec3(cubeSideHalfLength, cubeSideHalfLength, 0.f + cubeSideHalfLength)),  //4 Top Left
        Model("3D/planet.obj", cubeOrigin + glm::vec3(cubeSideHalfLength, -cubeSideHalfLength, 0.f + cubeSideHalfLength)), //5 Bot Left
        Model("3D/planet.obj", cubeOrigin + glm::vec3(-cubeSideHalfLength, cubeSideHalfLength, 0.f + cubeSideHalfLength)), //6 Top Right
        Model("3D/planet.obj", cubeOrigin + glm::vec3(-cubeSideHalfLength, -cubeSideHalfLength, 0.f + cubeSideHalfLength)) //7 Bot Right
    };
    /*
      4--------6
     /|       /|
    0--------2 |
    | |      | |
    | 5--------7
    |/       |/
    1--------3
    */
    /*
    Model playerShip;
    playerShip.loadObj("3D/ship2.obj");
    */

    //Vertices for the cube
    float skyboxDisplaySize = 100.f;
    float skyboxVertices[]{
    -skyboxDisplaySize, -skyboxDisplaySize, skyboxDisplaySize, //0
    skyboxDisplaySize, -skyboxDisplaySize, skyboxDisplaySize,  //1
    skyboxDisplaySize, -skyboxDisplaySize, -skyboxDisplaySize, //2
    -skyboxDisplaySize, -skyboxDisplaySize, -skyboxDisplaySize,//3
    -skyboxDisplaySize, skyboxDisplaySize, skyboxDisplaySize,  //4
    skyboxDisplaySize, skyboxDisplaySize, skyboxDisplaySize,   //5
    skyboxDisplaySize, skyboxDisplaySize, -skyboxDisplaySize,  //6
    -skyboxDisplaySize, skyboxDisplaySize, -skyboxDisplaySize//7
};


    //Skybox Indices
    unsigned int skyboxIndices[]{
        1,2,6,
        6,5,1,

        0,4,7,
        7,3,0,

        4,5,6,
        6,7,4,

        0,3,2,
        2,1,0,

        0,1,5,
        5,4,0,

        3,7,6,
        6,2,3
    };

    //Skybox texture images
    std::string facesSkybox[]{
        "Skybox/Nebula/nebula_rt.png",
        "Skybox/Nebula/nebula_lf.png",
        "Skybox/Nebula/nebula_up.png",
        "Skybox/Nebula/nebula_dn.png",
        "Skybox/Nebula/nebula_ft.png",
        "Skybox/Nebula/nebula_bk.png"
    };

    
    //CREATING SHADERS
    //Object shader
    //Load shader file >> string stream
    std::fstream vertStream("Shaders/sample.vert");
    std::stringstream vertStrStream;
    //Add file stream >> string stream
    vertStrStream << vertStream.rdbuf();
    //string stream >> character array
    std::string vertStr = vertStrStream.str();
    const char* vertSrc = vertStr.c_str();
    
    std::fstream fragStream("Shaders/sample.frag");
    std::stringstream fragStrStream;
    fragStrStream << fragStream.rdbuf();
    std::string fragStr = fragStrStream.str();
    const char* fragSrc = fragStr.c_str();

    //Skybox shader
    std::fstream skyboxVertexSrc("Shaders/skybox.vert");
    std::stringstream skyboxVertexBuff;
    skyboxVertexBuff << skyboxVertexSrc.rdbuf();
    std::string skyboxVertexS = skyboxVertexBuff.str();
    const char* sky_v = skyboxVertexS.c_str();
    
    std::fstream skyboxFragSrc("Shaders/skybox.frag");
    std::stringstream skyboxFragBuff;
    skyboxFragBuff << skyboxFragSrc.rdbuf();
    std::string skyboxFragS = skyboxFragBuff.str();
    const char* sky_f = skyboxFragS.c_str();
    
    


    if (!glfwInit())
        return -1;


    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Charles Mathew D. Ong", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();

    
    //LOAD THE TEXTURES
    //Just load one model
    bullets[0].loadTexture("3D/mars_color.png", 0);
    bullets[0].loadTexture("3D/mars_normal.png", 1);
    bullets[0].loadTexture("3D/mars_impact.png", 2);
    //Then copy its texture since same anyways
    for (int i = 1; i < MAX_PARTICLES; i++) {
        bullets[i].copyTextures(bullets[0]);
    }
    //2 diffrent colors copied to cube models
    massAggregateModels[0].loadTexture("3D/color_green.png", 0);
    massAggregateModels[4].loadTexture("3D/color_brown.png", 0);
    //Doesn't need normal or overlay tex
    for (int i = 1; i < 4; i++) {
        massAggregateModels[i].copyTextures(massAggregateModels[0]);
        massAggregateModels[i + 4].copyTextures(massAggregateModels[4]);
    }

    /*
    playerShip.loadTexture("3D/ship2.png", 0);
    playerShip.loadTexture("3D/ship2_normal.png", 1);
    playerShip.loadTexture("3D/camo_pattern.png", 2);
    */

    glEnable(GL_DEPTH_TEST); //For rendering front of obj only

    //CREATE THE SHADER PROGRAMS
    Shader objectShader;
    objectShader.generateShaderProgram(vertSrc, fragSrc);   //Object/model shader using sample.frag & sample.vert
    GLuint shaderProgram = objectShader.shaderProgram;
    
    Shader skybox;
    skybox.generateShaderProgram(sky_v, sky_f);             //Skybox shader using skybox.frag & skybox.vert
    GLuint skyboxShader = skybox.shaderProgram;

    
    //GENERATE THE VAOs and VBOs
    bullets[0].generateVAO();
    //Same sphere obj so just copy
    for (int i = 1; i < MAX_PARTICLES; i++) {
        bullets[i].VAO = bullets[0].VAO;
    }
    for (int i = 0; i < 8; i++) {
        massAggregateModels[i].VAO = bullets[0].VAO;
    }
    //playerShip.generateVAO();

    //Skybox VAO, VBO, EBO
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    //Skybox Texture
    unsigned int skyboxTex;
    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
    //Avoid pixelate. Blurs according to size
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  //Max size
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  //Min size

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   
    for (unsigned int i = 0; i < 6; i++) {
        int w, h, skyChannel;
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(facesSkybox[i].c_str(), &w, &h, &skyChannel, 0);

        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, //R, L, T, B, F, Bk
                0,
                GL_RGBA,    //Swtiched to RGBA since skybox altered to PNG with alpha when saving
                w,
                h,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data
                );

            stbi_image_free(data);
        }
        else {

        }
    }
    //stbi_set_flip_vertically_on_load(true);

    float lastTime = glfwGetTime();     //Last frame timestamp

    //Keyboard input
    glfwSetKeyCallback(window, keyboard);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    //Create a pointlight and spotlight light instance + activeLight for switching
    Light pointLight, shipLight, activeLight;
    //Create cameras
    Camera mainCam(PERSPECTIVE_MODE, SCREEN_WIDTH, SCREEN_HEIGHT, MAX_CAM_DISTANCE);   //Perspective cam
    Camera topCam(ORTHOGRAPHIC_MODE, SCREEN_WIDTH, SCREEN_HEIGHT, MAX_CAM_DISTANCE);    //Orthographic cam
    Camera activeCam = mainCam; //Set active to the persp cam
    
    Player tester;                      //Instance the player for calculating movement
    tester.startingRotation = 180.f;
    tester.orientation[1] = tester.startingRotation;

    glm::vec3 playerFacing;             //For use in spotlight 

    

    


    //PARTICLE_HW(2/3)
    Particle bulletParticle[MAX_SPRINGS];     //Array of basic Particles
    int particleSlots = MAX_SPRINGS - 1;      //Counter to avoid exceeding total Models limit
    
    ForceRegistry registryGeneral;  //Links forces and particles

    //Force Generators
    GravityForce gravityGeneral;        //Gravity force calculator
    ConstantForce constantGeneral;      //Constant forces calculator
    DragForce dragGeneral(0.f, 0.1f);   //Drag forces calculator ~ Higher constants = More drag


    CentripetalForce centripetalGeneral(ORIGIN);


    AnchoredSpring anchorS1(ORIGIN);    //Anchored spring calculator. Anchored to origin
    BasicSpring basicS1;
    ElasticBungee elasticS1;
    //PARTICLE_HW(2/3) END

    ShaderPackage packedShader;
    // Old fireworks HW
    //Coil coilParticle[MAX_PARTICLES];         //Array of fireworks/coils
    //glm::vec3 detonationPos(0.f, 0.f, 0.f);   //Only 1 copy for now. Good for 1 firework only
    //int phase = 2;   //Phase of the fireworks. (1-Launch, 2-Spread)

    
    while (!glfwWindowShouldClose(window))  //Main loop for each frame
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Frame and time calculator
        GLfloat currTime = glfwGetTime();       //Current time
        float frameTime = currTime - lastTime;  //Time passed
        if (frameTime > 2) {    //Avoid large skips in time
            lastTime = currTime - 0.2;
            frameTime = 0.2;
        }

        
        //Switch active projection mode
        if (isPerspMode > 0) {
            mainCam.updateCamera(window, frameTime, glm::vec3(0.f,0.f,-30.f), 0, 0);    //Camera's position. switch with player position
            activeCam = mainCam;
        }
        else {
            topCam.updateCamera(window, frameTime, glm::vec3(0.f, 0.f, 0.f), movementInput[0], movementInput[1]);
            activeCam = topCam;
        }

        //Switch active light
        if (isPointLight) {
            activeLight = pointLight;
        }
        else {
            activeLight = shipLight;
        }

        //Enable player movement only in perspective mode
        /*if (isPerspMode == 1) {
            //Turn WASDQE inputs into movement inputs
            tester.movePlayer(movementInput, deltaTime);
            for (int i = 0; i <= 2; i++) {  //Copy player position and orientation into its model
                playerShip.position[i] = tester.position[i];
                playerShip.rotation[i] = tester.orientation[i];
            }
            playerShip.rotate();            //Compute the updates to the model
        }*/
        

        while (frameTime > 0.f) {
            float deltaTime = fmin(frameTime, TIMESTEP);    //Doesn't exceed timesteps
            frameTime -= deltaTime;                         //Deduct with deltaTime for next loop

            //PARTICLE HW(3/x)
            //INITIALIZATION WHEN SWITCHED PARTICLE
            if (isSwitched) {
                for (int i = 0; i < MAX_SPRINGS; i++) {
                    bulletParticle[i].despawnParticle(&particleSlots);  //Despawn all active particles
                }

                bulletParticle[0].initParticle(projectileType, bullets[0].scale, currTime, springsInitPos[projectileType - 1][0]);
                bulletParticle[1].initParticle(projectileType, bullets[0].scale, currTime, springsInitPos[projectileType - 1][1]);
                bulletParticle[0].partVel = glm::vec3(0.f, 0.f, 0.f);   //Stop one pair from moving to show it reacts to the other spring end
                //bulletParticle[1].partPos = glm::vec3(-20.f, 5.f, 0.f);

                if (projectileType == ANCHORED_SPRING || projectileType == CENTRIPETAL_FORCE) {
                    bulletParticle[0].mass = 0;     //Turn the anchor into a stationary object to exit updateMotion()
                }
                isSwitched = INACTIVE;
                spaceBarPressed = INACTIVE;
            }

            
            //APPLY FORCE WHEN CLICKED
            if (isFired) {
                registryGeneral.add(&(bulletParticle[1]), &constantGeneral);    //Only apply force to one of the particle pairs
                isFired = INACTIVE;
            }


            //Force updates
            switch (projectileType) {
            case BASIC_SPRING:
                basicS1.linkOtherEnd(bulletParticle[0].partPos);               //Link the other end
                registryGeneral.add(&(bulletParticle[1]), &basicS1);           //Calculate spring force
                basicS1.linkOtherEnd(bulletParticle[1].partPos);               //Repeat for other end
                registryGeneral.add(&(bulletParticle[0]), &basicS1);
                break;
            case ELASTIC_BUNGEE:
                elasticS1.linkOtherEnd(bulletParticle[0].partPos);
                registryGeneral.add(&(bulletParticle[1]), &elasticS1);
                elasticS1.linkOtherEnd(bulletParticle[1].partPos);
                registryGeneral.add(&(bulletParticle[0]), &elasticS1);
                break;
            case ANCHORED_SPRING:
                registryGeneral.add(&(bulletParticle[1]), &anchorS1);
                break;
            case CENTRIPETAL_FORCE:
                if (spaceBarPressed != 1) {
                    registryGeneral.add(&(bulletParticle[1]), &centripetalGeneral);
                }
                break;
            default:
                std::cout << "NO SPRING SELECTED" << std::endl;
            }

            //Add GRAVITY and DRAG if activated
            registryGeneral.add(&(bulletParticle[1]), &gravityGeneral);     //Gravity toggle in particle.h
            registryGeneral.add(&(bulletParticle[1]), &dragGeneral);        //Drag toggled on for all springs

            //Motion updates using all active forces
            bulletParticle[0].updateMotion(deltaTime, currTime, &particleSlots);
            bulletParticle[1].updateMotion(deltaTime, currTime, &particleSlots);

            //Copy particle pos to model pos
            bullets[0].translate(bulletParticle[0].partPos);
            bullets[1].translate(bulletParticle[1].partPos);


            
            for (int i = 0; i < 8; i++) {
               massAggregateModels[i].translate(massAggregateModels[i].posVec);
            }
            //PARTICLE_HW(3/3) END

            /*if (projectileType >= BASIC_SPRING && isSwitched) {
                for (int i = 0; i < MAX_PARTICLES; i++) {
                    bulletParticle[i].despawnParticle(&particleSlots);
                }
                isSwitched = INACTIVE;
            }*/
            /*
            //BULLET INITIALIZATION
            //If a bullet projectile is fired, initialize an inactive particles
            if (isFired && particleSlots > 0) {
                switch (projectileType) {
                /*case COIL:
                case FIREWORK:
                    for (int i = 0; i < MAX_PARTICLES; i++) {
                        if (coilParticle[i].partType < 1) {            //Find an inactive particle and initialize it
                            coilParticle[i].initParticle(projectileType, bullets[i].scale, currTime);
                            coilParticle[i].prepStage(detonationPos, phase);   //Prepare its spawn location and rng values
                            break;  //Exit for loop when one is initialized
                        }
                    }
                    break;*/
            /*
                case BASIC_SPRING:
                case ANCHORED_SPRING:
                case ELASTIC_BUNGEE:
                    bulletParticle[0].initParticle(projectileType, bullets[0].scale, currTime, ORIGIN); //Serve as particle on one end
                    //bulletParticle[1].initParticle(projectileType, bullets[0].scale, currTime); //Serve as the other particle
                    //bulletParticle[1].partPos = glm::vec3(2.f, 0.f, 0.f);                       //Offset to spring rest legnth
                    //bulletParticle[0].despawnTime = 100;    //Readjust despawn time
                    //bulletParticle[1].despawnTime = 100;    //Readjust despawn time
                    break;
                default:    //For bullet initialization
                    for (int i = 0; i < MAX_PARTICLES; i++) {
                        if (bulletParticle[i].partType < ACTIVE) {            //Find an inactive particle and initialize it
                            bulletParticle[i].initParticle(projectileType, bullets[i].scale, currTime, ORIGIN);
                            break;  //Exit the for loop to avoid initializing other INACTIVE particles
                        }
                    }
                    break;  //Exit switch case
                }
                isFired = INACTIVE;
                particleSlots--;    //Decrese number of particle slots available
            }*/
            

            //UPDATES
            //Continue updating active particles
            /*for (int i = 0; i < MAX_PARTICLES; i++) {
                if (bulletParticle[i].partType) {
                    //Links particle to force it will receive
                    registryGeneral.add(&(bulletParticle[i]), &gravityGeneral);         //Gravity   
                    registryGeneral.add(&(bulletParticle[i]), &constantGeneral);        //Constant forces
                    registryGeneral.add(&(bulletParticle[i]), &dragGeneral);            //Drag force
                    
                    bulletParticle[i].updateMotion(deltaTime, currTime, &particleSlots);    //Update the bullets

                    bullets[i].translate(bulletParticle[i].partPos);                        //Update model pos
                }/*
                else if (coilParticle[i].partType && (projectileType >= COIL)) {
                    coilParticle[i].updateMotion(deltaTime, currTime, &particleSlots);      //Update firework and coil
                    coilParticle[i].addCoil(currTime, &detonationPos);
                    bullets[i].translate(coilParticle[i].partPos);
                }*/
            /*}*/   
            
            
            /*//FIREWORK SPAWNER
            if (coilParticle[0].partType == INACTIVE  && projectileType == FIREWORK) {     //If parent detonated and still hasn't reached final explosion chain
                phase++;
                coilParticle[0].initParticle(COIL, bullets[0].scale, currTime);
                coilParticle[0].prepStage(detonationPos, phase);
                //Spawn more using coilParticle[coilParticle[0].payload]
            }*/
            
        }




        //Calculate player orientation for spotlight
        playerFacing = glm::vec3(cos(glm::radians(tester.orientation[1] - tester.startingRotation / 2)),
                                sin(glm::radians(tester.orientation[0])),
                                cos(glm::radians(tester.orientation[1] - tester.startingRotation))
                                );



        //SKYBOX SHADER
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glUseProgram(skyboxShader);

        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(glm::mat3(activeCam.view));
        //Pass the view and projection matrix to the skybox shader
        unsigned int skyboxViewLoc = glGetUniformLocation(skyboxShader, "view");
        glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));

        unsigned int skyboxProjLoc = glGetUniformLocation(skyboxShader, "projection");
        glUniformMatrix4fv(skyboxProjLoc, 1, GL_FALSE, glm::value_ptr(activeCam.projection));
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);




        //OBJECT SHADERS
        glUseProgram(shaderProgram);    //Reset to shader program, otherwise obj won't draw
        /*
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        */

        //Begin packing materials for shader
        packedShader.storeClasses(activeCam, activeLight, playerFacing);

        //Render the active bullets
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (bulletParticle[i].partType) {   //Render when a particle is still active
                packedShader.storeModelProperties(bullets[i].transform, bullets[i].texBase, bullets[i].texNorm, bullets[i].texOverlay, bullets[i].VAO, bullets[i].fullVertexData);
                packedShader.blendingMode = 1;
                packedShader.lightingMode = UNLIT_LIGHTMODE;

                objectShader.drawSequence(packedShader);
                }
            }
        
        //Draw all cube models
        for (int i = 0; i < 8; i++) {
            packedShader.storeModelProperties(massAggregateModels[i].transform, massAggregateModels[i].texBase, massAggregateModels[i].texNorm, massAggregateModels[i].texOverlay, massAggregateModels[i].VAO, massAggregateModels[i].fullVertexData);
            packedShader.blendingMode = 0;
            packedShader.lightingMode = UNLIT_LIGHTMODE;

            objectShader.drawSequence(packedShader);
        }
        //Draw the lines/rods connecting the cube
        /*
  4--------6
 /|       /|
0--------2 |
| |      | |
| 5--------7
|/       |/
1--------3
    */
        /*  //TRIED DRAWING ROD LINES BUT TOO THIN TO BE NOTICEABLY HELPFUL
        glBegin(GL_LINES);
            glVertex3f(massAggregateModels[0].posVec.x, massAggregateModels[0].posVec.y, massAggregateModels[0].posVec.z);
            glVertex3f(massAggregateModels[1].posVec.x, massAggregateModels[1].posVec.y, massAggregateModels[1].posVec.z);
        glEnd();*/

        /*
        //Render the player model
        objectShader.passMVP(playerShip.transform, projection, view);
        objectShader.passTextures(playerShip.texBase, playerShip.texNorm, playerShip.texOverlay, 2);
        objectShader.passLight(lightPos, lightColor, ambientColor, ambientStr, specStr, specPhong, cameraPos, isPointLight, playerFacing);
        objectShader.draw(playerShip.VAO, playerShip.fullVertexData);
        */
        glDisable(GL_BLEND);    //Stop blending to avoid affecting other textures


        glfwSwapBuffers(window);
        glfwPollEvents();
        lastTime = currTime;
        //end of while loop
        }

    //Clean up the VAO and VBO
    for (int i = 0; i < MAX_PARTICLES; i++) {
        glDeleteVertexArrays(1, &bullets[i].VAO);
        glDeleteBuffers(1, &bullets[i].VBO);
    }
    for (int i = 0; i < 8; i++) {
        glDeleteVertexArrays(1, &massAggregateModels[i].VAO);
        glDeleteBuffers(1, &massAggregateModels[i].VBO);
    }
    /*
    glDeleteVertexArrays(1, &playerShip.VAO);
    glDeleteBuffers(1, &playerShip.VBO);
    */
    glfwTerminate();
    return 0;
}





Model::Model() {
    posVec = { 0.f, 0.f, 0.f };
    scale = DEFAULT_MODEL_SIZE;
    rotVec = { 0.f, 0.f, 0.f };
    revVec = { 0.f, 0.f, 0.f };
}

Model::Model(std::string objAddress, glm::vec3 startPos) {
    posVec = startPos;
    scale = DEFAULT_MODEL_SIZE;
    rotVec = { 0.f, 0.f, 0.f };
    revVec = { 0.f, 0.f, 0.f };

    loadObj(objAddress);
    //"wrangle the OpenGL extension"    glewInit() or gl3wInit()
    //loadTexture and generateVAO refuse to work in constructor without clearing old GLuint variables
}

//Loads 3D object's properties
// @param fileAddress - String of the 3D obj's location
void Model::loadObj(std::string fileAddress) {

    std::string path = fileAddress;     //3D OBJ
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(&attributes,
        &shapes, &material,
        &warning, &error, path.c_str());

    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
    }

    //Getting the vectors tan
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;

    //Tangents and bitangents for lighting
    for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3) {
        tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
        tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
        tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

        //Vertex positions
        glm::vec3 v1 = glm::vec3(
            attributes.vertices[(vData1.vertex_index * 3)],
            attributes.vertices[(vData1.vertex_index * 3) + 1],
            attributes.vertices[(vData1.vertex_index * 3) + 2]
        );
        glm::vec3 v2 = glm::vec3(
            attributes.vertices[(vData2.vertex_index * 3)],
            attributes.vertices[(vData2.vertex_index * 3) + 1],
            attributes.vertices[(vData2.vertex_index * 3) + 2]
        );
        glm::vec3 v3 = glm::vec3(
            attributes.vertices[(vData3.vertex_index * 3)],
            attributes.vertices[(vData3.vertex_index * 3) + 1],
            attributes.vertices[(vData3.vertex_index * 3) + 2]
        );

        //UVs
        glm::vec2 uv1 = glm::vec2(
            attributes.texcoords[(vData1.texcoord_index * 2)],
            attributes.texcoords[(vData1.texcoord_index * 2) + 1]
        );
        glm::vec2 uv2 = glm::vec2(
            attributes.texcoords[(vData2.texcoord_index * 2)],
            attributes.texcoords[(vData2.texcoord_index * 2) + 1]
        );
        glm::vec2 uv3 = glm::vec2(
            attributes.texcoords[(vData3.texcoord_index * 2)],
            attributes.texcoords[(vData3.texcoord_index * 2) + 1]
        );

        glm::vec3 deltaPos1 = v2 - v1;
        glm::vec3 deltaPos2 = v3 - v1;

        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float r = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
    }

    //VERTEX DATA ARRAY
    //Important data for position XYZ + normals XYZ + UV + tan XYZ + Btan XYZ
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        int pos_offset = (vData.vertex_index) * 3;
        int norm_offset = (vData.normal_index) * 3;
        int tex_offset = (vData.texcoord_index) * 2;
        //X Index 0
        fullVertexData.push_back(
            attributes.vertices[pos_offset]);
        //Y Index 1
        fullVertexData.push_back(
            attributes.vertices[pos_offset + 1]);
        //Z Index 2
        fullVertexData.push_back(
            attributes.vertices[pos_offset + 2]);

        //normals X Index 3
        fullVertexData.push_back(
            attributes.normals[norm_offset]);
        //normals Y Index 4
        fullVertexData.push_back(
            attributes.normals[norm_offset + 1]);
        //normals Z Index 5
        fullVertexData.push_back(
            attributes.normals[norm_offset + 2]);

        //U Index 6
        fullVertexData.push_back(
            attributes.texcoords[tex_offset]);
        //V Index 7
        fullVertexData.push_back(
            attributes.texcoords[tex_offset + 1]);

        //Tangents      Index 8,9,10
        fullVertexData.push_back(tangents[i].x);
        fullVertexData.push_back(tangents[i].y);
        fullVertexData.push_back(tangents[i].z);
        //Bitangents    Index 11,12,13
        fullVertexData.push_back(bitangents[i].x);
        fullVertexData.push_back(bitangents[i].y);
        fullVertexData.push_back(bitangents[i].z);
    }
}


//Loads and generates opengl textures
// @param fileAddress (Only use PNG for consistency) - String of the texture file location
// @param textureIdentifier (0-Base tex, 1-Normal map, 2-Additional) - Identifies which texture is being loaded
void Model::loadTexture(std::string fileAddress, int textureIdentifier) {
    int img_width, img_height, colorChannel; //Properties of the texture image
    stbi_set_flip_vertically_on_load(true);
    unsigned char* tex_bytes =
        stbi_load(fileAddress.c_str(), &img_width, &img_height, &colorChannel, 0);

    //Switch between which GL_TEXTURE to assign to
    switch (textureIdentifier) {
    case 0:
        glGenTextures(1, &texBase);
        glActiveTexture(GL_TEXTURE0);           //Load first texture
        glBindTexture(GL_TEXTURE_2D, texBase);  //Assign to our base texture slot
        break;
    case 1:
        glGenTextures(1, &texNorm);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texNorm);  //Assign to our normal map texture slot
        break;
    case 2:
        glGenTextures(1, &texOverlay);
        glActiveTexture(GL_TEXTURE2);               //Load as additional texture
        glBindTexture(GL_TEXTURE_2D, texOverlay);   //Assigned as overlay
        break;
    default:
        glActiveTexture(GL_TEXTURE3);  //Load to an unused 3rd
        break;
    }

    //May need to allow for switching between clamp or repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);   //GL_CLAMP edge to extend
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);   //GL_REPEAT repeats

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA,
        img_width, img_height, 0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        tex_bytes);

    glGenerateMipmap(GL_TEXTURE_2D);  //Mipmaps
    stbi_image_free(tex_bytes);       //Frees up the bytes
}


//Generates the VAO and VBO
void Model::generateVAO() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat) * fullVertexData.size(),
        fullVertexData.data(),
        GL_DYNAMIC_DRAW
    );

    //Position
    glVertexAttribPointer(
        0,  //Index0 = Pos, Index1 = Color, Index 2 = UV
        3,  //3 floats: X,Y,Z
        GL_FLOAT,
        GL_FALSE,
        //Number of floats per array
        14 * sizeof(GLfloat), //14 total: XYZ, normal XYZ, UV, T(x,y,z), Bitan(x,y,z)
        (void*)0
    );

    //Normals
    GLintptr normPtr = 3 * sizeof(GLfloat);
    glVertexAttribPointer(
        1, //Index 1
        3, //3 floats: normal XYZ
        GL_FLOAT,
        GL_TRUE, //Yes, normalize
        14 * sizeof(GLfloat),
        (void*)normPtr
    );

    //UV
    GLintptr uvPtr = 6 * sizeof(GLfloat); //Start offset at Index listed in Vertex Array Data
    glVertexAttribPointer(
        2, //Index 2
        2, //2 floats: U,V
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(GLfloat),
        (void*)uvPtr
    );

    //Tangents
    GLintptr tangentPtr = 8 * sizeof(GLfloat);
    glVertexAttribPointer(
        3,
        3, //T(x,y,z)
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(GLfloat),
        (void*)tangentPtr
    );

    //Bitangents
    GLintptr bitangentPtr = 11 * sizeof(GLfloat);
    glVertexAttribPointer(
        4,
        3,  //B(x,y,z)
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(GLfloat),
        (void*)bitangentPtr
    );

    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::copyTextures(Model sourceModel) {
    this->texBase = sourceModel.texBase;
    this->texNorm = sourceModel.texNorm;
    this->texOverlay = sourceModel.texOverlay;
}


//Calculate transform matrix for revolving around a point
// @param radius (>0) - Distance of how far the object should be from its origin when revolved
void Model::revolve(float radius) {
    glm::mat4 identity = glm::mat3(1.0f);

    transform = glm::translate(identity,
        posVec);  //Use object position as center
    //Scale vertices
    transform = glm::scale(transform,
        glm::vec3(scale, scale, scale));
    //Rotate vertices
    transform = glm::rotate(transform,
        glm::radians(revVec.x),
        glm::vec3(1, 0, 0));
    //Rotate first 
    transform = glm::rotate(transform,
        glm::radians(revVec.y),
        glm::vec3(0, 1, 0));
    transform = glm::rotate(transform,
        glm::radians(revVec.z),
        glm::vec3(0, 0, -1));
    //Then extend by radius
    transform = glm::translate(transform,
        glm::vec3(radius, 0.f, 0.f));
}


void Model::rotate() {
    //Matrix setup
    glm::mat4 identity = glm::mat3(1.0f);

    //Transform vertices
    transform = glm::translate(
        identity,
        posVec);

    //Scale vertices
    transform = glm::scale(transform,
        glm::vec3(scale, scale, scale));
    //Rotate vertices
    transform = glm::rotate(transform,
        glm::radians(rotVec.x),
        glm::vec3(1, 0, 0));
    //Rotate by other axis
    transform = glm::rotate(transform,
        glm::radians(rotVec.y),
        glm::vec3(0, 1, 0));
    transform = glm::rotate(transform,
        glm::radians(rotVec.z),
        glm::vec3(0, 0, -1));
}


void Model::translate(glm::vec3 inputPos) {
    glm::mat4 identity = glm::mat3(1.0f);

    transform = glm::translate(
        identity,
        inputPos);
}