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
		minko var _data		: Vector.<Number>	= null;
		
		private var _version	: uint			= 0;
		private var _update		: uint			= UPDATE_NONE;
		
		public function get version()	: uint		{ return _version; }
		
		public function get m11()		: Number	{ return data[0]; }
		public function get m12()		: Number	{ return data[1]; }
		public function get m13()		: Number	{ return data[2]; }
		public function get m14()		: Number	{ return data[3]; }
		public function get m21()		: Number	{ return data[4]; }
		public function get m22()		: Number	{ return data[5]; }
		public function get m23()		: Number	{ return data[6]; }
		public function get m24()		: Number	{ return data[7]; }
		public function get m31()		: Number	{ return data[8]; }
		public function get m32()		: Number	{ return data[9]; }
		public function get m33()		: Number	{ return data[10]; }
		public function get m34()		: Number	{ return data[11]; }
		public function get m41()		: Number	{ return data[12]; }
		public function get m42()		: Number	{ return data[13]; }
		public function get m43()		: Number	{ return data[14]; }
		public function get m44()		: Number	{ return data[15]; }
		
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
		
		protected function get matrix() : Matrix3D
		{
			if (_update & UPDATE_MATRIX)
			{
				_update &= ~UPDATE_MATRIX;
				_matrix.rawData = _data;
			}
			
			return _matrix;
		}
		
		protected function get data() : Vector.<Number>
		{
			if (_update & UPDATE_DATA)
			{
				_update &= ~UPDATE_DATA;
				_data = _matrix.rawData;
			}
			
			return _data;
		}
				
		public function Matrix4x4(data : Vector.<Number> = null)
		{
			initialize(data);
		}
		
		private function initialize(data : Vector.<Number>) : void
		{
			_data = data ? new Vector.<Number>(16, true)
						 : Vector.<Number>(DEFAULT_DATA);
			
			if (data)
				for (var i : int = 0; i < 16; ++i)
					_data[i] = data[i];
			
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
			_update |= UPDATE_MATRIX;
		}
		
		public function getRawData(data : Vector.<Number> = null) : Vector.<Number>
		{
			if (_update & UPDATE_DATA)
			{
				_update ^= UPDATE_DATA;
				_data = matrix.rawData;
			}
			
			data ||= new Vector.<Number>(16, true);
			for (var i : int = 0; i < 16; ++i)
				data[i] = _data[i];
			
			return data;
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
		
		public static function multiply(m1 		: Matrix4x4,
										m2 		: Matrix4x4,
										output	: Matrix4x4	= null) : Matrix4x4
		{
			output ||= FACTORY.create();
			
			output.identity();
			output.matrix.append(m1.matrix);
			output.multiply(m2);
			
			return output;
		}
		
		public static function multiplyInverse(m1 		: Matrix4x4,
											   m2 		: Matrix4x4,
											   output	: Matrix4x4 = null) : Matrix4x4
		{
			output ||= FACTORY.create();
			
			output.identity();
			output.matrix.append(m1.matrix);
			output.multiplyInverse(m2);
			
			return output;
		}
		
		public static function clone(source	: Matrix4x4,
									 target : Matrix4x4 = null) : Matrix4x4
		{
			target ||= FACTORY.create();
			target._matrix.identity();
			target._matrix.append(source.matrix);
			
			var targetData : Vector.<Number> = target._data;
			var sourceData : Vector.<Number> = source.data;
			
			for (var i : int = 0; i < 16; ++i)
				targetData[i] = sourceData[i];
			
			target._update = UPDATE_NONE;
			
			return target;
		}
		
		public static function invert(input		: Matrix4x4,
							   		  output	: Matrix4x4	= null) : Matrix4x4
		{
			output ||= FACTORY.create();
			
			output.identity();
			output.invert();
			
			return output;
		}
		
		/**
		 * Builds a (left-handed) view trnasform.
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
		public static function lookAtLeftHanded(eye 	: Vector4,
												lookAt 	: Vector4,
												up		: Vector4,
												result	: Matrix4x4 = null) : Matrix4x4
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
			
			result ||= FACTORY.create();
			result.setRawData(Vector.<Number>([x_axis.x,	y_axis.x,	z_axis.x,	0.,
											   x_axis.y,	y_axis.y,	z_axis.y,	0.,
											   x_axis.z,	y_axis.z,	z_axis.z,	0.,
											   m41,			m42,		m43,		1.]));
			
			return result;
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
		public static function lookAtRightHanded(eye 	: Vector4,
												 lookAt	: Vector4,
												 up		: Vector4,
												 result	: Matrix4x4 = null) : Matrix4x4
		{
			var z_axis		: Vector4	= null;
			var	x_axis		: Vector4	= null;
			var	y_axis		: Vector4	= null;
			var	m41			: Number	= 0.;
			var	m42			: Number	= 0.;
			var	m43			: Number	= 0.;
			
			z_axis = Vector4.subtract(eye, lookAt).normalize();
			x_axis = Vector4.crossProduct(up, z_axis).normalize();
			y_axis = Vector4.crossProduct(z_axis, x_axis).normalize();
			
			m41 = -Vector4.dotProduct(x_axis, eye);
			m42 = -Vector4.dotProduct(y_axis, eye);
			m43 = -Vector4.dotProduct(z_axis, eye);
			
			result ||= FACTORY.create();
			result.setRawData(Vector.<Number>([x_axis.x,	y_axis.x,	z_axis.x,	0.,
											   x_axis.y,	y_axis.y,	z_axis.y,	0.,
											   x_axis.z,	y_axis.z,	z_axis.z,	0.,
											   m41,			m42,		m43,		1.]))
			
			return result;
		}
		
		public static  function perspectiveFoVLH(fov	: Number,
												 ratio	: Number,
												 zNear	: Number,
												 zFar 	: Number,
												 result	: Matrix4x4 = null) : Matrix4x4
		{
			var	y_scale		: Number	= 1. / Math.tan(fov / 2.0);
			var	x_scale		: Number	= y_scale / ratio;
			var	m33			: Number	= zFar / (zFar - zNear);
			var	m43			: Number	= -zNear * zFar / (zFar - zNear);
			
			result ||= FACTORY.create();
			result.setRawData(Vector.<Number>([x_scale,	0.,			0.,		0.,
											   0.,		y_scale,	0.,		0.,
											   0.,		0.,			m33,	1.,
											   0.,		0.,			m43,	0.]));
			
			return result;
		}
	}
}