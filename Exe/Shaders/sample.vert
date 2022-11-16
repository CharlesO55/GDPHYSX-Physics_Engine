#version 330 core //Version Number

//the position variable has attribute position 0
//Accesses the vertex position and assigns it to aPos
layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 m_tan;
layout(location = 4) in vec3 m_btan;

out vec2 texCoord;
out vec3 normCoord;
out vec3 fragPos;
out mat3 TBN;       //Tan to obj space

//Create MVP matrices
uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;


void main()
{
    //transform then view then projection
    gl_Position = projection * view * transform * //Multiply the matrix with the position
                    vec4(aPos, 1.0); // Turns our 3x1 matrix into a 4x1

    texCoord = aTex;

    
    mat3 modelMat = mat3(transpose(inverse(transform)));
    normCoord =  modelMat * vertexNormal;

    //Tangent light
    vec3 T = normalize(modelMat * m_tan);
    //vec3 B = normalize(modelMat * m_btan);
    vec3 N = normalize(normCoord);

    T = normalize(T - dot(T, N) * N);   //Same as vec3 B but Gram-Schmidt Process
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);

    fragPos = vec3(transform * vec4(aPos, 1.0));
}