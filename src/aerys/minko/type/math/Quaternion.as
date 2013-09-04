package aerys.minko.type.math
{
	import aerys.minko.type.Factory;

	public final class Quaternion
	{
		private static const FACTORY				: Factory			= Factory.getFactory(Quaternion);
		private static const EPSILON				: Number			= 1e-6;
		private static const TMP_QUATERNION			: Quaternion		= new Quaternion();

		private var _r		: Number	= 1.;
		private var _i		: Number	= 0.;
		private var _j		: Number	= 0.;
		private var _k		: Number	= 0.;
		
		final public function get r() : Number
		{
			return _r;
		}
		
		final public function get i() : Number
		{
			return _i;
		}
		
		final public function get j() : Number
		{
			return _j;
		}
		
		final public function get k() : Number
		{
			return _k;
		}

		public function get length() : Number
		{
			return Math.sqrt(_r * _r + _i * _i + _j * _j + _k * _k);
		}

		public function get norm() : Number
		{
			return length;
		}
		
		public static function add(q1 : Quaternion, q2 : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= FACTORY.create() as Quaternion;
			out.copyFrom(q1)
			return out.incrementBy(q2);
		}

		public static function subtract(q1 : Quaternion, q2 : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= FACTORY.create() as Quaternion;
			out.copyFrom(q1)
			return out.decrementBy(q2);
		}

		public static function scale(q : Quaternion, f : Number, out : Quaternion = null) : Quaternion
		{
			out ||= FACTORY.create() as Quaternion;
			out.copyFrom(q)
			return out.scaleBy(f);
		}
		
		public static function multiply(q1 : Quaternion, q2 : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= FACTORY.create() as Quaternion;
			out.copyFrom(q1);
			return out.multiplyBy(q2);
		}

		public static function divide(q1 : Quaternion, q2 : Quaternion, out : Quaternion = null) : Quaternion
		{
			Quaternion.invert(q2, TMP_QUATERNION);
			Quaternion.multiply(q1, TMP_QUATERNION, out);
			return out;
		}

		public static function negate(q : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= FACTORY.create() as Quaternion;
			out.copyFrom(q);
			return out.negate();
		}

		public static function conjugate(q : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= FACTORY.create() as Quaternion;
			out.copyFrom(q);
			return out.conjugate();
		}

		public static function square(q : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= FACTORY.create() as Quaternion;
			out.copyFrom(q);
			return out.square();
		}

		public static function invert(q : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= FACTORY.create() as Quaternion;
			out.copyFrom(q);
			return out.invert();
		}


		public function Quaternion(r : Number = 0,
								   i : Number = 0,
								   j : Number = 0,
								   k : Number = 0)
		{
			_r = r;
			_i = i;
			_j = j;
			_k = k;
		}
		
		public function set(r : Number,
							i : Number,
							j : Number,
							k : Number) : Quaternion
		{
			_r = r;
			_i = i;
			_j = j;
			_k = k;
			
			return this;
		}

		public function copyFrom(q : Quaternion) : Quaternion
		{
			return set(q._r, q._i, q._j, q._k);
		}
		
		public function decrementBy(q : Quaternion) : Quaternion
		{
			_r += q._r;
			_i += q._i;
			_j += q._j;
			_k += q._k;
			
			return this;
		}

		public function incrementBy(q : Quaternion) : Quaternion
		{
			_r += q._r;
			_i += q._i;
			_j += q._j;
			_k += q._k;

			return this;
		}

		public function scaleBy(f : Number) : Quaternion
		{
			_r *= f;
			_i *= f;
			_j *= f;
			_k *= f;

			return this;
		}

		public function multiplyBy(q : Quaternion) : Quaternion
		{
			var t0 : Number = (_k - _j) * (q._j - q._k);
			var t1 : Number = (_r + _i) * (q._r + q._i);
			var t2 : Number = (_r - _i) * (q._j + q._k);
			var t3 : Number = (_k + _j) * (q._r - q._i);
			var t4 : Number = (_k - _i) * (q._i - q._j);
			var t5 : Number = (_k + _i) * (q._i + q._j);
			var t6 : Number = (_r + _j) * (q._r - q._k);
			var t7 : Number = (_r - _j) * (q._r + q._k);
			var t8 : Number = t5 + t6 + t7;
			var t9 : Number = (t4 + t8) / 2;
			
			_r = t0 + t9 - t5;
			_i = t1 + t9 - t8;
			_j = t2 + t9 - t7;
			_k = t3 + t9 - t6;
			
			return this;
		}

		public function normalize(q : Quaternion) : Quaternion
		{
			var l	: Number	= length;
			
			if (l != 0.)
			{
				_r *= l;
				_i *= l;
				_j *= l;
				_k *= l;
			}
			
			return this;
		}
		
		public function divide(q : Quaternion) : Quaternion
		{
			return Quaternion.divide(this, q, this);
		}

		public function negate() : Quaternion
		{
			return scaleBy(-1);
		}

		public function conjugate() : Quaternion
		{
			_i = -_i;
			_j = -_j;
			_k = -_k;

			return this;
		}

		public function square() : Quaternion
		{
			var tmp : Number = 2 * _r;
			
			_r = _r * _r - (_i * _i + _j * _j + _k * _k);
			_i = tmp * _i;
			_j = tmp * _j;
			_k = tmp * _k;
			
			return this;
		}

		public function invert() : Quaternion
		{
			var l : Number = length;
			
			_r /= l;
			_i /= -l;
			_j /= -l;
			_k /= -l;

			return this;
		}
		
		public function equals(q : Quaternion) : Boolean
		{
			return _r == q._r && _i == q._i && _j == q._j && _k == q._k;
		}
		
		public function clone() : Quaternion
		{
			return Quaternion(FACTORY.create()).set(_r, _i, _j, _k);
		}

		public function toString() : String
		{
			return '[Quaternion r="' + _r + '" i="' + _i + '" j="' + _j + '" k="' + _k + '"]';
		}

		public static function fromRotationBetweenVectors(u : Vector4, v : Vector4) : Quaternion
		{
			u.normalize();
			v.normalize();
			
			var kCosTheta		: Number	= Vector4.dotProduct(u, v)
			var k				: Number	= Math.sqrt(u.lengthSquared * v.lengthSquared);
			
			if (Math.abs(kCosTheta / k + 1) < EPSILON)
			{
				// 180 degree rotation around any orthogonal vector
				var other		: Vector4	= (Math.abs(Vector4.dotProduct(u, Vector4.X_AXIS)) < 1.0) ? Vector4.X_AXIS : Vector4.Y_AXIS;
				var axis		: Vector4	= Vector4.crossProduct(u, other).normalize();
				
				return fromRotationOnAxis(axis, Math.PI);
			}
			
			var c	: Vector4		= Vector4.crossProduct(u, v);
			var d	: Number		= Vector4.dotProduct(u, v);
			var q	: Quaternion	= Quaternion(FACTORY.create()).set(d + k, c.x, c.y, c.z);
			
			return q.scaleBy(1. / q.norm);
		}
		
		private static function fromRotationOnAxis(axis : Vector4, theta : Number) : Quaternion
		{
			theta *= .5;
			axis.normalize();
			
			var sine    : Number		= Math.sin(theta);
			
			var out		: Quaternion	= Quaternion(FACTORY.create()).set(
				Math.cos(theta),
				axis.x * sine,
				axis.y * sine,
				axis.z * sine
			);
			
			return out;
		}
	}
}