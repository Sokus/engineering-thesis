#include "kernel.h"

#include <gl/shader.h>
#include <math.h>
#include <stdio.h>
#include "glm/gtc/constants.hpp"

namespace Game {

    GL::ShaderProgram &kernel1DProgram() {
        static auto result = GL::ShaderProgram::ForPostprocessing(R"glsl(
            #version 330 core

            in vec2 v_uv;

            uniform sampler2D tex;
            uniform float kernel[32];
            uniform int kernel_length;
            uniform vec2 sampling_step;

            out vec4 frag_color;

            void main()
            {
                vec2 uv_step = sampling_step / vec2(textureSize(tex,0));
                vec2 uv_offset = ((-kernel_length / 2.0) + 0.5) * uv_step;
                vec4 result = vec4(0.0);
                for(int i=0; i<kernel_length; ++i)
                    result += kernel[i] * texture(tex, v_uv + uv_offset + i*uv_step);
                frag_color = result;
            }
        )glsl");
        return result;
    }

    void Kernel1D::Apply(
        GL::Framebuffer &dst, GL::TextureUnit src,
        const glm::vec2 &step
    ) const {

        dst.BindForDrawing();
        kernel1DProgram()
            .SetUniform("tex", src)
            .SetUniform("kernel", data, count)
            .SetUniform("kernel_length", count)
            .SetUniform("sampling_step", step)
            .DrawPostprocessing();
    }

    void Kernel1D::Normalize() {
        float sum = 0;
        for(int i=0; i<count; ++i) sum += data[i];
        for(int i=0; i<count; ++i) data[i] /= sum;
    }

    Kernel1D Kernel1D::GaussianBlur(float stdev) {

        Kernel1D kernel;

        float minKernelValue = 0.01f;
        float xMax = sqrt(-2*stdev*stdev*log(minKernelValue*stdev*sqrt(2*glm::pi<float>())));
        xMax = glm::clamp(xMax, 0.0f, (float)maxRadius);

        kernel.count = 0;
        for(float x=ceil(-xMax); x<=floor(xMax); x += 1.0f) {
            kernel.data[kernel.count++] = exp(-x*x/(2*stdev*stdev)) / (stdev * sqrt(2*glm::pi<float>()));
        }
        kernel.Normalize();
        return kernel;
    }

    Kernel1D Kernel1D::BoxBlur(int radius) {
        radius = glm::min(radius, maxRadius);
        Kernel1D kernel;
        kernel.count = 2*radius+1;
        for(int i=0; i<kernel.count; ++i) kernel.data[i] = 1;
        kernel.Normalize();
        return kernel;
    }

    int Kernel1D::Count() const {
        return count;
    }
    const float *Kernel1D::Data() const {
        return data;
    }
}