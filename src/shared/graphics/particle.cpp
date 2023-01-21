#include "particle.h"
#include <stdio.h>

#ifndef RESOURCE_PATH
#define RESOURCE_PATH ""
#endif

namespace Game {

    int ParticleType::GetID() const {
        return id;
    }

    bool Particle::Serialize(BitStream *stream) {
        return true;
    }

    bool Particle::IsAlive() const {
        return lifetime < type->maxLifetime;
    }
    void Particle::Update(float dt) {
        bounds.x += velocity.x * dt;
        bounds.y += velocity.y * dt;
        rotation += angularVelocity *dt;
        lifetime += dt;
    }
    void Particle::Draw(Texture atlas) const {
        DrawTexturePro(atlas, type->textureBounds, bounds, {.x = bounds.width/2, .y = bounds.height/2}, rotation, WHITE);
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

    ParticleRegistry particleRegistry;



    ParticleSystem::ParticleSystem() {
        particles.reserve(1024);
    }

    void ParticleSystem::LoadTextures() {
        atlas = LoadTexture(RESOURCE_PATH "/particle_atlas.png");
    }
    void ParticleSystem::UnloadTextures() {
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
        for(const auto &particle : particles) 
            particle.Draw(atlas);
    }
}