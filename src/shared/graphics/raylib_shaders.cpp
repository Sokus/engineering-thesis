#include "raylib_shaders.h"

#include <rlgl.h>

namespace Game::RaylibShaders {

    const char *defaultVertexSource = R"glsl(
        #version 330         

        in vec3 vertexPosition;
        in vec2 vertexTexCoord;
        in vec4 vertexColor;   

        out vec2 fragTexCoord; 
        out vec4 fragColor; 

        uniform mat4 mvp;      

        void main() {                                  
            fragTexCoord = vertexTexCoord; 
            fragColor = vertexColor;       
            gl_Position = mvp*vec4(vertexPosition, 1.0);
        }                  
    )glsl";

    Shader world;
    int worldDepthLoc;

    void LoadShaders() {
        world = LoadShaderFromMemory(
            defaultVertexSource,
            R"glsl(
                #version 330       

                in vec2 fragTexCoord;      
                in vec4 fragColor;    

                layout(location=0) out vec4 finalColor; 
                layout(location=1) out float fragDepth;

                uniform sampler2D texture0;
                uniform vec4 colDiffuse;  
                uniform float depth;

                void main() {                                  
                    vec4 texelColor = texture(texture0, fragTexCoord);   
                    finalColor = texelColor*colDiffuse*fragColor;  
                    fragDepth = depth;      
                }                                  
            )glsl"
        );
        worldDepthLoc = GetShaderLocation(world, "depth");
    }
    void UnloadShaders() {
        UnloadShader(world);
    }
}