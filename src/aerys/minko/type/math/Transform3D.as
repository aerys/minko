package aerys.minko.type.math
{
	import aerys.common.Factory;
	import aerys.common.IVersionnable;
	import aerys.minko.ns.minko;
	
	import flash.geom.Matrix3D;
	import flash.geom.Orientation3D;
	import flash.geom.Utils3D;
	import flash.geom.Vector3D;

	public class Transform3D implements IVersionnable
	{
		use namespace minko;
		
		private static const TRANSFORM		: Factory	= Factory.getFactory(Transform3D);
		private static const RAD2DEG		: Number	= 180. / Math.PI;
		private static const ORIENTATION	: String	= Orientation3D.EULER_ANGLES;
		
		minko var _matrix				: Matrix3D			= new Matrix3D();
		
		private var _version			: uint				= 0;
		private var _updateRawData		: Boolean			= false;
		private var _updateComponents	: Boolean			= false;
		
		private var _position			: Vector3D			= new Vector3D();
		private var _rotation			: Vector3D			= new Vector3D();
		private var _scale				: Vector3D			= new Vector3D(1., 1., 1.);
		private var _components			: Vector.<Vector3D>	= Vector.<Vector3D>([_position,
																				 _rotation,
																				 _scale]);
		
		public function get x() : Number
		{
			_updateComponents && updateComponents();
			
			return _position.x;
		}
		public function get y() : Number
		{
			_updateComponents && updateComponents();
			
			return _position.y;
		}
		public function get z() : Number
		{
			_updateComponents && updateComponents();
			
			return _position.z;
		}
		public function get scaleX() : Number
		{
			_updateComponents && updateComponents();
			
			return _scale.x;
		}
		public function get scaleY() : Number
		{
			_updateComponents && updateComponents();
			
			return _scale.y;
		}
		public function get scaleZ() : Number
		{
			_updateComponents && updateComponents();
			
			return _scale.z;
		}
		public function get rotationX() : Number
		{
			_updateComponents && updateComponents();
			
			return _rotation.x;
		}
		public function get rotationY() : Number
		{
			_updateComponents && updateComponents();
			
			return _rotation.y;
		}
		public function get rotationZ() : Number
		{
			_updateComponents && updateComponents();
			
			return _rotation.z;
		}
		
		public function set x(value : Number) : void
		{
			if (value != _position.x)
			{
				_position.x = value;
				++_version;
				_updateRawData = true;
			}
		}
		
		public function set y(value : Number) : void
		{
			if (value != _position.y)
			{
				_position.y = value;
				++_version;
				_updateRawData = true;
			}
		}
		
		public function set z(value : Number) : void
		{
			if (value != _position.x)
			{
				_position.z = value;
				++_version;
				_updateRawData = true;
			}
		}
		
		public function set scaleX(value : Number) : void
		{
			if (value != _scale.x)
			{
				++_version;
				_updateRawData = true;
				_scale.x = value;
			}
		}
		
		public function set scaleY(value : Number) : void
		{
			if (value != _scale.y)
			{
				++_version;
				_updateRawData = true;
				_scale.y = value;
			}
		}
		
		public function set scaleZ(value : Number) : void
		{
			if (value != _scale.z)
			{
				++_version;
				_updateRawData = true;
				_scale.z = value;
			}
		}
		
		public function set rotationX(value : Number) : void
		{
			if (value != _rotation.x)
			{
				++_version;
				_updateRawData = true;
				_rotation.x = value;
			}
		}
		
		public function set rotationY(value : Number) : void
		{
			if (value != _rotation.y)
			{
				++_version;
				_updateRawData = true;
				_rotation.y = value;
			}
		}
		
		public function set rotationZ(value : Number) : void
		{
			if (value != _rotation.z)
			{
				++_version;
				_updateRawData = true;
				_rotation.z = value;
			}
		}
		
		public function get version() : uint
		{
			return _version;
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
		public static function lookAtLeftHanded(position 	: Vector3D,
												lookAt	 	: Vector3D,
												up			: Vector3D) : Transform3D
		{
			var z_axis		: Vector3D		= null;
			var	x_axis		: Vector3D		= null;
			var	y_axis		: Vector3D		= null;
			var	m41			: Number		= 0.;
			var	m42			: Number		= 0.;
			var	m43			: Number		= 0.;
			var matrix		: Transform3D	= TRANSFORM.create();
			
			z_axis = lookAt.subtract(position);
			z_axis.normalize();
			x_axis = up.crossProduct(z_axis);
			x_axis.normalize();
			y_axis = z_axis.crossProduct(x_axis);
			y_axis.normalize();
			
			m41 = -x_axis.dotProduct(position);
			m42 = -y_axis.dotProduct(position);
			m43 = -z_axis.dotProduct(position);
			
			matrix.setRawData(Vector.<Number>([x_axis.x,	y_axis.x,	z_axis.x,	0.,
											   x_axis.y,	y_axis.y,	z_axis.y,	0.,
											   x_axis.z,	y_axis.z,	z_axis.z,	0.,
											   m41,			m42,		m43,		1.]));
			
			return matrix;
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
		public static function lookAtRightHanded(position 	: Vector3D,
												 lookAt		: Vector3D,
												 up			: Vector3D) : Transform3D
		{
			var z_axis		: Vector3D		= null;
			var	x_axis		: Vector3D		= null;
			var	y_axis		: Vector3D		= null;
			var	m41			: Number		= 0.;
			var	m42			: Number		= 0.;
			var	m43			: Number		= 0.;
			var matrix		: Transform3D	= TRANSFORM.create();
			
			z_axis = position.subtract(lookAt);
			z_axis.normalize();
			x_axis = up.crossProduct(z_axis);
			x_axis.normalize();
			y_axis = z_axis.crossProduct(x_axis);
			y_axis.normalize();
			
			m41 = -x_axis.dotProduct(position);
			m42 = -y_axis.dotProduct(position),
			m43 = -z_axis.dotProduct(position);

			matrix.setRawData(Vector.<Number>([x_axis.x,	y_axis.x,	z_axis.x,	0.,
											   x_axis.y,	y_axis.y,	z_axis.y,	0.,
											   x_axis.z,	y_axis.z,	z_axis.z,	0.,
											   m41,			m42,		m43,		1.]))
			
			return matrix;
		}
		
		public static  function perspectiveFovLH(fov	: Number,
												 ratio	: Number,
												 zNear	: Number,
												 zFar 	: Number) : Transform3D
		{
			var	y_scale		: Number		= 1. / Math.tan(fov * .5);
			var	x_scale		: Number		= y_scale / ratio;
			var	m33			: Number		= zFar / (zFar - zNear);
			var	m43			: Number		= -zNear * zFar / (zFar - zNear);
			var t			: Transform3D	= TRANSFORM.create();
			
			t.setRawData(Vector.<Number>([x_scale,	0.,			0.,		0.,
										  0.,		y_scale,	0.,		0.,
										  0.,		0.,			m33,	1.,
										  0.,		0.,			m43,	0.]));
			
			return t;
		}
		
		public function Transform3D(rawData : Vector.<Number> = null)
		{
			if (rawData)
			{
				_updateComponents = true;
				_matrix.rawData = rawData;
			}
		}
		
		private function updateRawData() : void
		{
			_updateRawData = false;
			_matrix.recompose(_components, ORIENTATION);
		}
		
		private function updateComponents() : void
		{
			_updateComponents = false;
			_components = _matrix.decompose(ORIENTATION);
			_position = _components[0];
			_rotation = _components[1];
			_scale = _components[2];
		}
		
		public function append(lhs : Transform3D) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			lhs._updateRawData && lhs.updateRawData();
			_matrix.append(lhs._matrix);
			
			return this;
		}
		
		public function appendRotation(radians		: Number,
									   axis			: Vector3D,
									   pivotPoint	: Vector3D	= null) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			_matrix.appendRotation(radians * RAD2DEG, axis, pivotPoint);
			
			return this;
		}
		
		public function appendScale(xScale	: Number,
									yScale	: Number	= 1.,
									zScale	: Number	= 1.) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			_matrix.appendScale(xScale, yScale, zScale);
			
			return this;
		}
		
		public function appendUniformScale(scale : Number) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			_matrix.appendScale(scale, scale, scale);
			
			return this;
		}
		
		public function appendTranslation(x : Number,
										  y : Number = 0.,
										  z : Number = 0.) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			_matrix.appendTranslation(x, y, z);
			
			return this;
		}
		
		public function identity() : Transform3D
		{
			++_version;
			_updateComponents = true;
			_matrix.identity();
			
			return this;
		}
		
		public function interpolateTo(toMat 	: Transform3D,
									  percent 	: Number) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			_matrix.interpolateTo(toMat._matrix, percent);
			
			return this;
		}
		
		public function invert() : Boolean
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			
			return _matrix.invert();
		}
		
		public function pointAt(pos	: Vector3D,
								at	: Vector3D	= null,
								up	: Vector3D	= null) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			_matrix.pointAt(pos, at, up);
			
			return this;
		}
		
		public function prepend(rhs : Transform3D) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			rhs._updateRawData && rhs.updateRawData();
			_matrix.prepend(rhs._matrix);
			
			return this;
		}
		
		public function prependRotation(radians		: Number,
										axis		: Vector3D,
										pivotPoint	: Vector3D = null) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			_matrix.prependRotation(radians * RAD2DEG, axis, pivotPoint);
			
			return this;
		}
		
		public function prependScale(xScale	: Number,
									 yScale	: Number = 1.,
									 zScale	: Number = 1.) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			_matrix.prependScale(xScale, yScale, zScale);
			
			return this;
		}
		
		public function prependUniformScale(scale : Number) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			_matrix.prependScale(scale, scale, scale);
			
			return this;
		}
		
		public function prependTranslation(x : Number,
										   y : Number	= 1.,
										   z : Number	= 1.) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			_matrix.prependTranslation(x, y, z);
			
			return this;
		}
		
		public function recompose(components		: Vector.<Vector3D>,
								  orientationStyle	: String = "eulerAngles") : Boolean
		{
			var p : Vector3D = components[0];
			var r : Vector3D = components[1];
			var s : Vector3D = components[2];
			
			_position.x = p.x;
			_position.y = p.y;
			_position.z = p.z;
			_rotation.x = r.x;
			_rotation.y = r.y;
			_rotation.z = r.z;
			_scale.x = s.x;
			_scale.y = s.y;
			_scale.z = s.z;

			++_version;
			_updateComponents = false;
			
			return _matrix.recompose(components, orientationStyle);
		}
		
		public function transpose() : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData && updateRawData();
			_matrix.transpose();
			
			return this;
		}
		
		public function setRotation(x : Number = 0.,
									y : Number = 0.,
									z : Number = 0.) : Transform3D
		{
			_updateComponents && updateComponents();
			
			_rotation.x = x;
			_rotation.y = y;
			_rotation.z = z;
			
			++_version;
			_updateRawData = true;
			
			return this;
		}
		
		public function setScale(x : Number = 1.,
								 y : Number = 1.,
								 z : Number = 1.) : Transform3D
		{
			_updateComponents && updateComponents();
			
			_scale.x = x;
			_scale.y = y;
			_scale.z = z;
			
			++_version;
			_updateRawData = true;
			
			return this;
		}
		
		public function setPosition(x : Number 	= 0.,
									y : Number	= 0.,
									z : Number 	= 0.) : Transform3D
		{
			_updateComponents && updateComponents();
			
			_position.x = x;
			_position.y = y;
			_position.z = z;
			
			++_version;
			_updateRawData = true;
			
			return this;
		}
		
		public function clone() : Transform3D
		{
			var clone : Transform3D = TRANSFORM.create();
			
			clone.setRawData(_matrix.rawData);
			
			return clone;
		}
		
		minko function temporaryClone() : Transform3D
		{
			var clone : Transform3D = TRANSFORM.create(true);
			
			clone.setRawData(_matrix.rawData);
			
			return clone;
		}
		
		public function transformVector(vector : Vector3D) : Vector3D
		{
			return _matrix.transformVector(vector);
		}
		
		public function deltaTransformVector(vector : Vector3D) : Vector3D
		{
			return _matrix.deltaTransformVector(vector);
		}
		
		public function setRawData(data : Vector.<Number>) : Transform3D
		{
			++_version;
			_updateComponents = true;
			_updateRawData = false;
			_matrix.rawData = data;
			
			return this;
		}
		
		public function getRawData() : Vector.<Number>
		{
			_updateRawData && updateRawData();
			
			return _matrix.rawData;
		}
		
		public function decompose(orientation : String = "eulerAngles") : Vector.<Vector3D>
		{
			++_version;
			_updateRawData && updateRawData();
			
			var c : Vector.<Vector3D> = _matrix.decompose(orientation);
			var p : Vector3D = c[0];
			var r : Vector3D = c[1];
			var s : Vector3D = c[2];
			
			_position.x = p.x;
			_position.y = p.y;
			_position.z = p.z;
			_rotation.x = r.x;
			_rotation.y = r.y;
			_rotation.z = r.z;
			_scale.x = s.x;
			_scale.y = s.y;
			_scale.z = s.z;
			
			_updateComponents = false;
			
			return c;
		}
		
		public function transformVectors(input 	: Vector.<Number>,
										 output	: Vector.<Number>) : void
		{
			_updateRawData && updateRawData();
			_matrix.transformVectors(input, output);
		}
		
		public function getPosition() : Vector3D
		{
			_updateComponents && updateComponents()();
			
			return _position.clone();
		}
		
		public function getRotation() : Vector3D
		{
			_updateComponents && updateComponents();
			
			return _rotation.clone();
		}
		
		public function getScale() : Vector3D
		{
			_updateComponents && updateComponents();
			
			return _scale.clone();
		}
				
		public function projectVector(vector : Vector3D) : Vector3D
		{
			_updateRawData && updateRawData();
			
			return Utils3D.projectVector(_matrix, vector);
		}
		
		public function projectVectors(input 	: Vector.<Number>,
									   output	: Vector.<Number>,
									   uvt		: Vector.<Number>) : void
		{
			_updateRawData && updateRawData();
			Utils3D.projectVectors(_matrix, input, output, uvt);
		}
	}
}