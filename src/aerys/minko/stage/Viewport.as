package aerys.minko.stage
{
	import aerys.common.Factory;
	import aerys.common.IVersionnable;
	import aerys.minko.Minko;
	import aerys.minko.effect.IEffect;
	import aerys.minko.effect.basic.BasicEffect;
	import aerys.minko.ns.minko;
	import aerys.minko.render.DirectRenderer;
	import aerys.minko.render.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.visitor.data.CameraData;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	import aerys.minko.scene.visitor.rendering.WorldDataExtracterVisitor;
	
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.display.Stage3D;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DRenderMode;
	import flash.events.Event;
	import flash.events.FullScreenEvent;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.utils.Dictionary;
	import flash.utils.getTimer;
	
	/**
	 * The viewport is the the display area used to render a 3D scene.
	 * It can be used to render any IScene3D object.
	 * 
	 * @author Jean-Marc Le Roux
	 *
	 */
	public class Viewport extends Sprite implements IVersionnable
	{
		use namespace minko;
		
		private static const ZERO2		: Point						= new Point();
		
		private var _version			: uint						= 0;
		
		private var _width				: Number					= 0.;
		private var _height				: Number					= 0.;
		private var _autoResize			: Boolean					= false;
		private var _antiAliasing		: int						= 0;
		
		private var _wdExtracterQuery	: WorldDataExtracterVisitor	= null;
		private var _renderingQuery		: RenderingVisitor			= null;
		
		private var _time				: int						= 0;
		private var _sceneSize			: uint						= 0;
		private var _drawTime			: int						= 0;
		
		private var _stage3d			: Stage3D					= null;
		private var _rendererClass		: Class						= null;
		private var _renderer			: IRenderer					= null;
		private var _defaultEffect		: IEffect					= new BasicEffect();
		private var _backgroundColor	: int						= 0;
		
		public function get version() : uint
		{
			return _version;
		}
		
		override public function set x(value : Number) : void
		{
			super.x = value;
			updateRectangle();
		}
		
		override public function set y(value : Number) : void
		{
			super.y = value;
			updateRectangle();
		}
		
		/**
		 * Indicates the width of the viewport.
		 * @return The width of the viewport.
		 *
		 */
		override public function get width()	: Number
		{
			return _width;
		}
		
		override public function set width(value : Number) : void
		{
			if (value != _width)
			{
				_width = value;
				++_version;
				
				resetStage3D();
			}
		}
		
		public function get sceneSize() : uint
		{
			return _sceneSize;
		}
		
		/**
		 * Indicates the height of the viewport.
		 * @return The height of the viewport.
		 *
		 */
		override public function get height() : Number
		{
			return _height;
		}
		
		override public function set height(value : Number) : void
		{
			if (value != _height)
			{
				_height = value;
				++_version;
				
				resetStage3D();
			}
		}
		
		/**
		 * The anti-aliasing value used to render the scene.
		 *  
		 * @return 
		 * 
		 */
		public function get antiAliasing() : int
		{
			return _antiAliasing;
		}
		
		public function set antiAliasing(value : int) : void
		{
			if (value != _antiAliasing)
			{
				_antiAliasing = value;
				++_version;
				
				resetStage3D();
			}
		}
		
		public function get defaultEffect() : IEffect
		{
			return _defaultEffect;
		}
		
		public function set defaultEffect(value : IEffect) : void
		{
			_defaultEffect = value;
		}
		
		/**
		 * The amount of triangle rendered durung the last call to the
		 * "render" method. Sometimes, the number of triangles is higher
		 * than the total amount of triangles in the scene because some
		 * triangles are renderer multiple times (multipass).
		 *  
		 * @return 
		 * 
		 */
		public function get numTriangles() : uint
		{
			return _renderingQuery ? _renderingQuery.numTriangles : 0;
		}
		
		/**
		 * The time spent during the last call to the "render" method.
		 * 
		 * This time includes:
		 * <ul>
		 * <li>updating the scene graph</li>
		 * <li>rendering the scene graph</li>
		 * <li>performing draw calls to the internal 3D APIs</li>
		 * </ul>
		 *  
		 * @return 
		 * 
		 */
		public function get renderingTime() : uint
		{
			return _time;
		}
		
		public function get drawingTime() : int
		{
			return _drawTime;
		}
		
		public function get renderMode() : String
		{
			return _stage3d && _stage3d.context3D ? _stage3d.context3D.driverInfo : null;
		}

		public function get backgroundColor() : int
		{
			return _backgroundColor;
		}
		
		public function set backgroundColor(value : int) : void
		{
			_backgroundColor = value;
		}
		
		/**
		 * Creates a new Viewport3D object.
		 *
		 * @param width The width of the viewport.
		 * @param height The height of the viewport.
		 */
		public function Viewport(width			: uint		= 0,
								 height			: uint		= 0,
								 antiAliasing	: int		= 0,
								 rendererType	: Class 	= null)
		{
			this.width = width;
			this.height = height;
			
			_autoResize = _width == 0 || _height == 0;
			_antiAliasing = antiAliasing;
			_rendererClass = rendererType || DirectRenderer;
			
			addEventListener(Event.ADDED_TO_STAGE, addedToStageHandler);
			addEventListener(Event.REMOVED_FROM_STAGE, removedFromStage);
			addEventListener(Event.ADDED, addedHandler);
		}
		
		private function addedToStageHandler(event : Event) : void
		{
			var stageId	: int 		= 0;
			
			_stage3d = stage.stage3Ds[stageId];
			
			while (_stage3d.willTrigger(Event.CONTEXT3D_CREATE))
				_stage3d = stage.stage3Ds[int(++stageId)];
			
			_stage3d.addEventListener(Event.CONTEXT3D_CREATE, resetStage3D);
			_stage3d.requestContext3D(Context3DRenderMode.AUTO);
			
			if (_autoResize)
			{
				stage.align = StageAlign.TOP_LEFT;
				stage.scaleMode = StageScaleMode.NO_SCALE;
				stage.addEventListener(Event.RESIZE, stageResizeHandler);
				
				width = stage.stageWidth;
				height = stage.stageHeight;
			}
			
			showLogo();
		}
		
		private function removedFromStage(event : Event) : void
		{
			_stage3d.removeEventListener(Event.CONTEXT3D_CREATE, resetStage3D);
			_stage3d.context3D.dispose();
			_stage3d = null;
		}
		
		private function addedHandler(event : Event) : void
		{
			updateRectangle();
		}
		
		private function stageResizeHandler(event : Event) : void
		{
			var stage : Stage = event.target as Stage;
			
			if (stage.stageWidth)
				width = stage.stageWidth;
			if (stage.stageHeight)
				height = stage.stageHeight;
			
			resetStage3D();
			showLogo();
		}
		
		private function resetStage3D(event : Event = null) : void
		{
			if (_stage3d && _stage3d.context3D)
			{
				updateRectangle();
				_stage3d.context3D.configureBackBuffer(_width, _height, _antiAliasing, true);
				
				_renderer = new DirectRenderer(this, _stage3d.context3D);
				
				_wdExtracterQuery = new WorldDataExtracterVisitor();
				_renderingQuery = new RenderingVisitor(_renderer);
			}
		}
		
		private function updateRectangle() : void
		{
			if (_stage3d)
			{
				var origin : Point = localToGlobal(ZERO2);
				
				_stage3d.viewPort = new Rectangle(origin.x, origin.y, _width, _height);
			}
		}
		
		/**
		 * Render the specified scene.
		 * @param scene
		 */
		public function render(scene : IScene) : void
		{
			if (_renderingQuery)
			{
				var time : Number = getTimer();
				
				_wdExtracterQuery.reset();
				_renderingQuery.reset(_defaultEffect, _backgroundColor);
				
				// recover all WorldObjets' data.
				_wdExtracterQuery.visit(scene);
				var worldData : Dictionary = _wdExtracterQuery.worldData;
				if (worldData[CameraData] != null)
				{
					CameraData(worldData[CameraData]).ratio = _width / _height
				}
				
				// render
				_renderingQuery.updateWorldData(worldData);
				_renderingQuery.visit(scene);
				
				_renderer.present();
				//_renderer.clear();
				
				_sceneSize = _renderingQuery.numNodes;
				
				Factory.sweep();
				
				_time = getTimer() - time;
				_drawTime = _renderingQuery.drawingTime;
			}
			else
			{
				_time = 0;
				_drawTime = 0;
			}

			showLogo();
		}
		
		public function showLogo() : void
		{
			var logo : Sprite = Minko.logo;
			
			addChild(logo);
			
			logo.x = 5;
			logo.y = _height - logo.height - 5;
		}
	}
}