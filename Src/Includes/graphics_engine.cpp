#include "graphics_engine.h"


Light::Light(){     //Constructor
    lightPos = glm::vec3(0.f, 0.f, 0.f);
    lightColor = glm::vec3(0.0f, 1.0f, 1.0f);

    ambientStr = 0.5f;
    ambientColor = lightColor;

    specStr = 0.1f;  //Shine brightness
    specPhong = 5.f; //(Smallest is 10.f) Shine size. Lower = Shinier & Bigger
};





//Create shader program
void Shader::generateShaderProgram(const char* vertSrc, const char* fragSrc) {
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);   //Create shader
    glShaderSource(vertShader, 1, &vertSrc, NULL);          //Assign source
    glCompileShader(vertShader);                            //Compile

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSrc, NULL);
    glCompileShader(fragShader);

    shaderProgram = glCreateProgram();   //Create shader program
    glAttachShader(shaderProgram, vertShader);  //Attach compiled vert shader
    glAttachShader(shaderProgram, fragShader);  //Attach compile frag shader

    glLinkProgram(shaderProgram);

    glDeleteShader(vertShader); //Cleanup shaders
    glDeleteShader(fragShader);
}

//Pass the MVP into the shader
void Shader::passMVP(glm::mat4 transform, glm::mat4 projection, glm::mat4 view) {
    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(
        transformLoc,
        1,
        GL_FALSE,
        glm::value_ptr(transform)
    );
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(
        projectionLoc,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(
        viewLoc,
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );
}

//Pass the textures into the shader
// @param blendMode - (0-No overlay), (1-Overlay), (2-Multiply)
void Shader::passTextures(GLuint texture, GLuint norm_tex, GLuint tex2, int blendMode) {
    glActiveTexture(GL_TEXTURE0);
    GLuint tex0Loc = glGetUniformLocation(shaderProgram, "tex0");
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(tex0Loc, 0);    //Send base texture 0

    glActiveTexture(GL_TEXTURE1);
    GLuint tex1Loc = glGetUniformLocation(shaderProgram, "norm_tex");
    glBindTexture(GL_TEXTURE_2D, norm_tex);
    glUniform1i(tex1Loc, 1);    //Pass texture 1 (normal map)

    glActiveTexture(GL_TEXTURE2);
    GLuint tex2Loc = glGetUniformLocation(shaderProgram, "tex2");
    glBindTexture(GL_TEXTURE_2D, tex2);
    glUniform1i(tex2Loc, 2);    //Pass texture 2 (overlay)

    GLuint blendModeAddress = glGetUniformLocation(shaderProgram, "blendMode");
    glUniform1i(blendModeAddress, blendMode);   //Pass blend mode
}

//Pass lighting properties to shader
// @param isPointLight - Switches shader lighting mode (-1 Spotlight, 0 Unlit, 1 Point light)
/*void Shader::passLight(glm::vec3 lightPos, glm::vec3 lightColor, glm::vec3 ambientColor, float ambientStr, float specStr, float specPhong, glm::vec3 cameraPos, int isPointLight, glm::vec3 playerFacing) {
    //Diffuse
    GLuint lightPosAddress = glGetUniformLocation(shaderProgram, "lightPos");
    glUniform3fv(lightPosAddress, 1, glm::value_ptr(lightPos));

    GLuint lightColorAddress = glGetUniformLocation(shaderProgram, "lightColor");
    glUniform3fv(lightColorAddress, 1, glm::value_ptr(lightColor));

    //Ambience
    GLuint ambientColorAddress = glGetUniformLocation(shaderProgram, "ambientColor");
    glUniform3fv(ambientColorAddress, 1, glm::value_ptr(ambientColor));

    GLuint ambientStrAddress = glGetUniformLocation(shaderProgram, "ambientStr");
    glUniform1f(ambientStrAddress, ambientStr);

    //Specular
    GLuint specStrAddress = glGetUniformLocation(shaderProgram, "specStr");
    glUniform1f(specStrAddress, specStr);

    GLuint specPhongAddress = glGetUniformLocation(shaderProgram, "specPhong");
    glUniform1f(specPhongAddress, specPhong);

    GLuint lighingModeAddress = glGetUniformLocation(shaderProgram, "lightingMode");
    glUniform1i(lighingModeAddress, isPointLight);      //Pass the lighting mode

    GLuint cameraPosAddress = glGetUniformLocation(shaderProgram, "cameraPos");
    glUniform3fv(cameraPosAddress, 1, glm::value_ptr(cameraPos));

    GLuint playerFacingAddress = glGetUniformLocation(shaderProgram, "playerFacing");
    glUniform3fv(playerFacingAddress, 1, glm::value_ptr(playerFacing));
}*/

void Shader::passLight(Light activeLight, glm::vec3 cameraPos, int isPointLight, glm::vec3 playerFacing) {
    //Diffuse
    GLuint lightPosAddress = glGetUniformLocation(shaderProgram, "lightPos");
    glUniform3fv(lightPosAddress, 1, glm::value_ptr(activeLight.lightPos));

    GLuint lightColorAddress = glGetUniformLocation(shaderProgram, "lightColor");
    glUniform3fv(lightColorAddress, 1, glm::value_ptr(activeLight.lightColor));

    //Ambience
    GLuint ambientColorAddress = glGetUniformLocation(shaderProgram, "ambientColor");
    glUniform3fv(ambientColorAddress, 1, glm::value_ptr(activeLight.ambientColor));

    GLuint ambientStrAddress = glGetUniformLocation(shaderProgram, "ambientStr");
    glUniform1f(ambientStrAddress, activeLight.ambientStr);

    //Specular
    GLuint specStrAddress = glGetUniformLocation(shaderProgram, "specStr");
    glUniform1f(specStrAddress, activeLight.specStr);

    GLuint specPhongAddress = glGetUniformLocation(shaderProgram, "specPhong");
    glUniform1f(specPhongAddress, activeLight.specPhong);

    GLuint lighingModeAddress = glGetUniformLocation(shaderProgram, "lightingMode");
    glUniform1i(lighingModeAddress, isPointLight);      //Pass the lighting mode

    GLuint cameraPosAddress = glGetUniformLocation(shaderProgram, "cameraPos");
    glUniform3fv(cameraPosAddress, 1, glm::value_ptr(cameraPos));

    GLuint playerFacingAddress = glGetUniformLocation(shaderProgram, "playerFacing");
    glUniform3fv(playerFacingAddress, 1, glm::value_ptr(playerFacing));
}

//Render the object
void Shader::draw(GLuint VAO, std::vector<GLfloat> fullVertexData) {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8); //Divide by number of floats inside array XYZ,normXYZ,UV
}

