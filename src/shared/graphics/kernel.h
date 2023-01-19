#ifndef GRAPHICS_KERNEL_H
#define GRAPHICS_KERNEL_H

#include <gl/framebuffer.h>
#include <gl/texture.h>
#include <glm/vec2.hpp>

namespace Game {

    struct Kernel1D {
        private:

        static const int maxRadius = 15;

        float data[2*maxRadius+1];
        int count = 0;

        void Normalize();

        public:

        static Kernel1D GaussianBlur(float stdev);
        static Kernel1D BoxBlur(int radius);

        void Apply(
            GL::Framebuffer &dst, GL::TextureUnit src,
            const glm::vec2 &step
        ) const;

        int Count() const;
        const float *Data() const;
    };
}

#endif