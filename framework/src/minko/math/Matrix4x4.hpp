#pragma once

#include "minko/Common.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"

namespace minko
{
	namespace math
	{
		class Matrix4x4 :
			public std::enable_shared_from_this<Matrix4x4>
		{
		private:
			std::vector<float> _m;

		public:
			typedef std::shared_ptr<Matrix4x4>	ptr;

			inline static
			ptr
			create()
			{
				auto m = std::shared_ptr<Matrix4x4>(new Matrix4x4());

				m->identity();

				return m;
			}

			inline static
			ptr
			create(ptr value)
			{
				return std::shared_ptr<Matrix4x4>(new Matrix4x4(value));
			}

			ptr
			initialize(float m00, float m01, float m02, float m03,
				   	   float m10, float m11, float m12, float m13,
				       float m20, float m21, float m22, float m23,
				       float m30, float m31, float m32, float m33);

			ptr
			translation(float x, float y, float z);

			ptr
			rotationX(float radians);

			ptr
			rotationY(float radians);
			
			ptr
			rotationZ(float radians);

			inline
			const std::vector<float>
			values()
			{
				return _m;
			}

			ptr
			identity();

			ptr
			append(ptr matrix);

			ptr
			prepend(ptr matrix);

			ptr
			appendTranslation(float x, float y, float z);

			ptr
			prependTranslation(float x, float y, float z);

			ptr
			appendRotation(Vector4::ptr quaternion);

			ptr
			prependRotation(Vector4::ptr quaternion);

			ptr
			appendRotation(float radians, Vector3::ptr axis);

			ptr
			prependRotation(float radians, Vector3::ptr axis);

			float
			determinant();

			ptr
			invert();

			inline
			ptr
			operator*(ptr value)
			{
				Matrix4x4::ptr m1 = Matrix4x4::create(shared_from_this());

				m1->append(value);

				return m1;
			}

			inline
			ptr
			operator*=(Matrix4x4::ptr value)
			{
				append(value);

				return shared_from_this();
			}

			inline
			bool
			operator==(Matrix4x4& value)
			{
				std::vector<float> m = value._m;

				for (auto i = 0; i < 16; ++i)
					if (_m[i] != m[i])
						return false;

				return true;
			}

			ptr
			perspectiveFoV(float fov,
		                   float ratio,
		                   float zNear,
		                   float zFar);

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
			ptr
			view(Vector3::const_ptr eye, Vector3::const_ptr lookAt, Vector3::const_ptr up = 0);

			ptr
			lookAt(Vector3::const_ptr lookAt, Vector3::const_ptr position, Vector3::const_ptr up = 0);

			ptr
			lerp(Matrix4x4::ptr target, float ratio);

			Vector3::ptr
			translation(Vector3::ptr output = 0);

			ptr
			copyFrom(Matrix4x4::ptr source);

		private:
			Matrix4x4();

			Matrix4x4(ptr value);

			inline
			ptr
			append(float m00, float m01, float m02, float m03,
				   float m10, float m11, float m12, float m13,
				   float m20, float m21, float m22, float m23,
				   float m30, float m31, float m32, float m33);

			inline
			ptr
			prepend(float m00, float m01, float m02, float m03,
				   	float m10, float m11, float m12, float m13,
				   	float m20, float m21, float m22, float m23,
				   	float m30, float m31, float m32, float m33);
		};
	}
}

namespace std
{
	template<typename Matrix4x4>
	string
	to_string(Matrix4x4 value)
	{
		const std::vector<float> values = value.values();
		std::stringstream stream;

		stream << "(" << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3] << ", "
			<< values[4] << ", " << values[5] << ", " << values[6] << ", " << values[7] << ", "
			<< values[8] << ", " << values[9] << ", " << values[10] << ", " << values[11] << ", "
			<< values[12] << ", " << values[13] << ", " << values[14] << ", " << values[15] << ")";

		return stream.str();
	}
}