//Call all functions related to drawing
void Shader::drawSequence(ShaderPackage packagedShader) {
    passMVP(packagedShader.modelTransform, packagedShader.activeCam.projection, packagedShader.activeCam.view);
    passTextures(packagedShader.texBase, packagedShader.texNorm, packagedShader.texOverlay, packagedShader.blendingMode);    //0-Base tex and normal only , 1-Base tex with overlay , 2-Base tex with multiply
    passLight(packagedShader.activeLight, packagedShader.activeCam.cameraPos, packagedShader.lightingMode, packagedShader.playerFacing);
    draw(packagedShader.VAO, packagedShader.fullVertexData);
}

ShaderPackage::ShaderPackage() {
    lightingMode = UNLIT_LIGHTMODE;
    blendingMode = 1;
}
    void ShaderPackage::storeModelProperties(glm::mat4 modelTransform, GLuint texBase, GLuint texNorm, GLuint texOverlay, GLuint VAO, std::vector<GLfloat> fullVertexData) {
        this->modelTransform = modelTransform;
        this->texBase = texBase;
        this->texNorm = texNorm; 
        this->texOverlay = texOverlay;
        this->VAO = VAO;
        this->fullVertexData = fullVertexData;
    }

    void ShaderPackage::storeClasses(Camera activeCam, Light activeLight, glm::vec3 playerFacing) {
        this->activeCam = activeCam;
        this->activeLight = activeLight;
        this->playerFacing = playerFacing;
    }



Camera::Camera() {
}
Camera::Camera(int isPerspective, float screenWidth, float screenHeight, float maxCamRange) {
        screenW = screenWidth;
        screenH = screenHeight;
        perspMode = isPerspective;

        //SELECT A PROJECTION MODE
        if (isPerspective == 1) {   //Perspective
            projection = glm::perspective(glm::radians(60.0f), screenH / screenW, 0.1f, maxCamRange);
            camPosAdjustment = glm::vec3(0.f, 0.f, -0.05f);  //Position behind player position
        }
        else {  //Orthographic
            float projSize = 20.0f; //Bigger values makes result smaller
            projection = glm::ortho(-projSize, projSize, -projSize, projSize, -1.0f, maxCamRange);
            camPosAdjustment = glm::vec3(0.f, 20.f, 0.f);      //Position camera above
        }

        xPos = 0.f, yPos = 0.f;              //Cursor position
        camPan[0] = 90.f;
        camPan[1] = 90.f;
        orthoPos[0] = 0.f;
        orthoPos[1] = 0.f;
        cameraPos = camPosAdjustment;     //Initialize camera position
        dirX = 0.f, dirY = 0.f, dirZ = 0.f;
}    


