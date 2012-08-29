package aerys.minko.type.math
{
	public class HLSAMatrix4x4 extends Matrix4x4
	{
		private static const TMP_NUMBERS	: Vector.<Number> 	= new Vector.<Number>(16, true);
		private static const TMP_MATRIX		: Matrix4x4			= new Matrix4x4();
		private static const TMP_VECTOR4	: Vector4			= new Vector4();
		private static const RWGT			: Number 			= 0.3086;
		private static const GWGT			: Number 			= 0.6094;
		private static const BWGT			: Number 			= 0.0820;
		private static const SQRT2			: Number			= Math.sqrt(2.0);
		private static const SQRT3			: Number			= Math.sqrt(3.0);
		private static const INV_SQRT2		: Number			= 1. / SQRT2;
		
		private var _hue				: Number		= 0;
		private var _saturation			: Number		= 1;
		private var _luminance			: Number		= 1;
		private var _alpha				: Number		= 1;
		private var _hueMatrix			: Matrix4x4		= new Matrix4x4();
		private var _saturationMatrix	: Matrix4x4		= new Matrix4x4();
		private var _luminanceMatrix	: Matrix4x4		= new Matrix4x4();
		private var _luminancePlane 	: Vector4		= new Vector4(RWGT, GWGT, BWGT, 0);
		
		/**
		 * Alpha multiplier, from 0 to 1, default is 1.
		 */
		public function get alpha() : Number
		{
			return _alpha
		}
		public function set alpha(value : Number) : void
		{
			_alpha = value;
			
			updateDiffuseColorMatrix();
		}
		
		/**
		 * Hue correction, from -2PI to 2PI, default is 0.
		 */
		public function get hue() : Number
		{
			return _hue;
		}
		public function set hue(value : Number) : void
		{
			_hue = value;
			
			// First we make an identity matrix
			_hueMatrix.identity();
			
			// Rotate the grey vector into positive Z
			TMP_MATRIX.initialize(
				1,	0, 			0, 			0,
				0, 	INV_SQRT2, 	INV_SQRT2, 	0,
				0, 	-INV_SQRT2, INV_SQRT2, 	0,
				0, 	0, 			0, 			1
			);
			_hueMatrix.append(TMP_MATRIX);
			
			var mag : Number	= SQRT3;
			var yrs	: Number 	= -1.0 / mag;
			var yrc	: Number 	= SQRT2 / mag;
			
			TMP_MATRIX.initialize(
				yrc,	0,	-yrs,	0,
				0,		1,	0,		0,
				yrs,	0,	yrc,	0,
				0,		0,	0,		1
			);
			_hueMatrix.append(TMP_MATRIX);
			
			// Shear the space to make the luminance plane horizontal
			var l	: Vector4 	= _hueMatrix.transformVector(_luminancePlane, TMP_VECTOR4);
			var zsx	: Number 	= l.x / l.z;
			var zsy	: Number 	= l.y / l.z;
			
			TMP_MATRIX.initialize(
				1,	0,	zsx,	0,
				0,	1,	zsy,	0,
				0,	0,	1,		0,
				0,	0,	0,		1
			);
			_hueMatrix.append(TMP_MATRIX);
			
			// Rotate the hue
			var zrs	: Number = Math.sin(value);
			var zrc	: Number = Math.cos(value);
			
			TMP_MATRIX.initialize(
				zrc,	zrs,	0,	0,
				-zrs,	zrc,	0,	0,
				0,		0,		1,	0,
				0,		0,		0,	1
			);
			_hueMatrix.append(TMP_MATRIX);
			
			// Unshear the space to put the luminance plane back
			TMP_MATRIX.initialize(
				1,	0,	-zsx,	0,
				0,	1,	-zsy,	0,
				0,	0,	1,		0,
				0,	0,	0,		1
			);
			_hueMatrix.append(TMP_MATRIX);
			
			// Rotate the grey vector back into place
			TMP_MATRIX.initialize(
				yrc,	0,	yrs,	0,
				0,		1,	0,		0,
				-yrs,	0,	yrc,	0,
				0,		0,	0,		1
			);
			_hueMatrix.append(TMP_MATRIX);
			TMP_MATRIX.initialize(
				1,	0,			0,			0,
				0,	INV_SQRT2,	-INV_SQRT2,	0,
				0,	INV_SQRT2,	INV_SQRT2,	0,
				0,	0,			0,			1
			);
			_hueMatrix.append(TMP_MATRIX);
			
			updateDiffuseColorMatrix();
		}
		
		/**
		 * Saturation multiplier, default is 1.
		 */
		public function get saturation() : Number
		{
			return _saturation;
		}
		public function set saturation(s : Number):void
		{
			_saturation = s;
			
			var s1	: Number = 1.0 - s;
			var a	: Number = s1 * RWGT + s;
			var b	: Number = s1 * RWGT;
			var c	: Number = s1 * RWGT;
			var d	: Number = s1 * GWGT;
			var e	: Number = s1 * GWGT + s;
			var f	: Number = s1 * GWGT;
			var g	: Number = s1 * BWGT;
			var h	: Number = s1 * BWGT;
			var i	: Number = s1 * BWGT + s;
			
			_saturationMatrix.initialize(
				a, b, c, 0,
				d, e, f, 0,
				g, h, i, 0,
				0, 0, 0, 1
			);
			
			updateDiffuseColorMatrix();
		}
		
		/**
		 * Luminance multiplier, default is 1.
		 */
		public function get luminance() : Number
		{
			return _luminance;
		}
		public function set luminance(l : Number) : void
		{
			_luminance = l;
			_luminanceMatrix
				.lock()
				.identity()
				.appendUniformScale(l)
				.unlock();
			
			updateDiffuseColorMatrix();
		}
		
		public function HLSAMatrix4x4(hue 			: Number	= 0.,
									  luminance 	: Number	= 1.,
									  saturation 	: Number	= 1.,
									  alpha 		: Number	= 1.)
		{
			super();
			
			this.hue = hue;
			this.luminance = luminance;
			this.saturation = saturation;
			this.alpha = alpha;
			
			updateDiffuseColorMatrix();
		}
		
		private function updateDiffuseColorMatrix() : void
		{
			lock();
			identity();
			append(_hueMatrix);
			append(_saturationMatrix);
			append(_luminanceMatrix);
			setColumn(3, new Vector4(0, 0, 0, _alpha));
			unlock();
		}
		
		override public function clone() : Matrix4x4
		{
			return new HLSAMatrix4x4(_hue, _luminance, _saturation, _alpha);
		}
	}
}