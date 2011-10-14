package aerys.minko.type.math
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.Factory;
	import aerys.minko.type.IVersionable;
	
	import flash.geom.Matrix3D;
	import flash.geom.Utils3D;
	import flash.geom.Vector3D;

	use namespace minko;
		
	public final class Matrix4x4 implements IVersionable
	{
		
		private static const FACTORY			: Factory			= Factory.getFactory(Matrix4x4);
		private static const RAD2DEG			: Number			= 180. / Math.PI;
		private static const DEG2RAD			: Number			= Math.PI / 180.;
		private static const EPSILON			: Number			= 1e-100;

		private static const TMP_VECTOR			: Vector.<Number>	= new Vector.<Number>();
		private static const TMP_VECTOR4		: Vector4			= new Vector4();
		private static const TMP_MATRIX			: Matrix4x4			= new Matrix4x4();
		
		private static const UPDATE_NONE		: uint				= 0;
		private static const UPDATE_DATA		: uint				= 1;
		private static const UPDATE_MATRIX		: uint				= 2;
		private static const UPDATE_ALL			: uint				= UPDATE_DATA | UPDATE_MATRIX;

		private var _version	: uint					= 0;
		private var _data		: Vector.<Number>		= new Vector.<Number>();
		private var _numPushes	: int					= 0;
		minko var _matrix		: flash.geom.Matrix3D	= new flash.geom.Matrix3D();

		public function get version()		: uint		{ return _version; }
		public function get translationX()	: Number	{ return getTranslation(TMP_VECTOR4).x; }
		public function get translationY()	: Number	{ return getTranslation(TMP_VECTOR4).y; }
		public function get translationZ()	: Number	{ return getTranslation(TMP_VECTOR4).z; }
		public function get rotationX()		: Number	{ return getRotation(TMP_VECTOR4).x; }
		public function get rotationY()		: Number	{ return getRotation(TMP_VECTOR4).y; }
		public function get rotationZ()		: Number	{ return getRotation(TMP_VECTOR4).z; }

		public function set translationX(value : Number) : void
		{
			setTranslation(value, NaN, NaN);
		}

		public function set translationY(value : Number) : void
		{
			setTranslation(NaN, value, NaN);
		}

		public function set translationZ(value : Number) : void
		{
			setTranslation(NaN, NaN, value);
		}

		public function set rotationX(value : Number) : void
		{
			setRotation(value, NaN, NaN);
		}

		public function set rotationY(value : Number) : void
		{
			setRotation(NaN, value, NaN);
		}

		public function set rotationZ(value : Number) : void
		{
			setRotation(NaN, NaN, value);
		}

		public function Matrix4x4(m11 : Number	= 1., m12 : Number	= 0., m13 : Number	= 0., m14 : Number	= 0.,
								  m21 : Number	= 0., m22 : Number	= 1., m23 : Number	= 0., m24 : Number	= 0.,
								  m31 : Number	= 0., m32 : Number	= 0., m33 : Number	= 1., m34 : Number	= 0.,
								  m41 : Number	= 0., m42 : Number	= 0., m43 : Number	= 0., m44 : Number	= 1.)
		{
			initialize(m11, m12, m13, m14,
					   m21, m22, m23, m24,
					   m31, m32, m33, m34,
					   m41, m42, m43, m44);
		}

		private function initialize(m11 : Number, m12 : Number, m13 : Number, m14 : Number,
									m21 : Number, m22 : Number, m23 : Number, m24 : Number,
									m31 : Number, m32 : Number, m33 : Number, m34 : Number,
									m41 : Number, m42 : Number, m43 : Number, m44 : Number) : void
		{
			TMP_VECTOR.length = 0;
			TMP_VECTOR.push(m11, m12, m13, m14,
							m21, m22, m23, m24,
							m31, m32, m33, m34,
							m41, m42, m43, m44);

			_matrix.copyRawDataFrom(TMP_VECTOR);
		}
		
		public function push() : Matrix4x4
		{
			_matrix.copyRawDataTo(_data, _numPushes * 16);
			_numPushes++;

			return this;
		}
		
		public function pop() : Matrix4x4
		{
			if (_numPushes == 0)
				return this;
			
			_numPushes--;
			_matrix.copyRawDataFrom(_data, _numPushes * 16);
			++_version;

			return this;
		}
		
		public function prepend(m : Matrix4x4) : Matrix4x4
		{
			_matrix.prepend(m._matrix);
			++_version;

			return this;
		}

		public function append(m : Matrix4x4) : Matrix4x4
		{
			_matrix.append(m._matrix);
			++_version;

			return this;
		}

		public function appendRotation(radians		: Number,
									   axis			: Vector4,
									   pivotPoint	: Vector4	= null) : Matrix4x4
		{
			_matrix.appendRotation(radians * RAD2DEG,
								   axis._vector,
								   pivotPoint ? pivotPoint._vector : null);
			++_version;

			return this;
		}

		public function appendScale(x	: Number,
									y	: Number	= 1.,
									z	: Number	= 1.) : Matrix4x4
		{
			_matrix.appendScale(x, y, z);
			++_version;

			return this;
		}
		
		public function appendUniformScale(scale : Number) : Matrix4x4
		{
			_matrix.appendScale(scale, scale, scale);
			++_version;

			return this;
		}

		public function appendTranslation(x : Number,
										  y : Number = 0.,
										  z : Number = 0.) : Matrix4x4
		{
			_matrix.appendTranslation(x, y, z);
			++_version;

			return this;
		}

		public function prependRotation(radians		: Number,
										axis		: Vector4,
										pivotPoint	: Vector4 = null) : Matrix4x4
		{
			_matrix.prependRotation(radians * RAD2DEG,
									axis._vector,
									pivotPoint ? pivotPoint._vector : null);
			++_version;

			return this;
		}

		public function prependScale(x	: Number,
									 y	: Number = 1.,
									 z	: Number = 1.) : Matrix4x4
		{
			_matrix.prependScale(x, y, z);
			++_version;

			return this;
		}

		public function prependTranslation(x : Number,
										   y : Number	= 1.,
										   z : Number	= 1.) : Matrix4x4
		{
			_matrix.prependTranslation(x, y, z);
			++_version;

			return this;
		}

		public function transformVector(input 	: Vector4,
									    output	: Vector4	= null) : Vector4
		{
			var v : Vector3D = _matrix.transformVector(input._vector);

			output ||= new Vector4();
			output.set(v.x, v.y, v.z, v.w);

			return output;
		}

		public function deltaTransformVector(input 	: Vector4,
											 output	: Vector4	= null) : Vector4
		{
			var v : Vector3D = _matrix.deltaTransformVector(input._vector);

			output ||= new Vector4();
			output.set(v.x, v.y, v.z, v.w);

			return output;
		}

		public function transformRawVectors(input 	: Vector.<Number>,
										    output	: Vector.<Number> = null) : Vector.<Number>
		{
			output ||= new Vector.<Number>();
			_matrix.transformVectors(input, output);

			return output;
		}
		
		public function identity() : Matrix4x4
		{
			_matrix.identity();

			++_version;

			return this;
		}
		
		public function invert() : Matrix4x4
		{
			_matrix.invert();

			++_version;

			return this;
		}
		
		public function transpose() : Matrix4x4
		{
			_matrix.transpose();

			++_version;

			return this;
		}

		public function projectVector(input 	: Vector4,
									  output	: Vector4 = null) : Vector4
		{
			var v : Vector3D = Utils3D.projectVector(_matrix, input._vector);

			output ||= new Vector4();
			output.set(v.x, v.y, v.z, v.w);

			return output;
		}

		public function getRawData(out 			: Vector.<Number> = null,
								   offset		: int			  = 0,
								   transposed	: Boolean 		  = false) : Vector.<Number>
		{
			out ||= new Vector.<Number>();
			_matrix.copyRawDataTo(out, offset, transposed);

			return out;
		}

		public function setRawData(input		: Vector.<Number>,
								   offset		: int		= 0,
								   transposed	: Boolean	= false) : Matrix4x4
		{
			_matrix.copyRawDataFrom(input, offset, transposed);

			++_version;

			return this;
		}

		public function pointAt(pos	: Vector4,
								at	: Vector4	= null,
								up	: Vector4	= null) : Matrix4x4
		{
			_matrix.pointAt(pos._vector,
							at ? at._vector : null,
							up ? up._vector : null);

			++_version;

			return this;
		}
		
		public function interpolateTo(target : Matrix4x4, percent : Number) : Matrix4x4
		{
			_matrix.interpolateTo(target._matrix, percent);

			++_version;

			return this;
		}

		public function projectVectors(input 	: Vector.<Number>,
									   output	: Vector.<Number>,
									   uvt		: Vector.<Number>) : void
		{
			Utils3D.projectVectors(_matrix, input, output, uvt);
		}
		
		public function getRotation(output : Vector4 = null) : Vector4
		{
			var components 	: Vector.<Vector3D>	= _matrix.decompose();

			output ||= new Vector4();
			output._vector = components[1];

			return output;
		}
		
		public function setRotation(x : Number, y : Number, z : Number) : Matrix4x4
		{
			var components 	: Vector.<Vector3D>	= _matrix.decompose();
			var rotation	: Vector3D			= components[1];

			if (!isNaN(x))
				rotation.x = x;
			if (!isNaN(y))
				rotation.y = y;
			if (!isNaN(z))
				rotation.z = z;

			++_version;
			_matrix.recompose(components);

			return this;
		}

		public function getTranslation(output : Vector4 = null) : Vector4
		{
			output ||= new Vector4();
			_matrix.copyColumnTo(3, output._vector);

			return output;
		}
		
		public function setTranslation(x : Number, y : Number, z : Number) : Matrix4x4
		{
			var position : Vector4	= getTranslation(TMP_VECTOR4);
			
			if (!isNaN(x))
				position.x = x;
			if (!isNaN(y))
				position.y = y;
			if (!isNaN(z))
				position.z = z;
			
			_matrix.copyColumnFrom(3, position._vector);
			++_version;
			
			return this;
		}
		
		public function getScale(output : Vector4 = null) : Vector4
		{
			var components 	: Vector.<Vector3D>	= _matrix.decompose();
			
			output ||= new Vector4();
			output._vector = components[2];
			
			return output;
		}
		
		public function setScale(x : Number, y : Number, z : Number) : Matrix4x4
		{
			var components 	: Vector.<Vector3D>	= _matrix.decompose();
			var scale		: Vector3D			= components[2];
			
			if (!isNaN(x))
				scale.x = x;
			if (!isNaN(y))
				scale.y = y;
			if (!isNaN(z))
				scale.z = z;
			
			++_version;
			_matrix.recompose(components);
			
			return this;
		}

		public function toDualQuaternion(n : Vector4,
										 d : Vector4) : void
		{
			var m : Vector.<Number> = TMP_VECTOR
			_matrix.copyRawDataTo(m, 0, true);

			var mTrace	: Number = m[0] + m[5] + m[10];
			var s		: Number;
			var nw		: Number;
			var nx		: Number;
			var ny		: Number;
			var nz		: Number;

			if (mTrace > 0)
			{
				s = 2.0 * Math.sqrt(mTrace + 1.0);
				nw = 0.25 * s;
				nx = (m[9] - m[6]) / s;
				ny = (m[2] - m[8]) / s;
				nz = (m[4] - m[1]) / s;
			}
			else if (m[0] > m[5] && m[0] > m[10])
			{
				s = 2.0 * Math.sqrt(1.0 + m[0] - m[5] - m[10]);

				nw = (m[9] - m[6]) / s
				nx = 0.25 * s;
				ny = (m[1] + m[4]) / s;
				nz = (m[2] + m[8]) / s;
			}
			else if (m[5] > m[10])
			{
				s = 2.0 * Math.sqrt(1.0 + m[5] - m[0] - m[10]);

				nw = (m[2] - m[8]) / s;
				nx = (m[1] + m[4]) / s;
				ny = 0.25 * s;
				nz = (m[6] + m[9]) / s;
			}
			else
			{
				s = 2.0 * Math.sqrt(1.0 + m[10] - m[0] - m[5]);

				nw = (m[4] - m[1]) / s;
				nx = (m[2] + m[8]) / s;
				ny = (m[6] + m[9]) / s;
				nz = 0.25 * s;
			}

			var dw : Number = -	0.5 * (	 m[3] * nx + m[7] * ny + m[11] * nz);
			var dx : Number = 	0.5 * (	 m[3] * nw + m[7] * nz - m[11] * ny);
			var dy : Number = 	0.5 * (- m[3] * nz + m[7] * nw + m[11] * nx);
			var dz : Number = 	0.5 * (	 m[3] * ny - m[7] * nx + m[11] * nw);

			n.set(nx, ny, nz, nw);
			d.set(dx, dy, dz, dw);
		}

		public function toString() : String
		{
			return getRawData().toString();
		}
		
		public static function multiply(m1 	: Matrix4x4,
										m2 	: Matrix4x4,
										out	: Matrix4x4	= null) : Matrix4x4
		{
			out = copy(m1, out);
			out.prepend(m2);

			return out;
		}
		
		public static function copy(source	: Matrix4x4,
									target 	: Matrix4x4 = null) : Matrix4x4
		{
			target ||= FACTORY.create() as Matrix4x4;
			source._matrix.copyToMatrix3D(target._matrix);
			target._version++;

			return target;
		}
		
		public static function invert(input		: Matrix4x4,
							   		  output	: Matrix4x4	= null) : Matrix4x4
		{
			output = copy(input, output);
			output.invert();

			return output;
		}

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
		public static function lookAtLH(eye 	: Vector4,
										lookAt 	: Vector4,
										up		: Vector4,
										out		: Matrix4x4 = null) : Matrix4x4
		{
			var z_axis	: Vector4	= null;
			var	x_axis	: Vector4	= null;
			var	y_axis	: Vector4	= null;
			var	m41		: Number	= 0.;
			var	m42		: Number	= 0.;
			var	m43		: Number	= 0.;

			z_axis = Vector4.subtract(lookAt, eye).normalize();
			x_axis = Vector4.crossProduct(up, z_axis).normalize();
			y_axis = Vector4.crossProduct(z_axis, x_axis).normalize();

			if (Vector4.equals(x_axis, ConstVector4.ZERO) || Vector4.equals(y_axis, ConstVector4.ZERO))
			{
				throw new Error('Invalid argument(s): the eye direction (look at - eye position) '
								+ 'and the up vector appear to be the same.');
			}

			m41 = -Vector4.dotProduct(x_axis, eye);
			m42 = -Vector4.dotProduct(y_axis, eye);
			m43 = -Vector4.dotProduct(z_axis, eye);
			
			out ||= FACTORY.create() as Matrix4x4;
			out.initialize(x_axis.x,	y_axis.x,	z_axis.x,	0.,
						   x_axis.y,	y_axis.y,	z_axis.y,	0.,
						   x_axis.z,	y_axis.z,	z_axis.z,	0.,
						   m41,			m42,		m43,		1.);

			return out;
		}

		/**
		 * Builds a (right-handed) view transform.
		 * <br /><br />
		 * Eye : eye position, At : eye direction, Up : up vector
		 * <br /><br />
		 * zaxis = normal(Eye - At)<br />
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
		public static function lookAtRH(eye 	: Vector4,
										lookAt	: Vector4,
										up		: Vector4,
										out		: Matrix4x4 = null) : Matrix4x4
		{
			var z_axis	: Vector4	= null;
			var	x_axis	: Vector4	= null;
			var	y_axis	: Vector4	= null;
			var	m41		: Number	= 0.;
			var	m42		: Number	= 0.;
			var	m43		: Number	= 0.;

			z_axis = Vector4.subtract(eye, lookAt).normalize();
			x_axis = Vector4.crossProduct(up, z_axis).normalize();
			y_axis = Vector4.crossProduct(z_axis, x_axis).normalize();

			if (Vector4.equals(x_axis, ConstVector4.ZERO) || Vector4.equals(y_axis, ConstVector4.ZERO))
			{
				throw new Error('Invalid argument(s): the eye direction (look at - eye position) '
								+ 'and the up vector appear to be the same.');
			}

			m41 = -Vector4.dotProduct(x_axis, eye);
			m42 = -Vector4.dotProduct(y_axis, eye);
			m43 = -Vector4.dotProduct(z_axis, eye);
			
			out ||= FACTORY.create() as Matrix4x4;
			out.initialize(x_axis.x,	y_axis.x,	z_axis.x,	0.,
						   x_axis.y,	y_axis.y,	z_axis.y,	0.,
						   x_axis.z,	y_axis.z,	z_axis.z,	0.,
						   m41,			m42,		m43,		1.);

			return out;
		}

		public static function perspectiveFoVLH(fov		: Number,
												ratio	: Number,
												zNear	: Number,
												zFar 	: Number,
												out		: Matrix4x4 = null) : Matrix4x4
		{
			var	y_scale		: Number	= 1. / Math.tan(fov * .5);
			var	x_scale		: Number	= y_scale / ratio;
			var	m33			: Number	= zFar / (zFar - zNear);
			var	m43			: Number	= -zNear * zFar / (zFar - zNear);
			
			out ||= FACTORY.create() as Matrix4x4;
			out.initialize(x_scale,	0.,			0.,		0.,
						   0.,		y_scale,	0.,		0.,
						   0.,		0.,			m33,	1.,
						   0.,		0.,			m43,	0.);

			return out;
		}

		public static function orthoLH(w 		: Number,
									   h		: Number,
									   zNear	: Number,
									   zFar		: Number,
									   out		: Matrix4x4 = null) : Matrix4x4
		{
			out ||= FACTORY.create() as Matrix4x4;
			out.initialize(2. / w,	0.,		0.,						0.,
						   0.,		2. / h,	0.,						0.,
						   0.,		0.,		1. / (zFar - zNear),	0.,
						   0.,		0.,		zNear / (zNear - zFar),	1.);

			return out;
		}

		public static function orthoRH(w 		: Number,
									   h		: Number,
									   zNear	: Number,
									   zFar		: Number,
									   out		: Matrix4x4 = null) : Matrix4x4
		{
			out ||= FACTORY.create() as Matrix4x4;
			out.initialize(2. / w,	0.,		0.,						0.,
						   0.,		2. / h,	0.,						0.,
						   0.,		0.,		1. / (zNear - zFar),	0.,
						   0.,		0.,		zNear / (zNear - zFar),	1.);

			return out;
		}

		public static function orthoOffCenterLH(l		: Number,
												r		: Number,
												b		: Number,
												t		: Number,
												zNear	: Number,
												zFar	: Number,
												out		: Matrix4x4 = null) : Matrix4x4
		{
			out ||= FACTORY.create() as Matrix4x4;
			out.initialize(2. / (r - l),		0.,					0.,						0.,
						   0.,					2. / (t - b),		0.,						0.,
						   0.,					0.,					1. / (zFar - zNear),	0.,
						   (l + r) / (l - r),	(t + b) / (b - t),	zNear / (zNear - zFar),	1.);

			return out;
		}

		public static function orthoOffCenterRH(l		: Number,
												r		: Number,
												b		: Number,
												t		: Number,
												zNear	: Number,
												zFar	: Number,
												out		: Matrix4x4 = null) : Matrix4x4
		{
			out ||= FACTORY.create() as Matrix4x4;
			out.initialize(2. / (r - l),		0.,					0.,						0.,
						   0.,					2. / (t - b),		0.,						0.,
						   0.,					0.,					1. / (zNear - zFar),	0.,
						   (l + r) / (l - r),	(t + b) / (b - t),	zNear / (zNear - zFar),	1.);

			return out;
		}
		
		public static function fromQuaternion(quaternion : Vector4, out : Matrix4x4 = null) : Matrix4x4
		{
			out ||= FACTORY.create() as Matrix4x4;
			
			var x : Number = quaternion.x;
			var y : Number = quaternion.y;
			var z : Number = quaternion.z;
			var w : Number = quaternion.w;
			var xy2 : Number = 2.* x * y;
			var xz2 : Number = 2.* x * z;
			var xw2 : Number = 2.* x * w;
			var yz2 : Number = 2.* y * z;
			var yw2 : Number = 2.* y * w;
			var zw2 : Number = 2.* z * w;
			var xx : Number = x * x;
			var yy : Number = y * y;
			var zz : Number = z * z;
			var ww : Number = w * w;

			out.initialize(xx - yy - zz + ww, 	xy2 + zw2, 			xz2 - yw2, 			0.,
						   xy2 - zw2,			-xx + yy - zz + ww,	yz2 + xw2,			0.,
						   xz2 + yw2,			yz2 - xw2,			-xx - yy + zz + ww, 0.,
						   0.,					0.,					0.,					1.);

			return out;
		}
		
		public static function fromDualQuaternion(dQn : Vector4, dQd : Vector4, out : Matrix4x4 = null) : Matrix4x4
		{
			out ||= FACTORY.create() as Matrix4x4;
			
			var len2Inv	: Number = 1 / (dQn.w * dQn.w + dQn.x * dQn.x + dQn.y * dQn.y + dQn.z * dQn.z);

			var w		: Number = dQn.w;
			var x		: Number = dQn.x;
			var y		: Number = dQn.y;
			var z		: Number = dQn.z;

			var t0		: Number = dQd.w;
			var t1		: Number = dQd.x;
			var t2		: Number = dQd.y;
			var t3		: Number = dQd.z;

			out.initialize(
				len2Inv * (w * w + x * x - y * y - z * z),
				len2Inv * (2 * x * y + 2 * w * z),
				len2Inv * (2 * x * z - 2 * w * y),
				0,

				len2Inv * (2 * x * y - 2 * w * z),
				len2Inv * (w * w + y * y - x * x - z * z),
				len2Inv * (2 * y * z + 2 * w * x),
				0,

				len2Inv * (2 * x * z + 2 * w * y),
				len2Inv * (2 * y * z - 2 * w * x),
				len2Inv * (w * w + z * z - x * x - y * y),
				0,

				len2Inv * (-2 * t0 * x + 2 * w * t1 - 2 * t2 * z + 2 * y * t3),
				len2Inv * (-2 * t0 * y + 2 * t1 * z - 2 * x * t3 + 2 * w * t2),
				len2Inv * (-2 * t0 * z + 2 * x * t2 + 2 * w * t3 - 2 * t1 * y),
				1
			);

			return out;
		}

		public static function fromRawData(data			: Vector.<Number>,
										   offset		: int		= 0,
										   transposed	: Boolean	= false) : Matrix4x4
		{
			return (FACTORY.create() as Matrix4x4).setRawData(data, offset, transposed);
		}

	}
}