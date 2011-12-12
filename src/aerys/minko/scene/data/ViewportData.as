package aerys.minko.scene.data
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.Viewport;
	import aerys.minko.render.effect.IEffect;
	
	import flash.display.Stage;
	import flash.utils.Dictionary;

	public class ViewportData implements IWorldData
	{
		public static const RATIO 				: String = 'ratio';
		public static const WIDTH				: String = 'width';
		public static const HEIGHT				: String = 'height';
		public static const ANTIALIASING		: String = 'antialiasing';
		public static const BACKGROUND_COLOR	: String = 'backgroundColor';

		protected var _viewport					: Viewport;
		protected var _renderTarget				: RenderTarget;
		protected var _backbufferRenderTarget	: RenderTarget;

		public function get frameId() : uint
		{
			return _viewport.frameId;
		}

		public function get ratio() : Number
		{
			return _viewport.width / _viewport.height;
		}

		public function get width() : int
		{
			return _viewport.width;
		}

		public function get height() : int
		{
			return _viewport.height;
		}

		public function get antiAliasing() : int
		{
			return _viewport.antiAliasing;
		}

		public function get backgroundColor() : uint
		{
			return _viewport.backgroundColor;
		}

		public function get defaultEffect() : IEffect
		{
			return _viewport.defaultEffect;
		}

		public function get renderTarget() : RenderTarget
		{
			var viewportHasPostProcess : Boolean = _viewport.postProcessingEffect != null;
			
			if (!viewportHasPostProcess)
				return backBufferRenderTarget;

			var viewportWidth			: int	= _viewport.width;
			var viewportHeight			: int	= _viewport.height;
			var viewportAntialias		: int	= _viewport.antiAliasing;
			var viewportBgColor			: int	= _viewport.backgroundColor;
			var viewportSize			: int	= Math.max(viewportWidth, viewportHeight);
			var bitshift				: int	= Math.min(11, Math.ceil(Math.log(viewportSize) * Math.LOG2E));
			var size					: uint	= 1 << (bitshift - (viewportSize & 0x1 != 0 && _viewport.downScaleRenderTarget ? 1 : 0));
			
			if (_renderTarget 			== null
				|| size					!= _renderTarget.width
				|| size					!= _renderTarget.height
				|| viewportAntialias	!= _renderTarget.antiAliasing
				|| viewportBgColor		!= _renderTarget.backgroundColor)
			{
				_renderTarget = new RenderTarget(RenderTarget.TEXTURE, size, size, viewportBgColor, true, viewportAntialias);
			}

			return _renderTarget;
		}

		public function get backBufferRenderTarget() : RenderTarget
		{
			var viewportWidth			: int		= _viewport.width;
			var viewportHeight			: int		= _viewport.height;
			var viewportAntialias		: int		= _viewport.antiAliasing;
			var viewportBgColor			: int		= _viewport.backgroundColor;

			if (_backbufferRenderTarget == null
				|| viewportWidth		!= _backbufferRenderTarget.width
				|| viewportHeight		!= _backbufferRenderTarget.height
				|| viewportAntialias	!= _backbufferRenderTarget.antiAliasing
				|| viewportBgColor		!= _backbufferRenderTarget.backgroundColor)
			{
				_backbufferRenderTarget = new RenderTarget(
					RenderTarget.BACKBUFFER,
					viewportWidth, viewportHeight,
					viewportBgColor, true, viewportAntialias);
			}

			return _backbufferRenderTarget;
		}

		public function get stage() : Stage
		{
			return _viewport.stage;
		}

		public function get viewport() : Viewport
		{
			return _viewport;
		}

		public function ViewportData(viewport : Viewport)
		{
			_viewport = viewport;
		}

		public function setDataProvider(styleStack	: StyleData,
										transformData	: TransformData,
										worldData	: Dictionary) : void
		{
		}

		public function invalidate() : void
		{
		}

		public function reset() : void
		{
		}
	}
}