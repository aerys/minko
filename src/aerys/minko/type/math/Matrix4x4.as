package aerys.minko.type.math
{
	import aerys.common.Factory;
	import aerys.common.IVersionnable;
	import aerys.minko.ns.minko;
	
	import flash.geom.Matrix3D;
	import flash.geom.Utils3D;
	import flash.geom.Vector3D;

	public class Matrix4x4 implements IVersionnable
	{
		use namespace minko;
		
		private static const FACTORY			: Factory	= Factory.getFactory(Matrix4x4);
		private static const DEFAULT_DATA		: Array		= [1., 0., 0., 0.,
															   0., 1., 0., 0.,
															   0., 0., 1., 0.,
															   0., 0., 0., 1.];
		
		private static const UPDATE_NONE		: uint		= 0;
		private static const UPDATE_DATA		: uint		= 1;
		private static const UPDATE_MATRIX		: uint		= 2;
		private static const UPDATE_ALL			: uint		= UPDATE_DATA | UPDATE_MATRIX;
		
		minko var _matrix	: Matrix3D			= null;
		minko var _data		: Vector.<Number>	= new Vector.<Number>(16, true);
		
		private var _version	: uint			= 0;
		private var _update		: uint			= UPDATE_NONE;
		
		public function get version()	: uint		{ return _version; }
		
		public function get m11()		: Number	{ return rawData[0]; }
		public function get m12()		: Number	{ return rawData[1]; }
		public function get m13()		: Number	{ return rawData[2]; }
		public function get m14()		: Number	{ return rawData[3]; }
		public function get m21()		: Number	{ return rawData[4]; }
		public function get m22()		: Number	{ return rawData[5]; }
		public function get m23()		: Number	{ return rawData[6]; }
		public function get m24()		: Number	{ return rawData[7]; }
		public function get m31()		: Number	{ return rawData[8]; }
		public function get m32()		: Number	{ return rawData[9]; }
		public function get m33()		: Number	{ return rawData[10]; }
		public function get m34()		: Number	{ return rawData[11]; }
		public function get m41()		: Number	{ return rawData[12]; }
		public function get m42()		: Number	{ return rawData[13]; }
		public function get m43()		: Number	{ return rawData[14]; }
		public function get m44()		: Number	{ return rawData[15]; }
		
		public function set m11(value : Number) : void	{ setComponent(0, value); }
		public function set m12(value : Number) : void	{ setComponent(1, value); }
		public function set m13(value : Number) : void	{ setComponent(2, value); }
		public function set m14(value : Number) : void	{ setComponent(3, value); }
		public function set m21(value : Number) : void	{ setComponent(4, value); }
		public function set m22(value : Number) : void	{ setComponent(5, value); }
		public function set m23(value : Number) : void	{ setComponent(6, value); }
		public function set m24(value : Number) : void	{ setComponent(7, value); }
		public function set m31(value : Number) : void	{ setComponent(8, value); }
		public function set m32(value : Number) : void	{ setComponent(9, value); }
		public function set m33(value : Number) : void	{ setComponent(10, value); }
		public function set m34(value : Number) : void	{ setComponent(11, value); }
		public function set m41(value : Number) : void	{ setComponent(12, value); }
		public function set m42(value : Number) : void	{ setComponent(13, value); }
		public function set m43(value : Number) : void	{ setComponent(14, value); }
		public function set m44(value : Number) : void	{ setComponent(15, value); }
		
		protected function get invalidRawData() : Boolean	{ return (_update & UPDATE_DATA) != 0; }
		protected function get invalidMatrix() 	: Boolean	{ return (_update & UPDATE_MATRIX) != 0; }
		
		minko function get matrix3D() : Matrix3D
		{
			return matrix;
		}
		
		protected function get matrix() : Matrix3D
		{
			if (invalidMatrix)
			{
				updateMatrix();
				_update &= ~UPDATE_MATRIX;
			}
			
			return _matrix;
		}
		
		protected function get rawData() : Vector.<Number>
		{
			if (invalidRawData)
			{
				 updateRawData();
				 _update &= ~UPDATE_DATA;
			}
			
			return _data;
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
		
		protected function updateMatrix() : void
		{
			_matrix.rawData = _data;
		}
		
		protected function updateRawData() : void
		{
			_data = _matrix.rawData;
		}
		
		private function initialize(m11 : Number, m12 : Number, m13 : Number, m14 : Number,
									m21 : Number, m22 : Number, m23 : Number, m24 : Number,
									m31 : Number, m32 : Number, m33 : Number, m34 : Number,
									m41 : Number, m42 : Number, m43 : Number, m44 : Number) : void
		{
			_data[0] = m11;
			_data[1] = m12;
			_data[2] = m13;
			_data[3] = m14;
			_data[4] = m21;
			_data[5] = m22;
			_data[6] = m23;
			_data[7] = m24;
			_data[8] = m31;
			_data[9] = m32;
			_data[10] = m33;
			_data[11] = m34;
			_data[12] = m41;
			_data[13] = m42;
			_data[14] = m43;
			_data[15] = m44;
			
			_matrix = new Matrix3D(_data);
		}
		
		private function setComponent(index : int, value : Number) : void
		{
			if (_data[index] != value)
			{
				_data[index] = value;
				
				_update |= UPDATE_MATRIX;
				++_version;
			}
		}
		
		protected function invalidateRawData() : void
		{
			++_version;
			_update |= UPDATE_DATA;
		}
		
		protected function invalidateMatrix() : void
		{
			++_version;
			_update |= UPDATE_MATRIX;
		}
		
		public function multiply(m : Matrix4x4) : Matrix4x4
		{
			matrix.prepend(m.matrix);
			
			_update |= UPDATE_DATA;
			++_version;
			
			return this;
		}
		
		public function multiplyInverse(m : Matrix4x4) : Matrix4x4
		{
			matrix.append(m.matrix);
			
			_update |= UPDATE_DATA;
			++_version;
			
			return this;
		}
		
		public function multiplyVector(input 	: Vector4,
									   output	: Vector4	= null) : Vector4
		{
			var v : Vector3D = matrix.transformVector(input._vector);
			
			output ||= new Vector4();			
			output.set(v.x, v.y, v.z, v.w);
			
			return output;
		}
		
		public function deltaMultiplyVector(input 	: Vector4,
											output	: Vector4	= null) : Vector4
		{
			var v : Vector3D = matrix.deltaTransformVector(input._vector);
			
			output ||= new Vector4();			
			output.set(v.x, v.y, v.z, v.w);
			
			return output;
		}
		
		public function multiplyRawVectors(input 	: Vector.<Number>,
										   output	: Vector.<Number> = null) : Vector.<Number>
		{
			output ||= new Vector.<Number>();
			matrix.transformVectors(input, output);
			
			return output;
		}
		
		public function identity() : Matrix4x4
		{
			_matrix.identity();
			_data[0] = 1.;
			_data[1] = 0.;
			_data[2] = 0.;
			_data[3] = 0.;
			_data[4] = 0.;
			_data[5] = 1.;
			_data[6] = 0.;
			_data[7] = 0.;
			_data[8] = 0.;
			_data[9] = 0.;
			_data[10] = 1.;
			_data[11] = 0.;
			_data[12] = 0.;
			_data[13] = 0.;
			_data[14] = 0.;
			_data[15] = 1.;
			
			++_version;
			
			return this;
		}
		
		public function zero() : Matrix4x4
		{
			_data[0] = 0.;
			_data[1] = 0.;
			_data[2] = 0.;
			_data[3] = 0.;
			_data[4] = 0.;
			_data[5] = 0.;
			_data[6] = 0.;
			_data[7] = 0.;
			_data[8] = 0.;
			_data[9] = 0.;
			_data[10] = 0.;
			_data[11] = 0.;
			_data[12] = 0.;
			_data[13] = 0.;
			_data[14] = 0.;
			_data[15] = 0.;
			
			_update |= UPDATE_MATRIX;
			++_version;
			
			return this;
		}
		
		public function invert() : Boolean
		{
			++_version;
			_update |= UPDATE_DATA;
			
			return matrix.invert();
		}
		
		public function transpose() : void
		{
			matrix.transpose();
			
			++_version;
			_update |= UPDATE_DATA;
		}
		
		public function projectVector(input 	: Vector4,
									  output	: Vector4 = null) : Vector4
		{
			var v : Vector3D = Utils3D.projectVector(matrix, input._vector);
			
			output ||= new Vector4();
			output.set(v.x, v.y, v.z, v.w);
			
			return output;
		}
		
		public function setRawData(data : Vector.<Number>) : void
		{
			for (var i : int = 0; i < 16; ++i)
				_data[i] = data[i];
			
			++_version;
			_update = UPDATE_MATRIX;
		}
		
		public function getRawData(out : Vector.<Number> = null) : Vector.<Number>
		{
			var i 		: int 				= 0;
			var rawData : Vector.<Number> 	= rawData;
			
			out ||= new Vector.<Number>(16, true);
			for (i = 0; i < 16; ++i)
				out[i] = rawData[i];
			
			return out;
		}
		
		public function projectVectors(input 	: Vector.<Number>,
									   output	: Vector.<Number>,
									   uvt		: Vector.<Number>) : void
		{
			Utils3D.projectVectors(matrix, input, output, uvt);
		}
		
		public function interpolateTo(target : Matrix4x4, percent : Number) : void
		{
			matrix.interpolateTo(target.matrix, percent);
			
			++_version;
			_update |= UPDATE_DATA;
		}
		
		public function toString() : String
		{
			return getRawData().toString();
		}
		
		public static function multiply(m1 	: Matrix4x4,
										m2 	: Matrix4x4,
										out	: Matrix4x4	= null) : Matrix4x4
		{
			out ||= FACTORY.create();
			out.identity();
			out.matrix.append(m1.matrix);
			out.multiply(m2);
			
			return out;
		}
		
		public static function copy(source	: Matrix4x4,
									target 	: Matrix4x4 = null) : Matrix4x4
		{
			target ||= FACTORY.create();
			source.matrix.copyToMatrix3D(target._matrix);
			target._update = UPDATE_DATA;
			
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
			
			m41 = -Vector4.dotProduct(x_axis, eye);
			m42 = -Vector4.dotProduct(y_axis, eye);
			m43 = -Vector4.dotProduct(z_axis, eye);
			
			out ||= FACTORY.create();
			out.setRawData(Vector.<Number>([x_axis.x,	y_axis.x,	z_axis.x,	0.,
											x_axis.y,	y_axis.y,	z_axis.y,	0.,
											x_axis.z,	y_axis.z,	z_axis.z,	0.,
											m41,		m42,		m43,		1.]));
			
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
			
			m41 = -Vector4.dotProduct(x_axis, eye);
			m42 = -Vector4.dotProduct(y_axis, eye);
			m43 = -Vector4.dotProduct(z_axis, eye);
			
			out ||= FACTORY.create();
			out.setRawData(Vector.<Number>([x_axis.x,	y_axis.x,	z_axis.x,	0.,
											x_axis.y,	y_axis.y,	z_axis.y,	0.,
											x_axis.z,	y_axis.z,	z_axis.z,	0.,
											m41,		m42,		m43,		1.]))
			
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
			
			out ||= FACTORY.create();
			out.setRawData(Vector.<Number>([x_scale,	0.,			0.,		0.,
											0.,			y_scale,	0.,		0.,
											0.,			0.,			m33,	1.,
											0.,			0.,			m43,	0.]));
			
			return out;
		}
		
		public static function orthoLH(w 		: Number,
									   h		: Number,
									   zNear	: Number,
									   zFar		: Number,
									   out		: Matrix4x4 = null) : Matrix4x4
		{
			out ||= FACTORY.create();
			out.setRawData(Vector.<Number>([2. / w,	0.,		0.,						0.,
											0.,		2. / h,	0.,						0.,
											0.,		0.,		1. / (zFar - zNear),	0.,
											0.,		0.,		zNear / (zNear - zFar),	1.]));
			
			return out;
		}
		
		public static function orthoRH(w 		: Number,
									   h		: Number,
									   zNear	: Number,
									   zFar		: Number,
									   out		: Matrix4x4 = null) : Matrix4x4
		{
			out ||= FACTORY.create();
			out.setRawData(Vector.<Number>([2. / w,	0.,		0.,						0.,
											0.,		2. / h,	0.,						0.,
											0.,		0.,		1. / (zNear - zFar),	0.,
											0.,		0.,		zNear / (zNear - zFar),	1.]));
			
			return out;
		}
		
		public static function orthoOffCenterLH(l	: Number,
												r	: Number,
												b	: Number,
												t		: Number,
												zNear	: Number,
												zFar	: Number,
												out		: Matrix4x4 = null) : Matrix4x4
		{
			out ||= FACTORY.create();
			out.setRawData(Vector.<Number>([2. / (r - l),		0.,					0.,						0.,
											0.,					2. / (t - b),		0.,						0.,
											0.,					0.,					1. / (zFar - zNear),	0.,
											(l + r) / (l - r),	(t + b) / (b - t),	zNear / (zNear - zFar),	1.]));
			
			return out;
		}
		
		public static function orthoOffCenterRH(l	: Number,
												r	: Number,
												b	: Number,
												t		: Number,
												zNear	: Number,
												zFar	: Number,
												out		: Matrix4x4 = null) : Matrix4x4
		{
			out ||= FACTORY.create();
			out.setRawData(Vector.<Number>([2. / (r - l),		0.,					0.,						0.,
											0.,					2. / (t - b),		0.,						0.,
											0.,					0.,					1. / (zNear - zFar),	0.,
											(l + r) / (l - r),	(t + b) / (b - t),	zNear / (zNear - zFar),	1.]));
			
			return out;
		}
		
	}
}