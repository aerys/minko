package aerys.minko.type.math
{
	import aerys.common.Factory;
	import aerys.common.IVersionnable;
	import aerys.minko.ns.minko;
	
	import flash.geom.Vector3D;

	public final class Vector4 implements IVersionnable
	{
		use namespace minko;
		
		private static const FACTORY			: Factory	= Factory.getFactory(Vector4);
		
		public static const X_AXIS				: Vector4	= new Vector4(1., 0., 0.);
		public static const Y_AXIS				: Vector4	= new Vector4(0., 1., 0.);
		public static const Z_AXIS				: Vector4	= new Vector4(0., 0., 1.);
		public static const ZERO				: Vector4	= new Vector4();
		public static const ONE					: Vector4	= new Vector4(1., 1., 1.);
		
		private static const UPDATE_NONE		: uint		= 0;
		private static const UPDATE_LENGTH		: uint		= 1;
		private static const UPDATE_LENGTH_SQ	: uint		= 2;
		private static const UPDATE_ALL			: uint		= UPDATE_LENGTH | UPDATE_LENGTH_SQ;
		
		private var _version	: uint		= 0;
		private var _update		: uint		= UPDATE_NONE;
		
		minko var _vector		: Vector3D	= new Vector3D();
		
		private var _length		: Number	= 0.;
		private var _lengthSq	: Number	= 0.;
		
		public function get version()	: uint		{ return _version; }
		public function get x()			: Number	{ return _vector.x; }
		public function get y()			: Number	{ return _vector.y; }
		public function get z()			: Number	{ return _vector.z; }
		public function get w()			: Number	{ return _vector.w; }
		
		public function get lengthSquared()	: Number
		{
			if (_update & UPDATE_LENGTH_SQ)
			{
				_update ^= UPDATE_LENGTH_SQ;
				_lengthSq = _vector.x * _vector.x + _vector.y * _vector.y
							+ _vector.z * _vector.z;
			}
			
			return _lengthSq;
		}
		
		public function get length()	: Number
		{
			if (_update & UPDATE_LENGTH)
			{
				_update ^= UPDATE_LENGTH;
				_length = Math.sqrt(lengthSquared);
			}
			
			return _length;
		}
		
		public function set x(value : Number) : void
		{
			if (value != _vector.x)
			{
				_vector.x = value;
				_update = UPDATE_ALL;
				++_version;
			}
		}
		
		public function set y(value : Number) : void
		{
			if (value != _vector.y)
			{
				_vector.y = value;
				_update = UPDATE_ALL;
				++_version;
			}
		}
		
		public function set z(value : Number) : void
		{
			if (value != _vector.z)
			{
				_vector.z = value;
				_update = UPDATE_ALL;
				++_version;
			}
		}
		
		public function set w(value : Number) : void
		{
			if (value != _vector.w)
			{
				_vector.w = value;
				_update = UPDATE_ALL;
				++_version;
			}
		}
		
		public function Vector4(x 	: Number	= 0.,
								y	: Number	= 0.,
								z	: Number	= 0.,
								w 	: Number	= 0.)
		{
			_vector.x = x;
			_vector.y = y;
			_vector.z = z;
			_vector.w = w;
			
			_update = UPDATE_ALL;
		}
		
		public static function add(u : Vector4, v : Vector4) : Vector4
		{
			return new Vector4(u._vector.x + v._vector.x,
							   u._vector.y + v._vector.y,
							   u._vector.z + v._vector.z,
							   u._vector.w + v._vector.w);
		}
		
		public static function subtract(u : Vector4, v : Vector4) : Vector4
		{
			return new Vector4(u._vector.x - v._vector.x,
							   u._vector.y - v._vector.y,
							   u._vector.z - v._vector.z,
							   u._vector.w - v._vector.w);
		}
		
		public static function dotProduct(u : Vector4, v : Vector4) : Number
		{
			return u._vector.x * v._vector.x
				   + u._vector.y * v._vector.y
				   + u._vector.z * v._vector.z;
		}
		
		public static function crossProduct(u : Vector4, v : Vector4) : Vector4
		{
			return new Vector4(u._vector.y * v._vector.z - v._vector.y * u._vector.z,
							   u._vector.z * v._vector.x - v._vector.z * u._vector.x,
							   u._vector.x * v._vector.y - v._vector.x * u._vector.y);
		}
		
		public static function equals(u : Vector4, v : Vector4, allFour : Boolean = false) : Boolean
		{
			return u._vector.x == v._vector.x
				   && u._vector.y == v._vector.y
				   && u._vector.z == v._vector.z
				   && (!allFour || (u._vector.w == v._vector.w));
		}
		
		public static function distance(u : Vector4, v : Vector4) : Number
		{
			var x : Number = v._vector.x - u._vector.x;
			var y : Number = v._vector.y - u._vector.y;
			var z : Number = v._vector.z - u._vector.z;
			
			return Math.sqrt(x * x + y * y + z * z);
		}
		
		public function add(vector : Vector4) : Vector4
		{
			_vector.x += vector._vector.x;
			_vector.y += vector._vector.y;
			_vector.z += vector._vector.z;
			_vector.w += vector._vector.w;
			
			++_version;
			_update = UPDATE_ALL;
			
			return this;
		}
		
		public function subtract(vector : Vector4) : Vector4
		{
			_vector.x -= vector._vector.x;
			_vector.y -= vector._vector.y;
			_vector.z -= vector._vector.z;
			_vector.w -= vector._vector.w;
			
			++_version;
			_update = UPDATE_ALL;
			
			return this;
		}
		
		public function scaleBy(scale : Number) : Vector4
		{
			_vector.x *= scale;
			_vector.y *= scale;
			_vector.z *= scale;
			_vector.w *= scale;
			
			++_version;
			_update = UPDATE_ALL;

			return this;
		}
		
		public function project() : Vector4
		{
			_vector.x /= _vector.w;
			_vector.y /= _vector.w;
			_vector.z /= _vector.w;
			
			++_version;
			_update = UPDATE_ALL;
			
			return this;
		}
		
		public function normalize() : Vector4
		{
			var l : Number = length;
			
			if (l != 0.)
			{
				_vector.x /= l;
				_vector.y /= l;
				_vector.z /= l;
				
				_length = 1.;
				_lengthSq = 1.;
				
				++_version;
				_update = UPDATE_NONE;
			}
			
			return this;
		}
		
		public function crossProduct(vector : Vector4) : Vector4
		{
			var x : Number = _vector.x;
			var y : Number = _vector.y;
			var z : Number = _vector.z;
			
			_vector.x = y * vector._vector.z - vector._vector.y * z;
			_vector.y = z * vector._vector.x - vector._vector.z * x;
			_vector.z = x * vector._vector.y - vector._vector.x * y;
			
			_update = UPDATE_ALL;
			++_version;
			
			return this;
		}
		
		public function clone() : Vector4
		{
			return new Vector4(_vector.x, _vector.y, _vector.z, _vector.w);
		}
		
		public function getVector3D() : Vector3D
		{
			return _vector.clone();
		}
		
		public function set(x : Number,
							y : Number,
							z : Number,
							w : Number = 1.) : Vector4
		{
			if (x != _vector.x || y != _vector.y || z != _vector.z || w != _vector.w)
			{
				_vector.x = x;
				_vector.y = y;
				_vector.z = z;
				_vector.w = w;
			
				++_version;
				_update = UPDATE_ALL;
			}
			
			return this;
		}
		
		public function toString() : String
		{
			return "(" + _vector.x + ", " + _vector.y + ", " + _vector.z + ", "
				   + _vector.w + ")";
		}
		
		public static function copy(source : Vector4, target : Vector4 = null) : Vector4
		{
			target ||= FACTORY.create();
			target.set(source._vector.x,
					   source._vector.y,
					   source._vector.z,
					   source._vector.w);
			
			return target;
		}
		
		public static function scale(v : Vector4, s : Number, out : Vector4 = null) : Vector4
		{
			out ||= FACTORY.create();
			out._vector.x = v._vector.x;
			out._vector.y = v._vector.y;
			out._vector.z = v._vector.z;
			out._vector.w = v._vector.w;
			
			return out;
		}
		
		public static function normalize(v : Vector4, out : Vector4 = null) : Vector4
		{
			out = copy(v, out);
			return out.normalize();
		}
	}
}