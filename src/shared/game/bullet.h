#ifndef BULLET_H
#define BULLET_H

#include <raylib.h>
#include <glm/vec2.hpp>
#include <math/rframe.h>
#include <vector>

namespace Game {

    class BulletType 
    {
    private:
        friend class Bullet;
        float ln1MinusDragCoefficient = 0;

    public:

        /// How many seconds the bullet can live before it despawns.
        float maxLifetime = 1;

        /// Constant vertical acceleration applied to the bullet.
        float gravity = 0;

        /// Visible width and height of the bullet's sprite.
        glm::vec2 visibleSize = {50,50};

        Texture2D texture; 
        glm::ivec2 animationFrames = {1,1};

        /// Length of 1 full animation cycle, in seconds.
        float animationLength = 1;

        std::vector<float> sizeKeyframes = {1,1,1,0.5f};
        std::vector<float> alphaKeyframes = {1,1,1,0.5f};

        /** drag coefficient = how much velocity is lost per second.
         * 
         * e.g. 0.5 would cause the projectile to loose half of its velocity each second.
         */
        void SetDrag(float dragCoefficient);
    };

    class Bullet
    {
    private: 
        const BulletType *type;
        float damage = 0;
        float lifetime = 0;

    public:
        ReferenceFrame referenceFrame;

        Bullet();
        Bullet(const BulletType *type, const ReferenceFrame &referenceFrame, float damage);

        void Update(float dt);
        void Draw() const;
        bool IsAlive() const;

        const BulletType *GetType() const;
    };
}
#endif