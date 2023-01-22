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
    int worldEmissivenessLoc;

    void worldSetDepth(float value) {
        SetShaderValue(world, worldDepthLoc, &value, SHADER_UNIFORM_FLOAT);
    }

    void worldSetEmissiveness(float value) {
        SetShaderValue(world, worldEmissivenessLoc, &value, SHADER_UNIFORM_FLOAT);
    }

    Shader particle;
    int particleEmissiveTextureLoc;

    void particleSetEmissiveTexture(Texture2D texture) {
        SetShaderValueTexture(particle, particleEmissiveTextureLoc, texture);
    }

    void LoadShaders() {
        world = LoadShaderFromMemory(
            defaultVertexSource,
            R"glsl(
                #version 330       

                in vec2 fragTexCoord;      
                in vec4 fragColor;    

                layout(location=0) out vec4 fragAlbedo; 
                // we only need to output 1 float, but if we output less than 4 components then alpha
                // is undefined, which messes up the output due to blending
                layout(location=1) out vec4 fragDepth;
                layout(location=2) out vec4 fragEmissive;

                uniform sampler2D texture0;
                uniform vec4 colDiffuse;  
                uniform float depth;
                uniform float emissiveness;

                void main() {                                  
                    vec4 albedo = texture(texture0, fragTexCoord)*colDiffuse*fragColor;   
                    fragAlbedo = albedo;
                    fragDepth = vec4(depth, 0, 0, albedo.a);
                    fragEmissive = vec4(albedo.rgb * emissiveness, albedo.a);
                }                                  
            )glsl"
        );
        worldDepthLoc = GetShaderLocation(world, "depth");
        worldEmissivenessLoc = GetShaderLocation(world, "emissiveness");

        particle = LoadShaderFromMemory(
            defaultVertexSource,
            R"glsl(
                #version 330

                in vec2 fragTexCoord;      
                in vec4 fragColor; 

                layout(location=0) out vec4 fragAlbedo; 
                layout(location=1) out vec4 fragDepth;
                layout(location=2) out vec4 fragEmissive;

                uniform sampler2D texture0;
                uniform sampler2D emissiveTexture;
                uniform vec4 colDiffuse;

                void main() {
                    vec4 albedo = texture(texture0, fragTexCoord)*colDiffuse*fragColor;
                    vec4 emissive = texture(emissiveTexture, fragTexCoord)*colDiffuse*fragColor;
                    fragAlbedo = albedo;
                    fragDepth = vec4(1,0,0,albedo.a);
                    fragEmissive = vec4(emissive.rgb*emissive.a*255, albedo.a);
                }
            )glsl"
        );
        particleEmissiveTextureLoc = GetShaderLocation(particle, "emissiveTexture");
    }
    void UnloadShaders() {
        UnloadShader(world);
        UnloadShader(particle);
    }
}