package aerys.minko.type.math
{
	public final class Quaternion
	{
		private static const TMP_QUATERNION : Quaternion = new Quaternion();

		public function get r() : Number { return _r; }
		public function get i() : Number { return _i; }
		public function get j() : Number { return _j; }
		public function get k() : Number { return _k; }

		protected var _r : Number;
		protected var _i : Number;
		protected var _j : Number;
		protected var _k : Number;

		public static function isEqual(q1 : Quaternion, q2 : Quaternion) : Boolean
		{
			return q1._r == q2._r && q1._i == q2._i && q1._j == q2._j && q1._k == q2._k;
		}

		public static function isNotEqual(q1 : Quaternion, q2 : Quaternion) : Boolean
		{
			return !isEqual(q1, q2);
		}

		public static function norm(q : Quaternion) : Number
		{
			return q._r * q._r + q._i * q._i + q._j * q._j + q._k * q._k;
		}

		public static function absolute(q : Quaternion) : Number
		{
			return Math.sqrt(Quaternion.norm(q));
		}

		public static function add(q1 : Quaternion, q2 : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= new Quaternion();
			out._r = q1._r + q2._r;
			out._i = q1._i + q2._i;
			out._j = q1._j + q2._j;
			out._k = q1._k + q2._k;
			return out;
		}

		public static function subtract(q1 : Quaternion, q2 : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= new Quaternion();
			out._r = q1._r - q2._r;
			out._i = q1._i - q2._i;
			out._j = q1._j - q2._j;
			out._k = q1._k - q2._k;
			return out;
		}

		public static function multiplyScalar(q : Quaternion, f : Number, out : Quaternion = null) : Quaternion
		{
			out ||= new Quaternion();
			out._r = q._r * f;
			out._i = q._i * f;
			out._j = q._j * f;
			out._k = q._k * f;
			return out;
		}

		public static function multiply(q1 : Quaternion, q2 : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= new Quaternion();

			var t0 : Number = (q1._k - q1._j) * (q2._j - q2._k);
			var t1 : Number = (q1._r + q1._i) * (q2._r + q2._i);
			var t2 : Number = (q1._r - q1._i) * (q2._j + q2._k);
			var t3 : Number = (q1._k + q1._j) * (q2._r - q2._i);
			var t4 : Number = (q1._k - q1._i) * (q2._i - q2._j);
			var t5 : Number = (q1._k + q1._i) * (q2._i + q2._j);
			var t6 : Number = (q1._r + q1._j) * (q2._r - q2._k);
			var t7 : Number = (q1._r - q1._j) * (q2._r + q2._k);
			var t8 : Number = t5 + t6 + t7;
			var t9 : Number = (t4 + t8) / 2;

			out._r = t0 + t9 - t5;
			out._i = t1 + t9 - t8;
			out._j = t2 + t9 - t7;
			out._k = t3 + t9 - t6;

			return out;
		}

		public static function divideScalar(q : Quaternion, f : Number, out : Quaternion = null) : Quaternion
		{
			return Quaternion.multiplyScalar(q, 1 / f, out);
		}

		public static function divide(q1 : Quaternion, q2 : Quaternion, out : Quaternion = null) : Quaternion
		{
			Quaternion.invert(q2, TMP_QUATERNION);
			Quaternion.multiply(q1, TMP_QUATERNION, out);
			return out;
		}

		public static function copy(q : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= new Quaternion();
			out._r = q._r;
			out._i = q._i;
			out._j = q._j;
			out._k = q._k;
			return out;
		}

		public static function negate(q : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= new Quaternion();
			out._r = - q._r;
			out._i = - q._i;
			out._j = - q._j;
			out._k = - q._k;
			return out;
		}

		public static function conjugate(q : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= new Quaternion();
			out._r = q._r;
			out._i = - q._i;
			out._j = - q._j;
			out._k = - q._k;
			return out;
		}

		public static function square(q : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= new Quaternion();

			var tmp : Number = 2 * q._r;
			out._r = q._r * q._r - (q._i * q._i + q._j * q._j + q._k * q._k);
			out._i = tmp * q._i;
			out._j = tmp * q._j;
			out._k = tmp * q._k;
			return out;
		}

		public static function invert(q : Quaternion, out : Quaternion = null) : Quaternion
		{
			out ||= new Quaternion();

			var tmp : Number = Quaternion.norm(q);
			out._r = q._r / tmp;
			out._i = - q._i / tmp;
			out._j = - q._j / tmp;
			out._k = - q._k / tmp;

			return out;
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

		public function isEqual(q : Quaternion) : Boolean
		{
			return Quaternion.isEqual(this, q);
		}

		public function isNotEqual(q : Quaternion) : Boolean
		{
			return Quaternion.isNotEqual(this, q);
		}

		public function subtract(q : Quaternion) : Quaternion
		{
			return Quaternion.subtract(this, q, this);
		}

		public function add(q : Quaternion) : Quaternion
		{
			return Quaternion.add(this, q, this);
		}

		public function multiplyScalar(f : Number) : Quaternion
		{
			return Quaternion.multiplyScalar(this, f, this);
		}

		public function multiply(q : Quaternion) : Quaternion
		{
			return Quaternion.multiply(this, q, this);
		}

		public function divideScalar(f : Number) : Quaternion
		{
			return Quaternion.divideScalar(this, f, this);
		}

		public function divide(q : Quaternion) : Quaternion
		{
			return Quaternion.divide(this, q, this);
		}

		public function clone() : Quaternion
		{
			return Quaternion.copy(this);
		}

		public function negate() : Quaternion
		{
			return Quaternion.negate(this, this);
		}

		public function conjugate() : Quaternion
		{
			return Quaternion.conjugate(this, this);
		}

		public function square() : Quaternion
		{
			return Quaternion.square(this, this);
		}

		public function invert() : Quaternion
		{
			return Quaternion.invert(this, this);
		}

		public function toString() : String
		{
			return '[Quaternion r="' + _r + '" i="' + _i + '" j="' + _j + '" k="' + _k + '"]';
		}

	}
}