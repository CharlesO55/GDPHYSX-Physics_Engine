#version 330 core //Version
uniform sampler2D tex0;
uniform sampler2D norm_tex;
uniform sampler2D tex2;         //Take in 2nd texture

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 playerFacing;      //Direction player is facing

uniform float ambientStr;
uniform vec3 ambientColor;

uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;

uniform int blendMode;
uniform int lightingMode;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;
in mat3 TBN;

out vec4 FragColor; //Returns a Color

void main()
{
    //STORE TEXTURES
    vec4 pixelColor = texture(tex0, texCoord);
    vec4 pixelColor2 = texture(tex2, texCoord);
    
    //DISCARD ALPHA PARTS
    if(pixelColor.a  < 0.1){
        discard;    //Exits function
    }

    //Flat lighting w/o bump
    //vec3 normal = normalize(normCoord);   //Regular normals w/o bump
    
    //NORMAL MAP
    vec3 normal = texture(norm_tex, texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);         //Converts rgb (0 to 1) into xyz (-1 to 1)
    normal = normalize(TBN * normal);
    
    
    
    
    
    
    vec3 lightDir = normalize(lightPos - fragPos);

    //DIFFUSE
    float diff = max(dot(normal, lightDir), 0.0); //Avoid negative values
    vec3 diffuse = diff * lightColor;

    //AMBIENT
    vec3 ambientCol = ambientColor * ambientStr;

    //SPECULAR
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
    vec3 specColor = spec * specStr * lightColor;


    //MODIFY THESE LIGHTDROP VALUES BASED ON LEARNOPENGL WIKI https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    float intensityConstant = 1.0;          //Keep at 1.0 always
    float intensityLinear = 0.014;         //Smaller values = lower amount of light lost from distance
    float intensityQuadratic = 0.0007;

    float lightDis = length(lightPos - fragPos);                                                                                    //Calculate the distance between light source and object
    float intensityLight = 1.0 / (intensityConstant + (intensityLinear * lightDis) + (intensityQuadratic * (lightDis * lightDis))); //Divide by affluentiation formula where max is 1, and min is limit 0
    

    
    
    //LIGHTING
    vec4 lighting;
    if (lightingMode == 1){          
        lighting = vec4(specColor + ambientCol + diffuse, 1.0) * intensityLight;    //Point light
    }

    else if (lightingMode == -1){                                                   //Spotlight
        float maxAngle = 0.3;                                               
        float innerAngle = dot(lightDir, normalize(playerFacing));    //glm::vec3(1.f, 0.f, 0.f)

        if (innerAngle > maxAngle){                                         
            lighting = vec4(specColor + ambientCol + diffuse, 1.0);         //Light inside normally 
        }
        else{
            lighting = vec4(ambientCol / 3.f, 1.f);                         //Light outside with ambience
        }
    }

    else{                            
        lighting = vec4(1.f);       //Unlit object
    }



    //MULTITEXTURES
    //Blend mode 0 - Base texture and assigned lighting
    //Blend mode 1 - Regular overlay (Overwrite original base with new tex above)
    //Blend mode 2 - Multiply textures

    if (blendMode == 0){
        FragColor = lighting * pixelColor; 
    }

    else if(blendMode == 1){
        FragColor = lighting * pixelColor;    
        if (pixelColor2.a > 0.1){
            FragColor = lighting * pixelColor2;
        }
    }
    else if(blendMode == 2){
        FragColor = lighting * pixelColor * pixelColor2;    
    }

}