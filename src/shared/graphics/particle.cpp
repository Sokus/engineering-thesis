#include "particle.h"
#include "macros.h"
#include <serialization/serialize.h>
#include <serialization/serialize_extra.h>
#include <graphics/raylib_shaders.h>
#include "raymath.h"

#ifndef RESOURCE_PATH
#define RESOURCE_PATH ""
#endif

namespace Game {

    float Interpolate(const std::vector<float> &keyframes, float k) {

        if(k <= 0 || keyframes.size() == 1) 
            return keyframes.front();

        k *= keyframes.size()-1;

        if(k >= keyframes.size()-1) 
            return keyframes.back();

        return Lerp(keyframes[floor(k)], keyframes[ceil(k)], k-floor(k));
    }

    Rectangle RectangleScale(const Rectangle &r, float scale) {
        return {
            .x = r.x,
            .y = r.y,
            .width  = r.width  * scale,
            .height = r.height * scale
        };
    }

    int ParticleType::GetID() const {
        return id;
    }

    bool Particle::Serialize(BitStream *stream) {
        
        int typeId = type == nullptr ? 0 : type->GetID();
        SERIALIZE_INT(stream, typeId, 0, particleRegistry.Size()+1);
        type = particleRegistry.GetParticleTypeByID(typeId);

        if(type != nullptr) {
            SERIALIZE_RECTANGLE(stream, bounds);
            SERIALIZE_VECTOR2(stream, velocity);
            SERIALIZE_FLOAT(stream, rotation);
            SERIALIZE_FLOAT(stream, angularVelocity);
            SERIALIZE_FLOAT(stream, lifetime);
        }

        return true;
    }

    bool Particle::IsAlive() const {
        return lifetime < type->maxLifetime;
    }
    void Particle::Update(float dt) {
        bounds.x += velocity.x * dt;
        bounds.y += velocity.y * dt;
        rotation += angularVelocity * dt;
        velocity.y += type->gravity * dt;
        lifetime += dt;
    }

    void Particle::Draw(Texture atlas) const {

        const float T = lifetime / type->maxLifetime;

        Rectangle dest = RectangleScale(bounds, Interpolate(type->sizeKeyframes, T));
        Color tint = ColorAlpha(WHITE, Interpolate(type->alphaKeyframes, T));

        DrawTexturePro(atlas, type->textureBounds, dest, {.x = dest.width/2, .y = dest.height/2}, rotation, tint);
    }


    ParticleRegistry::~ParticleRegistry() {
        for(auto type : particleTypes) delete type;
    }

    ParticleType *ParticleRegistry::Register(ParticleType *type) {
        particleTypes.push_back(type);
        type->id = (int) particleTypes.size();
        return type;
    }

    ParticleType *ParticleRegistry::CreateParticleType() {
        return Register(new ParticleType());
    }

    ParticleType *ParticleRegistry::CopyParticleType(const ParticleType *other) {
        if(other == nullptr) {
            return CreateParticleType();
        } else {
            return Register(new ParticleType(*other));
        }
    }
    
    const ParticleType *ParticleRegistry::GetParticleTypeByID(int id) const {
        --id;
        if(id < 0 || id >= particleTypes.size()) return nullptr;
        else return particleTypes[id];
    }

    int ParticleRegistry::Size() const {
        return particleTypes.size();
    }

    ParticleRegistry particleRegistry;



    ParticleSystem::ParticleSystem() {
        particles.reserve(1024);
    }

    void ParticleSystem::LoadTextures() {
        atlas = LoadTexture(RESOURCE_PATH "/particle_atlas.png");
        atlasEmissive = LoadTexture(RESOURCE_PATH "/particle_atlas_emissive.png");
    }
    void ParticleSystem::UnloadTextures() {
        UnloadTexture(atlasEmissive);
        UnloadTexture(atlas);
    }

    void ParticleSystem::AddParticle(Particle particle) {
        particles.push_back(particle);
    }

    void ParticleSystem::ClearParticles() {
        particles.clear();
    }

    void ParticleSystem::Update(float dt) {
        int o=0;
        for(int i=0; i<particles.size(); ++i) {
            particles[i].Update(dt);
            if(particles[i].IsAlive()) {
                particles[o++] = particles[i];
            }
        }
        particles.resize(o);
    }

    void ParticleSystem::Draw() const {
        
        BeginShaderMode(RaylibShaders::particle);
        RaylibShaders::particleSetEmissiveTexture(atlasEmissive);

        for(const auto &particle : particles) 
            particle.Draw(atlas);

        EndShaderMode();
    }
}