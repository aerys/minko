/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <iostream>
#include <LinearMath/btTransform.h>

namespace minko
{
    namespace math
    {
        inline
        btVector3
        convert(const math::vec3&);

        inline
        math::mat4
        removeScalingShear(const math::mat4& input, const math::mat4& output, const minko::math::mat4& correction);

        inline
        math::mat4
        fromBulletTransform(const btTransform&);

        inline
        btTransform&
        toBulletTransform(const math::mat4&, btTransform&);

        inline
        btTransform&
        toBulletTransform(const math::quat&, const math::vec3&, btTransform&);

        inline
        std::ostream&
        print(std::ostream&, const btTransform&);
    }
}

inline
btVector3
minko::math::convert(const math::vec3& value)
{
    return btVector3(value.x, value.y, value.z);
}

inline
minko::math::mat4
minko::math::removeScalingShear(const minko::math::mat4& input,
                                const minko::math::mat4& output,
                                const minko::math::mat4& correction)
{
    static auto matrix = math::mat4();

    // Beta 2 implementation
    /* 
    // remove translational component, then perform QR decomposition
    auto translation = input->translation();
    matrix
        ->copyFrom(input)
        ->appendTranslation(-(*translation))
        ->decomposeQR(output, correction);

    return output->appendTranslation(translation);
    */

    // Beta 3 equivalent
    auto translation = input[3];
    matrix = math::translate(math::vec3(-translation)) * input;
    // TODO
    //->decomposeQR(output, correction);

    return matrix;
}


inline
minko::math::mat4
minko::math::fromBulletTransform(const btTransform& transform)
{
    auto basis = transform.getBasis();
    auto translation = transform.getOrigin();

    return minko::math::mat4(
        basis[0][0], basis[0][1], basis[0][2], translation[0],
        basis[1][0], basis[1][1], basis[1][2], translation[1],
        basis[2][0], basis[2][1], basis[2][2], translation[2],
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

inline
btTransform&
minko::math::toBulletTransform(const minko::math::mat4& transform, btTransform& output)
{
    return toBulletTransform(
        math::quat(transform),
        math::vec3(transform[3]),
        output
    );
}

inline
btTransform&
minko::math::toBulletTransform(const minko::math::quat& rotation,
                               const minko::math::vec3& translation,
                               btTransform& output)
{
    btQuaternion btRotation(rotation.x, rotation.y, rotation.z, rotation.w);
    btVector3 btOrigin(translation.x, translation.y, translation.z);

    output.setOrigin(btOrigin);
    output.setRotation(btRotation);

    return output;
}

inline
std::ostream&
minko::math::print(std::ostream& out, const btTransform& bulletTransform)
{
    const btVector3& origin (bulletTransform.getOrigin());
    const btMatrix3x3& basis (bulletTransform.getBasis());

    out << "\t- origin\t= [" << origin[0] << "\t" << origin[1] << "\t" << origin[2] << "]\n\t- basis \t=\n"
        << "\t[" << basis[0][0] << "\t" << basis[0][1] << "\t" << basis[0][2]
        << "\n\t " << basis[1][0] << "\t" << basis[1][1] << "\t" << basis[1][2]
        << "\n\t " << basis[2][0] << "\t" << basis[2][1] << "\t" << basis[2][2] << "]";

    return out;
}