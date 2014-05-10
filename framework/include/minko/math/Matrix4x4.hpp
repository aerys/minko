/*
Copyright (c) 2013 Aerys

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

#include "minko/Common.hpp"
#include "minko/data/Value.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Quaternion.hpp"

namespace minko
{
	namespace math
	{
		class Matrix4x4 :
			public std::enable_shared_from_this<Matrix4x4>,
			public data::Value,
			public Convertible<Matrix4x4>
		{
			friend component::Transform;

		public:
			typedef std::shared_ptr<Matrix4x4>	Ptr;

		private:
			typedef std::shared_ptr<Vector3>	Vector3Ptr;
			typedef std::shared_ptr<Quaternion>	QuaternionPtr;

		private:
			std::vector<float>	_m;
			bool				_lock;
			bool				_hasChanged;

		public:
			inline static
			Ptr
			create()
			{
				auto m = std::shared_ptr<Matrix4x4>(new Matrix4x4());

				m->identity();

				return m;
			}

			inline static
			Ptr
			create(Ptr value)
			{
				return std::shared_ptr<Matrix4x4>(new Matrix4x4(value));
			}

			inline
			std::vector<float>&
			data()
			{
				return _m;
			}

			inline
			const std::vector<float>&
			data() const
			{
				return _m;
			}

			Ptr
			initialize(float m00, float m01, float m02, float m03,
				   	   float m10, float m11, float m12, float m13,
				       float m20, float m21, float m22, float m23,
				       float m30, float m31, float m32, float m33);

			Ptr
			initialize(std::vector<float> m);

			Ptr
			initialize(Quaternion::Ptr, Vector3::Ptr);

			inline
			const std::vector<float>&
			values() const
			{
				return _m;
			}

			std::string
			toString()
			{
				std::stringstream stream;

				stream << "(" << _m[0] << ", " << _m[1] << ", " << _m[2] << ", " << _m[3] << ",\n"
					<< _m[4] << ", " << _m[5] << ", " << _m[6] << ", " << _m[7] << ",\n"
					<< _m[8] << ", " << _m[9] << ", " << _m[10] << ", " << _m[11] << ",\n"
					<< _m[12] << ", " << _m[13] << ", " << _m[14] << ", " << _m[15] << ")";

				return stream.str();
			}

			Ptr
			identity();

			Ptr
			append(Ptr matrix);

			Ptr
			prepend(Ptr matrix);

			Ptr
			appendTranslation(float x, float y = 0.f, float z = 0.f);

			Ptr
			prependTranslation(float x, float y = 0.f, float z = 0.f);

			inline
			Ptr
			appendTranslation(Vector3Ptr value)
			{
				return appendTranslation(value->x(), value->y(), value->z());
			}

			inline
			Ptr
			prependTranslation(Vector3Ptr value)
			{
				return prependTranslation(value->x(), value->y(), value->z());
			}

			Ptr
			appendRotationX(float radians);

			Ptr
			prependRotationX(float radians);

			Ptr
			appendRotationY(float radians);

			Ptr
			prependRotationY(float radians);

			Ptr
			appendRotationZ(float radians);

			Ptr
			prependRotationZ(float radians);

			Ptr
			appendRotation(float radians, Vector3::Ptr axis);

			Ptr
			prependRotation(float radians, Vector3::Ptr axis);

			Ptr
			append(Quaternion::Ptr);

			Ptr
			prepend(Quaternion::Ptr);

			Ptr
			appendScale(float x, float y, float z);

			Ptr
			prependScale(float x, float y, float z);

			inline
			Ptr
			appendScale(float scale)
			{
				return appendScale(scale, scale, scale);
			}

			inline
			Ptr
			prependScale(float scale)
			{
				return prependScale(scale, scale, scale);
			}

			float
			determinant() const;

			float
			determinant3x3() const;

			std::pair<Ptr, Ptr>
			decomposeQR(Ptr matrixQ = nullptr, Ptr matrixR = nullptr) const;

			std::pair<QuaternionPtr, Ptr>
			decomposeQR(QuaternionPtr quaternion = nullptr, Ptr matrixR = nullptr) const;

			void
			decompose(Vector3Ptr	translation,
					  QuaternionPtr	rotation,
					  Vector3Ptr	scaling) const;

			Ptr
			recompose(Vector3Ptr	translation,
					  QuaternionPtr	rotation,
					  Vector3Ptr	scaling);

			Ptr
			interpolateTo(Ptr	target, 
						  float	ratio);

			Ptr
			invert();

			Ptr
			transpose();

			inline
			std::shared_ptr<Vector3>
			transform(std::shared_ptr<Vector3> v) const
			{
				return transform(v, nullptr);
			}

            std::shared_ptr<Vector3>
            transform(std::shared_ptr<Vector3> v, std::shared_ptr<Vector3> output) const;

			inline
			std::shared_ptr<Vector3>
			deltaTransform(std::shared_ptr<Vector3> v) const
			{
				return deltaTransform(v, nullptr);
			}

			std::shared_ptr<Vector3>
            deltaTransform(std::shared_ptr<Vector3> v, std::shared_ptr<Vector3> output) const;

			Ptr
			lock();

			Ptr
			unlock();

			inline
			Ptr
			operator*(Ptr value)
			{
				auto m1 = Matrix4x4::create(shared_from_this());

				m1->prepend(value);

				return m1;
			}

			inline
			Ptr
			operator*=(Matrix4x4::Ptr value)
			{
				prepend(value);

				return shared_from_this();
			}
			
			inline
			bool
			operator==(Matrix4x4& value)
			{
				return _m == value._m;
			}

			Ptr
			perspective(float fov, float ratio, float zNear, float zFar);

			Ptr
			orthographic(float left, float right, float top, float bottom, float zNear, float zFar);

			/**
			 * Builds a (left-handed) view transform.
			 * <br /><br />
			 * Eye : eye position, At : eye direction, Up : up vector
			 * <br /><br />
			 * zaxis = normal(At - Eye)<br />
			 * xaxis = normal(cross(Up, zaxis))<br />
			 * yaxis = cross(zaxis, xaxis)<br />
			 * <br />
			 * [      xaxis.x          yaxis.x            zaxis.x  	     0 ]<br />
			 * [      xaxis.y          yaxis.y            zaxis.y        0 ]<br />
			 * [      xaxis.z          yaxis.z            zaxis.z        0 ]<br />
			 * [ -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)    1 ]<br />
			 *
			 * @return Returns a left-handed view Matrix3D to convert world coordinates into eye coordinates
			 *
			 */
			Ptr
			view(Vector3::Ptr eye, Vector3::Ptr lookAt, Vector3::Ptr up = 0);

			Ptr
			lookAt(Vector3::Ptr lookAt, Vector3::Ptr position, Vector3::Ptr up = 0);

			Ptr
			lerp(Matrix4x4::Ptr target, float ratio);

			Ptr
			fromQuaternion(Quaternion::Ptr quaternion);

			Quaternion::Ptr
			rotationQuaternion(Quaternion::Ptr output = nullptr) const;

			inline
			Vector3::Ptr
			translation() const
			{
				return Vector3::create(_m[3], _m[7], _m[11]);
			}

			inline
			Ptr
			translation(float x, float y, float z)
			{
				_m[3] = x;
				_m[7] = y;
				_m[11] = z;

				changed()->execute(shared_from_this());
				_hasChanged = true;		

				return shared_from_this();
			}

			inline
			Ptr
			translation(Vector3::Ptr t)
			{
				translation(t->x(), t->y(), t->z());

				return shared_from_this();
			}

			inline
			void
			copyTranslation(Vector3::Ptr t)
			{
				t->setTo(_m[3], _m[7], _m[11]);
			}

			inline
			bool
			equals(Ptr matrix)
			{
				return _m == matrix->_m;
			}

			Ptr
			copyFrom(Matrix4x4::Ptr source);

		private:
			Matrix4x4();

			Matrix4x4(Ptr value);

			inline
			Ptr
			append(float m00, float m01, float m02, float m03,
				   float m10, float m11, float m12, float m13,
				   float m20, float m21, float m22, float m23,
				   float m30, float m31, float m32, float m33);

			inline
			Ptr
			prepend(float m00, float m01, float m02, float m03,
				   	float m10, float m11, float m12, float m13,
				   	float m20, float m21, float m22, float m23,
				   	float m30, float m31, float m32, float m33);
		};
	}
}
