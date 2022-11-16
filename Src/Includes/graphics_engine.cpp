#include "graphics_engine.h"

/*
Model::Model() {
    position[0] = 0.f;  position[1] = 0.f;  position[2] = 0.f;
    scale = 1.f;
    rotation[0] = 0.f;  rotation[1] = 0.f;  rotation[2] = 0.f;
    revolution[0] = 0.f; revolution[1] = 0.f; revolution[2] = 0.f;
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

//Calculate transform matrix for revolving around a point
// @param radius (>0) - Distance of how far the object should be from its origin when revolved
void Model::revolve(float radius) {
    glm::mat4 identity = glm::mat3(1.0f);

    transform = glm::translate(identity,
        glm::vec3(position[0], position[1], position[2]));  //Use object position as center
    //Scale vertices
    transform = glm::scale(transform,
        glm::vec3(scale, scale, scale));
    //Rotate vertices
    transform = glm::rotate(transform,
        glm::radians(revolution[0]),
        glm::vec3(1, 0, 0));
    //Rotate first 
    transform = glm::rotate(transform,
        glm::radians(revolution[1]),
        glm::vec3(0, 1, 0));
    transform = glm::rotate(transform,
        glm::radians(revolution[2]),
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
        glm::vec3(position[0], position[1], position[2]));

    //Scale vertices
    transform = glm::scale(transform,
        glm::vec3(scale, scale, scale));
    //Rotate vertices
    transform = glm::rotate(transform,
        glm::radians(rotation[0]),
        glm::vec3(1, 0, 0));
    //Rotate by other axis
    transform = glm::rotate(transform,
        glm::radians(rotation[1]),
        glm::vec3(0, 1, 0));
    transform = glm::rotate(transform,
        glm::radians(rotation[2]),
        glm::vec3(0, 0, -1));
}

void Model::translate(glm::vec3 inputPos) {
    glm::mat4 identity = glm::mat3(1.0f);

    transform = glm::translate(
        identity,
        inputPos);
    /*transform = glm::translate(
        identity,
        glm::vec3(position[0], position[1], position[2]));*/
//}






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
void Shader::passLight(glm::vec3 lightPos, glm::vec3 lightColor, glm::vec3 ambientColor, float ambientStr, float specStr, float specPhong, glm::vec3 cameraPos, int isPointLight, glm::vec3 playerFacing) {
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
}

//Render the object
void Shader::draw(GLuint VAO, std::vector<GLfloat> fullVertexData) {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8); //Divide by number of floats inside array XYZ,normXYZ,UV
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
