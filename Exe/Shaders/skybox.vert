#version 330 core //Version Number

//the position variable has attribute position 0
//Accesses the vertex position and assigns it to aPos
layout(location = 0) in vec3 aPos; 

out vec3 texCoord;

uniform mat4 projection;
uniform mat4 view;



void main()
{
    vec4 pos = projection * view * 
                    vec4(aPos, 1.0); // Turns our 3x1 matrix into a 4x1

    //Write on view space. 1 = view space = w
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);

    texCoord = aPos;
}