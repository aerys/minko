package aerys.minko.render
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.Factory;
	import aerys.minko.type.Signal;
	
	import flash.display.BitmapData;
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.display.Stage3D;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.display3D.Context3D;
	import flash.events.Event;
	import flash.utils.getTimer;

	/**
	 * The Viewport is the display area where a 3D scene can be rendered.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class Viewport extends Sprite
	{
		private var _stage3d			: Stage3D		= null;
		
		private var _width				: uint			= 0;
		private var _height				: uint			= 0;
		private var _autoResize			: Boolean		= false;
		private var _antiAliasing		: uint			= 0;
		private var _backgroundColor	: uint			= 0;
		private var _backBuffer			: RenderTarget	= null;
		private var _invalidBackBuffer	: Boolean		= false;
		
		private var _renderingTime		: int			= 0;
		
		private var _resized			: Signal		= new Signal('Viewport.resized');
		private var _enterFrame			: Signal		= new Signal('Viewport.enterFrame');
		private var _exitFrame			: Signal		= new Signal('Viewport.exitFrame');
		
		minko_render function get context3D() : Context3D
		{
			return _stage3d ? _stage3d.context3D : null;
		}
		
		minko_render function get backBuffer() : RenderTarget
		{
			if (_invalidBackBuffer)
				updateBackBuffer();
			
			return _backBuffer;
		}
		
		public function get ready() : Boolean
		{
			return _stage3d && _stage3d.context3D;
		}
		
		/**
		 * Whether the viewport is visible or not. 
		 * @return 
		 * 
		 */
		override public function get visible() : Boolean
		{
			return _stage3d.visible;
		}
		override public function set visible(v : Boolean) : void
		{
			_stage3d.visible = v;
			super.visible = v;
		}
		
		/**
		 * The width of the display area. 
		 * @return 
		 * 
		 */
		override public function get width() : Number
		{
			return _width;
		}
		override public function set width(value : Number) : void
		{
			_width = value;
			_invalidBackBuffer = true;
		}
		
		/**
		 * The height of the display area. 
		 * @return 
		 * 
		 */
		override public function get height() : Number
		{
			return _height;
		}
		override public function set height(value : Number) : void
		{
			_height = value;
			_invalidBackBuffer = true;
		}
		
		/**
		 * The signal executed when the viewport is resized.
		 * Callback functions for this signal should accept the following
		 * arguments:
		 * <ul>
		 * <li>viewport : Viewport, the viewport executing the signal</li>
		 * <li>width : Number, the new width of the viewport</li>
		 * <li>height : Number, the new height of the viewport</li>
		 * </ul> 
		 * @return 
		 * 
		 */
		public function get resized() : Signal
		{
			return _resized;
		}
		
		/**
		 * The time spent (in milliseconds) calling methods from the Stage3D
		 * rendering API. 
		 * @return 
		 * 
		 */
		public function get renderingTime() : int
		{
			return _renderingTime;
		}
		
		/**
		 * The background color of the display area. This value must use the
		 * RGB format.
		 * @return 
		 * 
		 */
		public function get backgroundColor() : uint
		{
			return _backgroundColor;
		}
		public function set backgroundColor(value : uint) : void
		{
			_backgroundColor = value;
		}
		
		/**
		 * The anti-aliasing to use (0, 2, 4, 8 or 16). The actual anti-aliasing
		 * used for rendering depends on the hardware capabilities. If the specified
		 * anti-aliasing value is not supported, the value 0 will be used.
		 * @return 
		 * 
		 */
		public function get antiAliasing() : uint
		{
			return _antiAliasing;
		}
		public function set antiAliasing(value : uint) : void
		{
			_antiAliasing = value;
			_invalidBackBuffer = true;
		}
		
		/**
		 * The driver informations provided by the Stage3D API. 
		 * @return 
		 * 
		 */
		public function get driverInfo() : String
		{
			return _stage3d && _stage3d.context3D
				? _stage3d.context3D.driverInfo
				: null;
		}
		
		public function Viewport(stage	 		: Stage,
								 antiAliasing	: uint	= 0,
								 width			: uint 	= 0,
								 height			: uint	= 0)
		{
			_antiAliasing = antiAliasing;
			
			initialize(stage, width, height);
		}
		
		private function initialize(stage 	: Stage,
									width	: uint,
									height	: uint) : void
		{
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			
			_autoResize = width == 0 && height == 0;
			if (_autoResize)
			{
				stage.addEventListener(Event.RESIZE, stageResizedHandler);
				
				width = stage.stageWidth;
				height = stage.stageHeight;
			}
			
			_width = width;
			_height = height;
			
			_stage3d = stage.stage3Ds[0];
			_stage3d.addEventListener(Event.CONTEXT3D_CREATE, context3dCreatedHandler);
			_stage3d.requestContext3D();
		}
		
		/**
		 * Resize the display area. The "resized" signal is executed when the new width and
		 * height have be set. 
		 * @param width
		 * @param height
		 * 
		 */
		public function resize(width : Number, height : Number) : void
		{
			if (!width || !height)
				return;
			
			_width = width;
			_height = height;
			_invalidBackBuffer = true;
			_resized.execute(this, width, height);
		}
		
		private function stageResizedHandler(event : Event) : void
		{
			var stage : Stage = event.target as Stage;
			
			resize(stage.stageWidth, stage.stageHeight);
		}
		
		private function context3dCreatedHandler(event : Event) : void
		{
			_invalidBackBuffer = true;
			dispatchEvent(new Event(Event.INIT));
		}
		
		private function updateBackBuffer() : void
		{
			_invalidBackBuffer = false;
			_stage3d.context3D.configureBackBuffer(
				_width,
				_height,
				_antiAliasing,
				true
			);
			_backBuffer = new RenderTarget(
				_width,
				_height,
				null,
				0,
				_backgroundColor
			);
		}
	}
}