void Camera::updateCamera(GLFWwindow* window, float deltaTime, glm::vec3 playerPosition, int WSkey, int ADkey) {
    glm::vec3 WorldUp = glm::vec3(0, 1, 0);
    glm::mat4 cameraPosMatrix = glm::translate(
        glm::mat4(1.0f),
        cameraPos * -1.0f);   //translate using negative cam position

    //Pan camera values based on cursor position
    glfwGetCursorPos(window, &xPos, &yPos);                        //Cursor feedback for camera movement
    camPan[0] += deltaTime * (2 * xPos / screenW - 1) * 1000.f;
    camPan[1] += deltaTime * (-2 * yPos / screenH + 1) * 1000.f;
    glfwSetCursorPos(window, screenW / 2, screenH / 2);            //Recenters cursor position to avoid mouse going offscreen

    //Avoid exceeding 180 and 0 degrees
    if (camPan[0] >= 180)
        camPan[0] = 180;
    if (camPan[0] <= 0)
        camPan[0] = 0;
    if (camPan[1] >= 180)
        camPan[1] = 180;
    if (camPan[1] <= 0)
        camPan[1] = 0;


    //UPDATE CAMERA/EYE POS
    if (perspMode) {    //Perspective camera
        //OLD FPS CAMERA
        //cameraPos = playerPosition + camPosAdjustment;      //Sets the camera behind the player
        //centerPos = glm::vec3(camPan[0], camPan[1], 0.f);   //Sets the camera target bases on panning amount

        //THIRD PERSON CAMERA
        dirX = cos(glm::radians(camPan[0]));
        dirY = -cos(glm::radians(camPan[1]));
        dirZ = -1.f * sin(glm::radians(camPan[1])) * sin(glm::radians(camPan[0]));
        cameraPos = playerPosition + glm::vec3(dirX, dirY, dirZ) + camPosAdjustment;    //Adjust camera to properly move around the player

        centerPos = playerPosition;     //Focus camera on the ship position
    }
    else {              //Orthographic camera
        float glideSpeed = 180.f;

        orthoPos[0] += WSkey * deltaTime * glideSpeed;
        orthoPos[1] += ADkey * deltaTime * glideSpeed;

        cameraPos = glm::vec3(-orthoPos[1], 0.f, -orthoPos[0]) + camPosAdjustment;  //Move camera based WASD movement and then adjust its height
        centerPos = cameraPos + glm::vec3(0.f, 3.6f, -0.1f);                        //Make sure to always look down at camera position
    }

    glm::vec3 F = glm::normalize(centerPos - cameraPos);
    glm::vec3 R = glm::normalize(glm::cross(F, WorldUp));
    glm::vec3 U = glm::normalize(glm::cross(R, F));

    glm::mat4 cameraOrientationMatrix = glm::mat4(
        glm::vec4(R, 0),
        glm::vec4(U, 0),
        glm::vec4((F * -1.0f), 0),
        glm::vec4(glm::vec3(0, 0, 0), 1)
    );

    view = cameraOrientationMatrix * cameraPosMatrix;   //Update the view matrix
}




Player::Player() {  //Constructor
    //Convert to glm::vec3 instead on rebuild. Saves hassle from array 
    position[0] = 0.f; 
    position[1] = 0.f; 
    position[2] = -30.f;
    startingRotation = 0.f;
    orientation[0] = 0.f;
    orientation[1] = 0.f;
    orientation[2] = 0.f;
    spaceshipSpeed = 10.f;
    turningSpeed = 90.f;
    maxPitch = 90.f;
    autoLevelPitch = 5.f;     //Speed of returning ship back to normal pitch. Higher values = Slower restabilization
}

//Moves forward according to direction ship is facing
// @param input = Set to 1 for forward calculations
void Player::moveToDirection(int forward, float deltaTime) {
    position[0] += forward * deltaTime * spaceshipSpeed * -cos(glm::radians(orientation[1] - startingRotation / 2));
    position[1] += forward * deltaTime * spaceshipSpeed * -sin(glm::radians(orientation[0]));
    position[2] += forward * deltaTime * spaceshipSpeed * cos(glm::radians(orientation[1] - startingRotation)) * cos(glm::radians(orientation[0]));
}

//Orients and moves the player
void Player::movePlayer(int movement[], float deltaTime) {
    //Yaw
    if (movement[1] < 0) {
        orientation[1] += movement[1] * deltaTime * turningSpeed;   //turn
        moveToDirection(-movement[1], deltaTime);                   //move forward while turning
    }
    else if (movement[1] > 0) {
        orientation[1] += movement[1] * deltaTime * turningSpeed;
        moveToDirection(movement[1], deltaTime);
    }

    //Pitch
    if (movement[2] > 0) {
        if (orientation[0] <= maxPitch)     //Only increase if pitch hasn't reached max
            orientation[0] += deltaTime * turningSpeed * ((maxPitch - orientation[0]) / (maxPitch / 2));  //Avoids the snappy halt by slowly decelerating till turning speed approaches 0
        moveToDirection(1, deltaTime);      //Move forward while turning
    }
    else if (movement[2] < 0) {
        if (orientation[0] >= -maxPitch)
            orientation[0] += deltaTime * -turningSpeed * ((-maxPitch - orientation[0]) / (-maxPitch / 2));
        moveToDirection(1, deltaTime);
    }
    else {
        orientation[0] -= deltaTime * (orientation[0] / autoLevelPitch);      //Auto level the pitch back to normal if not being pressed
    }
    //Forward/back
    if (movement[0] != 0) {
        moveToDirection(movement[0], deltaTime);    //Move along current orientation
    }
}
