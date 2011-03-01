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
		
		private static const TMP				: Vector.<Number>	= new Vector.<Number>();
		
		private static const UPDATE_NONE		: uint		= 0;
		private static const UPDATE_DATA		: uint		= 1;
		private static const UPDATE_MATRIX		: uint		= 2;
		private static const UPDATE_ALL			: uint		= UPDATE_DATA | UPDATE_MATRIX;
		
		minko var _matrix		: Matrix3D			= new Matrix3D();
		
		private var _version	: uint				= 0;
		private var _data		: Vector.<Number>	= new Vector.<Number>();
		
		public function get version()	: uint		{ return _version; }
		
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
			TMP.length = 0;
			TMP.push(m11, m12, m13, m14,
					 m21, m22, m23, m24,
					 m31, m32, m33, m34,
					 m41, m42, m43, m44);
			
			_matrix.copyRawDataFrom(TMP);
		}
		
		public function push() : Matrix4x4
		{
			_matrix.copyRawDataTo(_data);
			
			return this;
		}
		
		public function pop() : Matrix4x4
		{
			var dataLength : int = _data.length;
			
			_matrix.copyRawDataFrom(_data, dataLength - 16);
			_data.length = dataLength - 16;
			
			return this;
		}
		
		public function multiply(m : Matrix4x4) : Matrix4x4
		{
			_matrix.prepend(m._matrix);
			
			++_version;
			
			return this;
		}
		
		public function multiplyInverse(m : Matrix4x4) : Matrix4x4
		{
			_matrix.append(m._matrix);
			
			++_version;
			
			return this;
		}
		
		public function multiplyVector(input 	: Vector4,
									   output	: Vector4	= null) : Vector4
		{
			var v : Vector3D = _matrix.transformVector(input._vector);
			
			output ||= new Vector4();			
			output.set(v.x, v.y, v.z, v.w);
			
			return output;
		}
		
		public function deltaMultiplyVector(input 	: Vector4,
											output	: Vector4	= null) : Vector4
		{
			var v : Vector3D = _matrix.deltaTransformVector(input._vector);
			
			output ||= new Vector4();			
			output.set(v.x, v.y, v.z, v.w);
			
			return output;
		}
		
		public function multiplyRawVectors(input 	: Vector.<Number>,
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
		
		public function setRawData(data 	: Vector.<Number>,
								   offset	: int	= 0) : void
		{
			_matrix.copyRawDataFrom(data);
			++_version;
		}
		
		public function getRawData(out 		: Vector.<Number> = null,
								   offset	: int			  = 0) : Vector.<Number>
		{
			out ||= new Vector.<Number>();
			_matrix.copyRawDataTo(out, offset);
			
			return out;
		}
		
		public function projectVectors(input 	: Vector.<Number>,
									   output	: Vector.<Number>,
									   uvt		: Vector.<Number>) : void
		{
			Utils3D.projectVectors(_matrix, input, output, uvt);
		}
		
		public function interpolateTo(target : Matrix4x4, percent : Number) : void
		{
			_matrix.interpolateTo(target._matrix, percent);
			
			++_version;
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
			out.multiply(m2);
			
			return out;
		}
		
		public static function copy(source	: Matrix4x4,
									target 	: Matrix4x4 = null) : Matrix4x4
		{
			target ||= FACTORY.create();
			source._matrix.copyToMatrix3D(target._matrix);
						
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
			
			m41 = -Vector4.dotProduct(x_axis, eye);
			m42 = -Vector4.dotProduct(y_axis, eye);
			m43 = -Vector4.dotProduct(z_axis, eye);
			
			out ||= FACTORY.create();
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
			
			out ||= FACTORY.create();
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
			out ||= FACTORY.create();
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
			out ||= FACTORY.create();
			out.initialize(2. / w,	0.,		0.,						0.,
						   0.,		2. / h,	0.,						0.,
						   0.,		0.,		1. / (zNear - zFar),	0.,
						   0.,		0.,		zNear / (zNear - zFar),	1.);
			
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
			out.initialize(2. / (r - l),		0.,					0.,						0.,
						   0.,					2. / (t - b),		0.,						0.,
						   0.,					0.,					1. / (zFar - zNear),	0.,
						   (l + r) / (l - r),	(t + b) / (b - t),	zNear / (zNear - zFar),	1.);
			
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
			out.initialize(2. / (r - l),		0.,					0.,						0.,
						   0.,					2. / (t - b),		0.,						0.,
						   0.,					0.,					1. / (zNear - zFar),	0.,
						   (l + r) / (l - r),	(t + b) / (b - t),	zNear / (zNear - zFar),	1.);
			
			return out;
		}
		
	}
}