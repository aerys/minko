package aerys.minko.type.interpolation
{
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.ConstVector4;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 */
	public class AbstractSegment implements IInterpolation
	{
		private static const TMP_VECTOR : Vector.<Number> = new Vector.<Number>;
		
		private static const DEFAULT_AT	: ConstVector4 = new ConstVector4(0, 0, -1);
		private static const DEFAULT_UP	: ConstVector4 = new ConstVector4(0, -1, 0);
		private static const ITERATIONS : uint = 20;
		
		protected var _at			: Vector4;
		protected var _up			: Vector4;
		
		protected var _start		: Vector4;
		protected var _end			: Vector4;
		
		protected var _tmpPosT		: Number;
		protected var _tmpTangentT	: Number;
		protected var _tmpPointAtT	: Number;
		
		protected var _length		: Number;
		protected var _tmpPos		: Vector4;
		protected var _tmpTangent	: Vector4;
		protected var _tmpPointAt	: Vector4;
		
		public function get checkpoints() : Vector.<Vector4>
		{
			return new Vector.<Vector4>([_start, _end]);
		}
		
		public function get start() : Vector4
		{
			return _start;
		}
		
		public function get end() : Vector4
		{
			return _end;
		}
		
		public function set start(value : Vector4) : void
		{
			_start = value;
			_length = NaN;
		}
		
		public function set end(value : Vector4) : void
		{
			_end = value;
			_length = NaN;
		}
		
		/**
		 * Approximate length of the curb.
		 * 
		 * The exact length value would be:
		 * 		integral[0, 1, sqrt(tangent(t).x ^ 2 + tangent(t).y ^ 2 + tangent(t).z ^ 2) * dt]
		 * which has only nonelementary solutions for all curbs, but the linear one.
		 * 
		 * @see http://fr.wikipedia.org/wiki/Longueur_d%27un_arc#Formulation
		 * @see http://www.numberempire.com/integralcalculator.php
		 * @see http://www.math.unt.edu/integration_bee/AwfulTruth.html
		 */
		public function get length() : Number
		{
			if (isNaN(_length))
			{
				var tmpVector1 : Vector4 = new Vector4();
				var tmpVector2 : Vector4 = new Vector4();
				
				_length = 0;
				position(0, tmpVector1);
				for (var i : int = 0; i < ITERATIONS; ++i)
				{
					position((i + 1) / ITERATIONS, tmpVector2);
					
					_length += Math.sqrt(
						Math.pow(tmpVector2.x - tmpVector1.x, 2)
						+ Math.pow(tmpVector2.y - tmpVector1.y, 2)
						+ Math.pow(tmpVector2.z - tmpVector1.z, 2)
					);
						
					Vector4.copy(tmpVector2, tmpVector1);
				}
			}
			return _length;
		}
		
		public function AbstractSegment(start	: Vector4,
										end		: Vector4,
										at		: Vector4 = null, 
										up		: Vector4 = null)
		{
			_at			= at || DEFAULT_AT;
			_up			= up || DEFAULT_UP;
			
			_start		= start;
			_end		= end;
			
			_length			= NaN;
			_tmpPosT		= -1;
			_tmpTangentT	= -1;
			_tmpPointAtT	= -1;
			
			_tmpPos		= new Vector4();
			_tmpTangent	= new Vector4();
			_tmpPointAt	= new Vector4();
		}
		
		public function position(t : Number, out : Vector4) : Vector4
		{
			if (t < 0 || t > 1)
				throw new Error('t must be between 0 and 1');
			
			updatePosition(t);
			return Vector4.copy(_tmpPos, out);
		}
		
		/**
		 * Alias for position
		 */
		public function translation(t : Number, out : Vector4) : Vector4
		{
			return position(t, out);
		}
		
		public function tangent(t : Number, out : Vector4) : Vector4
		{
			if (t < 0 || t > 1)
				throw new Error('t must be between 0 and 1');
			
			updateTangent(t);
			return Vector4.copy(_tmpTangent, out);
		}
		
		/**
		 * Alias for tangent
		 */
		public function lookAt(t : Number, out : Vector4) : Vector4
		{
			return tangent(t, out);
		}
		
		public function pointAt(t : Number, out : Vector4) : Vector4
		{
			if (t < 0 || t > 1)
				throw new Error('t must be between 0 and 1');
			
			updatePointAt(t);
			return Vector4.copy(_tmpTangent, out);
		}
		
		public function updateMatrix(matrix : Matrix4x4, t : Number) : void
		{
			if (t < 0 || t > 1)
				throw new Error('t must be between 0 and 1');
			
			updatePosition(t);
			updatePointAt(t);
			
			matrix.getRawData(TMP_VECTOR);
			TMP_VECTOR[12] = _tmpPos.x;
			TMP_VECTOR[13] = _tmpPos.y;
			TMP_VECTOR[14] = _tmpPos.z;
			matrix.setRawData(TMP_VECTOR);
			matrix.pointAt(_tmpPointAt, _at, _up);
		}
		
		public function updateTransform(transform : Transform3D, t : Number) : void
		{
			if (t < 0 || t > 1)
				throw new Error('t must be between 0 and 1');
			
			updatePosition(t);
			updatePointAt(t);
			
			Vector4.copy(_tmpPos, transform.position);
			transform.pointAt(_tmpPointAt, _at, _up);
		}
		
		protected function updatePosition(t : Number) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function updateTangent(t : Number) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function updatePointAt(t : Number) : void
		{
			if (_tmpPointAtT == t)
				return;
			
			updatePosition(t);
			updateTangent(t);
			
			Vector4.subtract(_tmpPos, _tmpTangent, _tmpPointAt);
			
			_tmpPointAtT = t;
		}
	}
}
