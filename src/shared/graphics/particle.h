#ifndef GRAPHICS_PARTICLE_H
#define GRAPHICS_PARTICLE_H

#include <raylib.h>
#include <vector>
#include <serialization/bitstream.h>

namespace Game {

    struct ParticleRegistry;

    struct ParticleType {
        private:

        int id = 0;

        ParticleType() = default;
        ParticleType(const ParticleType &) = default;

        friend struct ParticleRegistry;

        public:

        Rectangle textureBounds;
        float maxLifetime = 1;
        float gravity = 0;

        int GetID() const;
    };

    struct Particle {

        const ParticleType *type = nullptr;
        Rectangle bounds;
        Vector2 velocity = {0.0f, 0.0f};
        float rotation = 0;
        float angularVelocity = 0;
        float lifetime = 0;

        bool Serialize(BitStream *stream);
        bool IsAlive() const;
        void Update(float dt);
        void Draw(Texture atlas) const;
    };

    struct ParticleRegistry {
        private:

        std::vector<const ParticleType *> particleTypes;
        ParticleType *Register(ParticleType *type);

        public:
        ~ParticleRegistry();

        ParticleType *CreateParticleType();
        ParticleType *CopyParticleType(const ParticleType *other);
        const ParticleType *GetParticleTypeByID(int id) const;
        int Size() const;
    };

    extern ParticleRegistry particleRegistry;

    struct ParticleSystem {
        private:

        std::vector<Particle> particles;
        Texture atlas;
        Texture atlasEmissive;

        public:

        ParticleSystem();
        
        void LoadTextures();
        void UnloadTextures();

        void AddParticle(Particle particle);
        void ClearParticles();
        void Update(float dt);
        void Draw() const;
    };
}

#endif