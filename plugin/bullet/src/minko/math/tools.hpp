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
        removeScalingShear(const math::mat4& input, minko::math::mat4& correction);

        inline
        std::pair<math::mat4, math::mat4>
        decomposeQR(const minko::math::mat4& matA);

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
minko::math::removeScalingShear(const minko::math::mat4& input, minko::math::mat4& correction)
{
    static auto matrix = math::mat4();

    auto translation = input[3];
    matrix = math::translate(math::vec3(-translation)) * input; // Remove translation

    auto qr = math::decomposeQR(matrix);
    auto output = qr.first;
    correction = qr.second;

    return math::translate(math::vec3(translation)) * output;
}

inline
std::pair<minko::math::mat4, minko::math::mat4>
minko::math::decomposeQR(const minko::math::mat4& matA)
{
    auto jColumn = math::vec4();
    auto accProj = math::vec4();

    auto projVec = std::vector<double>(16, 0.f);
    auto rValues = std::vector<double>(16, 0.f);

    for (unsigned int j = 0; j < 4; ++j)
    {
        // jth column
        jColumn = matA[j];
        accProj = { 0.f, 0.f, 0.f, 0.f };

        for (unsigned int i = 0; i < j; ++i)
        {
            const double dot =
                projVec[4 * i] * jColumn[0] +
                projVec[4 * i + 1] * jColumn[1] +
                projVec[4 * i + 2] * jColumn[2] +
                projVec[4 * i + 3] * jColumn[3];

            for (unsigned int k = 0; k < 4; ++k)
                accProj[k] += dot * projVec[4 * i + k];
        }

        double squaredLength = 0.0;
        for (unsigned int k = 0; k < 4; ++k)
        {
            const double diff = jColumn[k] - accProj[k];

            projVec[4 * j + k] = diff;
            squaredLength += diff * diff;
        }

        const double invLength = squaredLength > 1e-6 ? 1.f / math::sqrt(squaredLength) : 0.f;

        for (unsigned int k = 0; k < 4; ++k)
            projVec[4 * j + k] *= invLength;

        for (unsigned int i = 0; i <= j; ++i)
        {
            const double dot =
                projVec[4 * i] * jColumn[0] +
                projVec[4 * i + 1] * jColumn[1] +
                projVec[4 * i + 2] * jColumn[2] +
                projVec[4 * i + 3] * jColumn[3];

            rValues[j + 4 * i] = dot;
        }
    }

    auto matQ = math::mat4(
        projVec[0], projVec[1], projVec[2], projVec[3],
        projVec[4], projVec[5], projVec[6], projVec[7],
        projVec[8], projVec[9], projVec[10], projVec[11],
        projVec[12], projVec[13], projVec[14], projVec[15]
    );

    auto matR = math::mat4(
        rValues[0], rValues[4], rValues[8], rValues[12],
        rValues[1], rValues[5], rValues[9], rValues[13],
        rValues[2], rValues[6], rValues[10], rValues[14],
        rValues[3], rValues[7], rValues[11], rValues[15]
    );

    if (math::determinant(math::mat3(matQ)) < 0.f)
    {
        // Important: account for possible reflection in the Q orthogonal matrix
        matQ[0][0] *= -1.f;
        matQ[0][1] *= -1.f;
        matQ[0][2] *= -1.f;
        matQ[0][3] *= -1.f;

        matR[0][0] *= -1.f;
        matR[1][0] *= -1.f;
        matR[2][0] *= -1.f;
        matR[3][0] *= -1.f;
    }

    return std::make_pair(matQ, matR);
}


inline
minko::math::mat4
minko::math::fromBulletTransform(const btTransform& transform)
{
    auto basis = transform.getBasis();
    auto translation = transform.getOrigin();

    return minko::math::mat4(
        basis[0][0], basis[1][0], basis[2][0], 0.f,
        basis[0][1], basis[1][1], basis[2][1], 0.f,
        basis[0][2], basis[1][2], basis[2][2], 0.f,
        translation[0], translation[1], translation[2], 1.f
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