package aerys.minko.scene.controller.mesh
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.data.DataProvider;
	import aerys.minko.type.math.*;

	/**
	 * This controller is used to apply hue, saturation, luminance and alpha transformations to the sampled diffuse
	 * color of any Mesh.
	 *
	 * @see http://www.graficaobscura.com/matrix/index.html
	 * @author Tortenazor
	 */
	public final class HLSAController extends AbstractController
	{
		private static var rwgt:Number = 0.3086;
		private static var gwgt:Number = 0.6094;
		private static var bwgt:Number = 0.0820;
		private static var TMP_NUMBERS:Vector.<Number> = new Vector.<Number>(16)
		private var _dataProvider			: DataProvider
		private var _hue	: Number	= 0
		private var _saturation	: Number	= 1
		private var _luminance	: Number	= 1
		private var _alpha	: Number	= 1
		private var _hueMatrix	: Matrix4x4
		private var _saturationMatrix	: Matrix4x4
		private var _luminanceMatrix	: Matrix4x4
		private var _finalMatrix	: Matrix4x4
		private var _tmp	: Matrix4x4
		private var _luminancePlane : Vector4

		/**
		 * This controller is used to apply hue, saturation, luminance and alpha transformations to the sampled diffuse
		 * color of any Mesh.
	 	 *
		 * @see http://www.graficaobscura.com/matrix/index.html
		 * @author Tortenazor
		 */
		public function HLSAController()
		{
			super(Mesh);

			_dataProvider = new DataProvider();
			_hueMatrix = new Matrix4x4();
			_saturationMatrix = new Matrix4x4();
			_luminanceMatrix = new Matrix4x4();
			_finalMatrix = new Matrix4x4();
			_tmp = new Matrix4x4();
			_luminancePlane = new Vector4(rwgt,gwgt,bwgt,0)
			updateDiffuseColorMatrix();

			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}

		/**
		 * Hue correction,from -2PI to 2PI, default 0
		 */
		public function set hue(h:Number):void
		{
			_hue = h;

			// First we make an identity matrix
			_hueMatrix.identity();

			// Rotate the grey vector into positive Z
			var mag:Number = Math.sqrt(2.0);
			var xrs:Number = 1.0/mag;
			var xrc:Number = 1.0/mag;
			_tmp.initialize(1,0,0,0,
								 0,xrc,xrs,0,
								 0,-xrs,xrc,0,
								 0,0,0,1)
			_hueMatrix.append(_tmp);

			mag = Math.sqrt(3.0);
			var yrs:Number = -1.0/mag;
			var yrc:Number = Math.sqrt(2.0)/mag;
			_tmp.initialize(yrc,0,-yrs,0,
								 0,1,0,0,
								 yrs,0,yrc,0,
								 0,0,0,1);
			_hueMatrix.append(_tmp);

			// Shear the space to make the luminance plane horizontal
			var l:Vector4 = _hueMatrix.transformVector(_luminancePlane);
    		var zsx:Number = l.x/l.z;
    		var zsy:Number = l.y/l.z;
    		_tmp.initialize(1,0,zsx,0,
								 0,1,zsy,0,
								 0,0,1,0,
								 0,0,0,1);
			_hueMatrix.append(_tmp);

			// Rotate the hue
			var zrs:Number = Math.sin(h);
			var zrc:Number = Math.cos(h);
			_tmp.initialize(zrc,zrs,0,0,
								 -zrs,zrc,0,0,
								 0,0,1,0,
								 0,0,0,1);
			_hueMatrix.append(_tmp);

			// Unshear the space to put the luminance plane back
    		_tmp.initialize(1,0,-zsx,0,
								 0,1,-zsy,0,
								 0,0,1,0,
								 0,0,0,1);
			_hueMatrix.append(_tmp);

			// Rotate the grey vector back into place
			_tmp.initialize(yrc,0,yrs,0,
								 0,1,0,0,
								 -yrs,0,yrc,0,
								 0,0,0,1);
			_hueMatrix.append(_tmp);
			_tmp.initialize(1,0,0,0,
								 0,xrc,-xrs,0,
								 0,xrs,xrc,0,
								 0,0,0,1);
			_hueMatrix.append(_tmp);

			updateDiffuseColorMatrix();
		}

		/**
		 * Hue correction, from -2PI to 2PI, default 0
		 */
		public function get hue():Number
		{
			return _hue;
		}

		/**
		 * Saturation multiplier, default 1
		 */
		public function set saturation(s:Number):void
		{
			_saturation = s;

			var s1:Number = 1.0-s;
			var a:Number = (s1)*rwgt + s;
			var b:Number = (s1)*rwgt;
			var c:Number = (s1)*rwgt;
			var d:Number = (s1)*gwgt;
			var e:Number = (s1)*gwgt + s;
			var f:Number = (s1)*gwgt;
			var g:Number = (s1)*bwgt;
			var h:Number = (s1)*bwgt;
			var i:Number = (s1)*bwgt + s;
			_saturationMatrix.initialize(a,b,c,0,
												  d,e,f,0,
												  g,h,i,0,
												  0,0,0,1);
			updateDiffuseColorMatrix();
		}

		/**
		 * Saturation multiplier, default 1
		 */
		public function get saturation():Number
		{
			return _saturation;
		}

		/**
		 * Luminance multiplier, default 1
		 */
		public function set luminance(l:Number):void
		{
			_luminance = l;
			_luminanceMatrix.initialize(l,0,0,0,
												 0,l,0,0,
												 0,0,l,0,
												 0,0,0,1);
			updateDiffuseColorMatrix();
		}

		/**
		 * Luminance multiplier, default 1
		 */
		public function get luminance():Number
		{
			return _luminance;
		}

		/**
		 * Alpha multiplier, from 0 to 1, default 1
		 */
		public function set alpha(a:Number):void
		{
			_alpha = a
		}

		/**
		 * Alpha multiplier, from 0 to 1, default 1
		 */
		public function get alpha():Number
		{
			return _alpha
		}

		private function updateDiffuseColorMatrix() : void
		{
			_finalMatrix.identity();
			_finalMatrix.append(_hueMatrix);
			_finalMatrix.append(_saturationMatrix);
			_finalMatrix.append(_luminanceMatrix);
			HLSAController.TMP_NUMBERS = _finalMatrix.getRawData(TMP_NUMBERS);
			HLSAController.TMP_NUMBERS[15] = _alpha;
			_finalMatrix.setRawData(HLSAController.TMP_NUMBERS);
			_dataProvider.setProperty("diffuseColorMatrix",_finalMatrix);
		}

		protected function targetAddedHandler(ctrl		: HLSAController,
		target	: Mesh) : void
		{
			target.bindings.add(_dataProvider);
		}

		protected function targetRemovedHandler(ctrl	: HLSAController,
		target	: Mesh) : void
		{
			target.bindings.remove(_dataProvider);
		}

	}
}