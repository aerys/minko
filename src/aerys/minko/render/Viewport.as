package aerys.minko.render
{
	import flash.display.DisplayObject;
	import flash.display.DisplayObjectContainer;
	import flash.display.Graphics;
	import flash.display.Shape;
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.display.Stage3D;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.geom.Point;
	
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.type.KeyboardManager;
	import aerys.minko.type.MouseManager;
	import aerys.minko.type.Signal;

	/**
	 * The Viewport is the display area where a 3D scene can be rendered.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class Viewport extends Sprite
	{
		private static const ZERO2		: Point				= new Point();
		
		private var _stage3d			: Stage3D			= null;
		private var _context3d			: Context3DResource	= null;
		
		private var _stage3dId			: uint				= 0;
		private var _width				: uint				= 0;
		private var _height				: uint				= 0;
		private var _autoResize			: Boolean			= false;
		private var _antiAliasing		: uint				= 0;
		private var _backgroundColor	: uint				= 0;
		private var _backBuffer			: RenderTarget		= null;
		private var _invalidBackBuffer	: Boolean			= false;
		
		private var _alwaysOnTop		: Boolean			= false;
		private var _mask				: Shape				= new Shape();
		
		private var _mouseManager		: MouseManager		= new MouseManager();
		private var _keyboardManager	: KeyboardManager	= new KeyboardManager();
		
		private var _resized			: Signal			= new Signal('Viewport.resized');
		
		minko_render function get context3D() : Context3DResource
		{
			return _context3d;
		}
		
		minko_render function get backBuffer() : RenderTarget
		{
			var positionOnStage	: Point	= localToGlobal(ZERO2);
			
			if (_stage3d.x != positionOnStage.x || _stage3d.y != positionOnStage.y)
				updateStage3D()
			
			return _backBuffer;
		}
		
		public function get ready() : Boolean
		{
			return _stage3d != null && _stage3d.context3D != null && _backBuffer != null;
		}
		
		/**
		 * Whether the viewport is visible or not. 
		 * @return 
		 * 
		 */
		override public function set visible(value : Boolean) : void
		{
			if (_stage3d)
				_stage3d.visible = value;
			
			super.visible = value;
		}
		
		override public function set x(value : Number) : void
		{
			super.x = value;
			updateStage3D();
		}
		
		override public function set y(value : Number) : void
		{
			super.y = value;
			updateStage3D();
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
		}
		
		public function get keyboardManager() : KeyboardManager
		{
			return _keyboardManager;
		}
		
		public function get mouseManager() : MouseManager
		{
			return _mouseManager;
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
			updateBackBuffer();
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
			updateBackBuffer();
		}
		
		/**
		 * The driver informations provided by the Stage3D API. 
		 */
		public function get driverInfo() : String
		{
			return _stage3d && _stage3d.context3D
				? _stage3d.context3D.driverInfo
				: null;
		}
		
		public function Viewport(antiAliasing	: uint	= 0,
								 stage3dId		: uint	= 0,
								 width			: uint 	= 0,
								 height			: uint	= 0)
		{
			_stage3dId = stage3dId;
			_antiAliasing = antiAliasing;
			_autoResize = width == 0 && height == 0;
			
			_width = width;
			_height = height;
			
			initialize();
		}
		
		private function initialize() : void
		{
			_mouseManager.bind(this);
			
			addEventListener(Event.ADDED_TO_STAGE, addedToStageHandler);
			addEventListener(Event.REMOVED_FROM_STAGE, removedFromStageHandler);
		}
		
		private function addedToStageHandler(event : Event) : void
		{
			_keyboardManager.bind(stage);

			parent.addEventListener(Event.RESIZE, parentResizedHandler);
			
			stage.addEventListener(Event.ADDED_TO_STAGE, displayObjectAddedToStageHandler);
			stage.addEventListener(Event.REMOVED_FROM_STAGE, displayObjectRemovedFromStageHandler);
			stage.addEventListener(Event.RESIZE, stageResizedHandler);
			
			setupOnStage(stage);
		}
		
		private function removedFromStageHandler(event : Event) : void
		{
			_keyboardManager.unbind(stage);
			
			parent.removeEventListener(Event.RESIZE, parentResizedHandler);
			
			stage.removeEventListener(Event.ADDED_TO_STAGE, displayObjectAddedToStageHandler);
			stage.removeEventListener(Event.REMOVED_FROM_STAGE, displayObjectRemovedFromStageHandler);
			stage.removeEventListener(Event.RESIZE, stageResizedHandler);
			
			if (_stage3d != null)
				_stage3d.visible = false;
		}
		
		private function setupOnStage(stage : Stage) : void
		{
			if (_autoResize)
			{
				_width = stage.stageWidth;
				_height = stage.stageHeight;
			}
			
			if (!_stage3d)
			{
				_stage3d = stage.stage3Ds[_stage3dId];
				_stage3d.addEventListener(Event.CONTEXT3D_CREATE, context3dCreatedHandler);
				_stage3d.requestContext3D();
			}
			
			_stage3d.visible = visible;
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
		}
		
		/**
		 * Dispose the Viewport and all the Stage3D related objects. After this operation,
		 * the Viewport cannot be used anymore and is ready for garbage collection.
		 */
		public function dispose():void
		{
			if (_stage3d != null)
			{
				_stage3d.removeEventListener(Event.CONTEXT3D_CREATE, context3dCreatedHandler);
				_stage3d = null;
			}
			
			if (_context3d != null)
			{
				_context3d.dispose();
				_context3d = null;
			}
			
			return ;
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
			_autoResize = false;
			
			setSize(width, height);
		}
		
		private function setSize(width : Number, height : Number) : void
		{
			if (width == _width && _height == height)
				return ;
			
			_width = width;
			_height = height;
			
			updateStage3D();
			updateBackBuffer();
			_resized.execute(this, width, height);
		}
		
		private function stageResizedHandler(event : Event) : void
		{
			updateMask();
			updateStage3D();
			updateBackBuffer();
		}
		
		private function parentResizedHandler(event : Event) : void
		{
			if (_autoResize)
			{
				if (parent == stage)
					setSize(stage.stageWidth, stage.stageHeight);
				else
					setSize(parent.width, parent.height);
			}
		}
		
		private function context3dCreatedHandler(event : Event) : void
		{
			_context3d = new Context3DResource(_stage3d.context3D);
			
			updateStage3D();
			updateBackBuffer();
			
			dispatchEvent(new Event(Event.INIT));
		}
		
		private function updateBackBuffer() : void
		{
			if (_width == 0 || _height == 0 || _stage3d == null || _stage3d.context3D == null)
				return ;
			
			_invalidBackBuffer = false;
			
			var backBufferWidth		: Number = _width * getGlobalScaleX();
			var backBufferHeight	: Number = _height * getGlobalScaleY();
			
			_stage3d.context3D.configureBackBuffer(backBufferWidth, backBufferHeight, _antiAliasing, true);
			_backBuffer = new RenderTarget(backBufferWidth, backBufferHeight, null, 0, _backgroundColor);
			
			graphics.clear();
			graphics.beginFill(0, 0);
			graphics.drawRect(0, 0, _width, _height);
		}
		
		private function updateStage3D() : void
		{
			if (_stage3d == null)
				return ;
			
			var upperLeft	: Point	= localToGlobal(ZERO2);
			
			_stage3d.x = upperLeft.x;
			_stage3d.y = upperLeft.y;
			
			if (_width > 4096)
			{
				_stage3d.x = (_width - 4096) * 0.5;
				_width = 4096;
			}
			else
				_stage3d.x = upperLeft.x;
			
			if (_height > 4096)
			{
				_stage3d.y = (_height - 4096) * 0.5;
				_height = 4096;
			}
			else
				_stage3d.y = upperLeft.y;

			updateMask();
		}
		
		private function updateMask() : void
		{
			if (!stage)
				return ;
			
			var numChildren : uint = stage.numChildren;
			var i 			: uint = 0;
			
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
				{
					var child : DisplayObject = stage.getChildAt(i);
					
					if (child.mask == _mask)
						child.mask = null;
				}
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
		
		private function getGlobalScaleX() : Number
		{
			var currentParent	: DisplayObjectContainer	= parent;
			var scale			: Number					= scaleX; 
			
			while(currentParent != stage)
			{
				scale *= currentParent.scaleX;
				currentParent = currentParent.parent;
			}
			
			return scale;
		}
		
		private function getGlobalScaleY() : Number
		{
			var currentParent	: DisplayObjectContainer	= parent;
			var scale			: Number					= scaleY; 
			
			while(currentParent != stage)
			{
				scale *= currentParent.scaleY;
				currentParent = currentParent.parent;
			}
			
			return scale;
		}
	}
}
