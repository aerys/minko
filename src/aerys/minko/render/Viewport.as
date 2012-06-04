package aerys.minko.render
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.Factory;
	import aerys.minko.type.Signal;
	
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	import flash.display.Graphics;
	import flash.display.Shape;
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.display.Stage3D;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.display3D.Context3D;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.events.TouchEvent;
	import flash.geom.Point;
	import flash.utils.getTimer;

	/**
	 * The Viewport is the display area where a 3D scene can be rendered.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class Viewport extends Sprite
	{
		private static const ZERO2		: Point				= new Point();
		
		private var _stage3d			: Stage3D			= null;
		private var _context3d			: Context3DResource	= null;
		
		private var _width				: uint				= 0;
		private var _height				: uint				= 0;
		private var _autoResize			: Boolean			= false;
		private var _antiAliasing		: uint				= 0;
		private var _backgroundColor	: uint				= 0;
		private var _backBuffer			: RenderTarget		= null;
		private var _invalidBackBuffer	: Boolean			= false;
		
		private var _alwaysOnTop		: Boolean			= false;
		private var _mask				: Shape				= new Shape();
		
		private var _resized			: Signal			= new Signal('Viewport.resized');
		
		minko_render function get context3D() : Context3DResource
		{
			return _context3d;
		}
		
		minko_render function get backBuffer() : RenderTarget
		{
			var positionOnStage	: Point	= localToGlobal(ZERO2);
			
			if (_invalidBackBuffer
				|| _stage3d.x != positionOnStage.x
				|| _stage3d.y != positionOnStage.y)
			{
				updateBackBuffer();
			}
			
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
			resize(value, _height);
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
			resize(_width, value);
		}
		
		public function get alwaysOnTop() : Boolean
		{
			return _alwaysOnTop;
		}
		public function set alwaysOnTop(value : Boolean) : void
		{
			_alwaysOnTop = value;
			
			updateMask();
			updateStageListeners();
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
		
		public function Viewport(antiAliasing	: uint	= 0,
								 width			: uint 	= 0,
								 height			: uint	= 0)
		{
			_antiAliasing = antiAliasing;
			_autoResize = width == 0 && height == 0;
			
			_width = width;
			_height = height;
			
			addEventListener(Event.ADDED_TO_STAGE, addedToStageHandler);
			addEventListener(Event.REMOVED_FROM_STAGE, removedFromStageHandler);
		}
		
		private function addedToStageHandler(event : Event) : void
		{
			setupOnStage(stage);
		}
		
		private function removedFromStageHandler(event : Event) : void
		{
			if (_stage3d != null)
				_stage3d.visible = false;
		}
		
		private function setupOnStage(stage : Stage, stage3dId : uint = 0) : void
		{
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;

			if (_autoResize)
			{
				_width = stage.stageWidth;
				_height = stage.stageHeight;
				_invalidBackBuffer = true;
			}
			
			updateStageListeners();
			
			if (!_stage3d)
			{
				_stage3d = stage.stage3Ds[stage3dId];
				_stage3d.addEventListener(Event.CONTEXT3D_CREATE, context3dCreatedHandler);
				_stage3d.requestContext3D();
			}
			else
			{
				_stage3d.visible = true;
			}
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
			if (_autoResize)
			{
				_autoResize = false;
				updateStageListeners()
			}
			
			setSize(width, height);
		}
		
		private function setSize(width : Number, height : Number) : void
		{
			if (width == _width && _height == height)
				return ;
			
			_width = width;
			_height = height;
			
			_invalidBackBuffer = true;
			_resized.execute(this, width, height);
		}
		
		private function stageResizedHandler(event : Event) : void
		{
			setSize(stage.stageWidth, stage.stageHeight);
		}
		
		private function context3dCreatedHandler(event : Event) : void
		{
			_invalidBackBuffer = true;
			_context3d = new Context3DResource(_stage3d.context3D);
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
		
		private function updateStage3D() : void
		{
			var upperLeft	: Point	= localToGlobal(ZERO2);
			
			if (_width > 2048)
			{
				_stage3d.x = (_width - 2048) * 0.5;
				_width = 2048;
			}
			else
				_stage3d.x = upperLeft.x;
			
			if (_height > 2048)
			{
				_stage3d.y = (_height - 2048) * 0.5;
				_height = 2048;
			}
			else
				_stage3d.y = upperLeft.y;

			_invalidBackBuffer = true;
			updateMask();
		}
		
		private function updateStageListeners() : void
		{
			if (!stage)
				return ;
			
			if (_autoResize)
				stage.addEventListener(Event.RESIZE, stageResizedHandler);
			else
				stage.removeEventListener(Event.RESIZE, stageResizedHandler);
			
			if (_alwaysOnTop)
			{
				stage.addEventListener(Event.RESIZE, stageResizeHandler);
				stage.addEventListener(Event.ADDED_TO_STAGE, displayObjectAddedToStageHandler);
				stage.addEventListener(Event.REMOVED_FROM_STAGE, displayObjectRemovedFromStageHandler);
				stage.addEventListener(MouseEvent.CLICK, stageEventHandler);
				stage.addEventListener(MouseEvent.DOUBLE_CLICK, stageEventHandler);
				stage.addEventListener(MouseEvent.MOUSE_DOWN, stageEventHandler);
				stage.addEventListener(MouseEvent.MOUSE_MOVE, stageEventHandler);
				stage.addEventListener(MouseEvent.MOUSE_OUT, stageEventHandler);
				stage.addEventListener(MouseEvent.MOUSE_OVER, stageEventHandler);
				stage.addEventListener(MouseEvent.MOUSE_WHEEL, stageEventHandler);
				stage.addEventListener(MouseEvent.ROLL_OUT, stageEventHandler);
				stage.addEventListener(MouseEvent.ROLL_OVER, stageEventHandler);
				stage.addEventListener(TouchEvent.TOUCH_BEGIN, stageEventHandler);
				stage.addEventListener(TouchEvent.TOUCH_END, stageEventHandler);
				stage.addEventListener(TouchEvent.TOUCH_MOVE, stageEventHandler);
			}
			else
			{
				stage.removeEventListener(Event.RESIZE, stageResizeHandler);
				stage.removeEventListener(Event.ADDED_TO_STAGE, displayObjectAddedToStageHandler);
				stage.removeEventListener(Event.REMOVED_FROM_STAGE, displayObjectRemovedFromStageHandler);
				stage.removeEventListener(MouseEvent.CLICK, stageEventHandler);
				stage.removeEventListener(MouseEvent.DOUBLE_CLICK, stageEventHandler);
				stage.removeEventListener(MouseEvent.MOUSE_DOWN, stageEventHandler);
				stage.removeEventListener(MouseEvent.MOUSE_MOVE, stageEventHandler);
				stage.removeEventListener(MouseEvent.MOUSE_OUT, stageEventHandler);
				stage.removeEventListener(MouseEvent.MOUSE_OVER, stageEventHandler);
				stage.removeEventListener(MouseEvent.MOUSE_WHEEL, stageEventHandler);
				stage.removeEventListener(MouseEvent.ROLL_OUT, stageEventHandler);
				stage.removeEventListener(MouseEvent.ROLL_OVER, stageEventHandler);
				stage.removeEventListener(TouchEvent.TOUCH_BEGIN, stageEventHandler);
				stage.removeEventListener(TouchEvent.TOUCH_END, stageEventHandler);
				stage.removeEventListener(TouchEvent.TOUCH_MOVE, stageEventHandler);
			}
		}
		
		private function stageResizeHandler(event : Event) : void
		{
			updateStage3D();
		}
		
		private function stageEventHandler(event : Object) : void
		{
			if (!_alwaysOnTop || event.target == this)
				return ;
			
			var stageX	: Number	= event.stageX;
			var stageY	: Number	= event.stageY;
			
			if (stageX > _stage3d.x && stageX < _stage3d.x + _width
				&& stageY > _stage3d.y && stageY < _stage3d.y + _height)
			{
				dispatchEvent(event.clone());
			}
		}
		
		private function updateMask() : void
		{
			if (!stage)
				return ;
			
			var numChildren : int = stage.numChildren;
			var i 			: int = 0;
			
			if (_alwaysOnTop)
			{
				var gfx			: Graphics	= _mask.graphics;
				var stageWidth	: int		= stage.stageWidth;
				var stageHeight	: int		= stage.stageHeight;
				
				gfx.clear();
				gfx.beginFill(0);
				gfx.moveTo(0, 0);
				gfx.lineTo(stageWidth, 0);
				gfx.lineTo(stageWidth, stageHeight);
				gfx.lineTo(0., stageHeight);
				gfx.lineTo(0, 0);
				gfx.moveTo(_stage3d.x, _stage3d.y);
				gfx.lineTo(_stage3d.x, _stage3d.y + height);
				gfx.lineTo(_stage3d.x + width, _stage3d.y + height);
				gfx.lineTo(_stage3d.x + width, _stage3d.y);
				gfx.lineTo(_stage3d.x, _stage3d.y);
				gfx.endFill();
				
				for (i = 0; i < numChildren; ++i)
					stage.getChildAt(i).mask = _mask;
			}
			else
			{
				for (i = 0; i < numChildren; ++i)
					stage.getChildAt(i).mask = null;
			}
		}
		
		private function displayObjectAddedToStageHandler(event : Event) : void
		{
			var displayObject : DisplayObject	= event.target as DisplayObject;
			
			if (displayObject.parent == stage)
				updateMask();
		}
		
		private function displayObjectRemovedFromStageHandler(event : Event) : void
		{
			var displayObject : DisplayObject	= event.target as DisplayObject;
			
			if (_autoResize && displayObject.parent == stage)
				displayObject.mask = null;
		}
	}
}