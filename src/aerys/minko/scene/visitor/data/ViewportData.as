package aerys.minko.scene.visitor.data
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.Viewport;
	import aerys.minko.render.effect.IEffect;
	
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
			var viewportWidth		: int = _viewport.width;
			var viewportHeight		: int = _viewport.height;
			var viewportAntialias	: int = _viewport.antiAliasing;
			var viewportBgColor		: int = _viewport.backgroundColor;
			
			if (_renderTarget == null 
				|| viewportWidth		!= _renderTarget.width
				|| viewportHeight		!= _renderTarget.height
				|| viewportAntialias	!= _renderTarget.antiAliasing
				|| viewportBgColor		!= _renderTarget.backgroundColor)
			{
				_renderTarget = new RenderTarget(
					RenderTarget.BACKBUFFER, viewportWidth, viewportHeight,
					viewportBgColor, true, viewportAntialias);
			}
			
			return _renderTarget;
		}
		
		public function ViewportData(viewport : Viewport)
		{
			_viewport = viewport;
		}
		
		public function setDataProvider(styleStack	: StyleStack, 
										localData	: LocalData,
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