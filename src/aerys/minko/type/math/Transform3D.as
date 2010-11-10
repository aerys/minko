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
		
		minko var _matrix		: Matrix3D			= new Matrix3D();
		
		private var _version	: uint				= 0;
		private var _update		: Boolean			= false;
		private var _invalidate	: Boolean			= false;
		
		private var _position	: Vector3D			= new Vector3D();
		private var _rotation	: Vector3D			= new Vector3D();
		private var _scale		: Vector3D			= new Vector3D(1., 1., 1.);
		private var _components	: Vector.<Vector3D>	= Vector.<Vector3D>([_position,
																		 _rotation,
																		 _scale]);
		
		public function get x() : Number
		{
			_invalidate && invalidateComponents();
			
			return _position.x;
		}
		public function get y() : Number
		{
			_invalidate && invalidateComponents();
			
			return _position.y;
		}
		public function get z() : Number
		{
			_invalidate && invalidateComponents();
			
			return _position.z;
		}
		public function get scaleX() : Number
		{
			_invalidate && invalidateComponents();
			
			return _scale.x;
		}
		public function get scaleY() : Number
		{
			_invalidate && invalidateComponents();
			
			return _scale.y;
		}
		public function get scaleZ() : Number
		{
			_invalidate && invalidateComponents();
			
			return _scale.z;
		}
		public function get rotationX() : Number
		{
			_invalidate && invalidateComponents();
			
			return _rotation.x;
		}
		public function get rotationY() : Number
		{
			_invalidate && invalidateComponents();
			
			return _rotation.y;
		}
		public function get rotationZ() : Number
		{
			_invalidate && invalidateComponents();
			
			return _rotation.z;
		}
		
		public function set x(value : Number) : void
		{
			if (value != _position.x)
			{
				_position.x = value;
				++_version;
				_update = true;
			}
		}
		
		public function set y(value : Number) : void
		{
			if (value != _position.y)
			{
				_position.y = value;
				++_version;
				_update = true;
			}
		}
		
		public function set z(value : Number) : void
		{
			if (value != _position.x)
			{
				_position.z = value;
				++_version;
				_update = true;
			}
		}
		
		public function set scaleX(value : Number) : void
		{
			if (value != _scale.x)
			{
				++_version;
				_update = true;
				_scale.x = value;
			}
		}
		
		public function set scaleY(value : Number) : void
		{
			if (value != _scale.y)
			{
				++_version;
				_update = true;
				_scale.y = value;
			}
		}
		
		public function set scaleZ(value : Number) : void
		{
			if (value != _scale.z)
			{
				++_version;
				_update = true;
				_scale.z = value;
			}
		}
		
		public function set rotationX(value : Number) : void
		{
			if (value != _rotation.x)
			{
				++_version;
				_update = true;
				_rotation.x = value;
			}
		}
		
		public function set rotationY(value : Number) : void
		{
			if (value != _rotation.y)
			{
				++_version;
				_update = true;
				_rotation.y = value;
			}
		}
		
		public function set rotationZ(value : Number) : void
		{
			if (value != _rotation.z)
			{
				++_version;
				_update = true;
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
		public static function lookAtLeftHanded(myEyePosition 	: Vector3D,
												myEyeDirection 	: Vector3D,
												myUp			: Vector3D) : Transform3D
		{
			var z_axis		: Vector3D		= null;
			var	x_axis		: Vector3D		= null;
			var	y_axis		: Vector3D		= null;
			var	m41			: Number		= 0.;
			var	m42			: Number		= 0.;
			var	m43			: Number		= 0.;
			var matrix		: Transform3D	= TRANSFORM.create();
			
			z_axis = myEyeDirection.subtract(myEyePosition);
			z_axis.normalize();
			x_axis = myUp.crossProduct(z_axis);
			x_axis.normalize();
			y_axis = z_axis.crossProduct(x_axis);
			y_axis.normalize();
			
			m41 = -x_axis.dotProduct(myEyePosition);
			m42 = -y_axis.dotProduct(myEyePosition);
			m43 = -z_axis.dotProduct(myEyePosition);
			
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
		public static function lookAtRightHanded(myEyePosition 	: Vector3D,
												 myEyeDirection	: Vector3D,
												 myUp			: Vector3D) : Transform3D
		{
			var z_axis		: Vector3D		= null;
			var	x_axis		: Vector3D		= null;
			var	y_axis		: Vector3D		= null;
			var	m41			: Number		= 0.;
			var	m42			: Number		= 0.;
			var	m43			: Number		= 0.;
			var matrix		: Transform3D	= TRANSFORM.create();
			
			z_axis = myEyePosition.subtract(myEyeDirection);
			z_axis.normalize();
			x_axis = myUp.crossProduct(z_axis);
			x_axis.normalize();
			y_axis = z_axis.crossProduct(x_axis);
			y_axis.normalize();
			
			m41 = -x_axis.dotProduct(myEyePosition);
			m42 = -y_axis.dotProduct(myEyePosition),
			m43 = -z_axis.dotProduct(myEyePosition);

			matrix.setRawData(Vector.<Number>([x_axis.x,	y_axis.x,	z_axis.x,	0.,
											   x_axis.y,	y_axis.y,	z_axis.y,	0.,
											   x_axis.z,	y_axis.z,	z_axis.z,	0.,
											   m41,			m42,		m43,		1.]))
			
			return matrix;
		}
		
		public static  function perspectiveFovLH(myFov		: Number,
												 myRatio	: Number,
												 myZNear	: Number,
												 myZFar 	: Number) : Transform3D
		{
			var	y_scale		: Number		= 1. / Math.tan(myFov / 2.0);
			var	x_scale		: Number		= y_scale / myRatio;
			var	m33			: Number		= myZFar / (myZFar - myZNear);
			var	m43			: Number		= -myZNear * myZFar / (myZFar - myZNear);
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
				_invalidate = true;
				_matrix.rawData = rawData;
			}
		}
		
		private function updateRawData() : void
		{
			_update = false;
			_matrix.recompose(_components, ORIENTATION);
		}
		
		private function invalidateComponents() : void
		{
			_invalidate = false;
			_components = _matrix.decompose(ORIENTATION);
			_position = _components[0];
			_rotation = _components[1];
			_scale = _components[2];
		}
		
		public function append(lhs : Transform3D) : Transform3D
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			lhs._update && lhs.updateRawData();
			_matrix.append(lhs._matrix);
			
			return this;
		}
		
		public function appendRotation(radians		: Number,
									   axis			: Vector3D,
									   pivotPoint	: Vector3D	= null) : Transform3D
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			_matrix.appendRotation(radians * RAD2DEG, axis, pivotPoint);
			
			return this;
		}
		
		public function appendScale(xScale	: Number,
									yScale	: Number	= 1.,
									zScale	: Number	= 1.) : Transform3D
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			_matrix.appendScale(xScale, yScale, zScale);
			
			return this;
		}
		
		public function appendUniformScale(scale : Number) : Transform3D
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			_matrix.appendScale(scale, scale, scale);
			
			return this;
		}
		
		public function appendTranslation(x : Number,
										  y : Number = 0.,
										  z : Number = 0.) : Transform3D
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			_matrix.appendTranslation(x, y, z);
			
			return this;
		}
		
		public function identity() : Transform3D
		{
			++_version;
			_invalidate = true;
			_matrix.identity();
			
			return this;
		}
		
		public function interpolateTo(toMat : Transform3D, percent : Number) : Transform3D
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			_matrix.interpolateTo(toMat._matrix, percent);
			
			return this;
		}
		
		public function invert() : Boolean
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			
			return _matrix.invert();
		}
		
		public function pointAt(pos	: Vector3D,
								at	: Vector3D	= null,
								up	: Vector3D	= null) : Transform3D
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			_matrix.pointAt(pos, at, up);
			
			return this;
		}
		
		public function prepend(rhs : Transform3D) : Transform3D
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			rhs._update && rhs.updateRawData();
			_matrix.prepend(rhs._matrix);
			
			return this;
		}
		
		public function prependRotation(radians		: Number,
										axis		: Vector3D,
										pivotPoint	: Vector3D = null) : Transform3D
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			_matrix.prependRotation(radians * RAD2DEG, axis, pivotPoint);
			
			return this;
		}
		
		public function prependScale(xScale	: Number,
									 yScale	: Number = 1.,
									 zScale	: Number = 1.) : Transform3D
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			_matrix.prependScale(xScale, yScale, zScale);
			
			return this;
		}
		
		public function prependTranslation(x : Number,
										   y : Number	= 1.,
										   z : Number	= 1.) : Transform3D
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			_matrix.prependTranslation(x, y, z);
			
			return this;
		}
		
		public function recompose(components		: Vector.<Vector3D>,
								  orientationStyle	: String = "eulerAngles") : Boolean
		{
			++_version;
			
			return _matrix.recompose(components, orientationStyle);
		}
		
		public function transpose() : Transform3D
		{
			++_version;
			_invalidate = true;
			_update && updateRawData();
			_matrix.transpose();
			
			return this;
		}
		
		public function setRotation(myX : Number = 0.,
									myY : Number = 0.,
									myZ : Number = 0.) : Transform3D
		{
			_rotation.x = myX;
			_rotation.y = myY;
			_rotation.z = myZ;
			
			++_version;
			_update = true;
			
			return this;
		}
		
		public function setScale(myX : Number = 1.,
								 myY : Number = 1.,
								 myZ : Number = 1.) : Transform3D
		{
			_scale.x = myX;
			_scale.y = myY;
			_scale.z = myZ;
			
			++_version;
			_update = true;
			
			return this;
		}
		
		public function setPosition(pos : Vector3D) : Transform3D
		{
			_position.x = pos.x;
			_position.y = pos.y;
			_position.z = pos.z;
			
			++_version;
			_update = true;
			
			return this;
		}
		
		public function clone(myTemporary : Boolean = false) : Transform3D
		{
			var clone : Transform3D = TRANSFORM.create(myTemporary);
			
			clone.setRawData(_matrix.rawData);
			
			return clone;
		}
		
		public function transformVector(myVector : Vector3D) : Vector3D
		{
			return _matrix.transformVector(myVector);
		}
		
		public function deltaTransformVector(myVector : Vector3D) : Vector3D
		{
			return _matrix.deltaTransformVector(myVector);
		}
		
		public function setRawData(v : Vector.<Number>) : Transform3D
		{
			++_version;
			_invalidate = true;
			_matrix.rawData = v;
			
			return this;
		}
		
		public function getRawData() : Vector.<Number>
		{
			_update && updateRawData();
			
			return _matrix.rawData;
		}
		
		public function decompose(myOrientation : String = "eulerAngles") : Vector.<Vector3D>
		{
			_update && updateRawData();
			
			return _matrix.decompose(myOrientation);
		}
		
		public function transformVectors(myIn 	: Vector.<Number>,
										 myOut	: Vector.<Number>) : void
		{
			_update && updateRawData();
			_matrix.transformVectors(myIn, myOut);
		}
		
		public function getPosition() : Vector3D
		{
			_invalidate && invalidateComponents()();
			
			return _position;
		}
		
		public function getRotation() : Vector3D
		{
			_invalidate && invalidateComponents();
			
			return _rotation;
		}
		
		public function getScale() : Vector3D
		{
			_invalidate && invalidateComponents();
			
			return _scale;
		}
				
		public function projectVector(myVector : Vector3D) : Vector3D
		{
			_update && updateRawData();
			
			return Utils3D.projectVector(_matrix, myVector);
		}
		
		public function projectVectors(myIn 	: Vector.<Number>,
									   myOut	: Vector.<Number>,
									   myUVT	: Vector.<Number>) : void
		{
			_update && updateRawData();
			Utils3D.projectVectors(_matrix, myIn, myOut, myUVT);
		}
	}
}