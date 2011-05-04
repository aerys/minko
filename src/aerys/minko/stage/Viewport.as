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
	import aerys.minko.scene.graph.IScene;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DRenderMode;
	import flash.events.Event;
	import flash.geom.Rectangle;
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
		
		private var _width			: Number			= 0.;
		private var _height			: Number			= 0.;
		
		private var _version		: uint				= 0;
		
		private var _query			: RenderingVisitor	= null;
		private var _time			: int				= 0;
		private var _sceneSize		: uint				= 0;
		private var _drawTime		: int				= 0;
		
		private var _rendererClass	: Class				= null;
		private var _renderer		: IRenderer			= null;
		private var _context		: Context3D			= null;
		
		private var _aa				: int				= 0;
		
		private var _defaultEffect	: IEffect			= new BasicEffect();
		
		public function get version() : uint
		{
			return _version;
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
				
				resetContext3D();
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
				
				resetContext3D();
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
			return _aa;
		}
		
		public function set antiAliasing(value : int) : void
		{
			if (value != _aa)
			{
				_aa = value;
				++_version;
				
				resetContext3D();
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
			return _query ? _query.numTriangles : 0;
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
			return _context ? _context.driverInfo : null;
		}

		/**
		 * Creates a new Viewport3D object.
		 *
		 * @param width The width of the viewport.
		 * @param height The height of the viewport.
		 */
		public function Viewport(width			: Number,
								   height		: Number,
								   antiAliasing	: int	= 0,
								   rendererType	: Class = null)
		{
			this.width = width;
			this.height = height;
			
			_aa = antiAliasing;
			_rendererClass = rendererType || DirectRenderer;
		}
		
		/**
		 * Create a new Viewport3D object and setup the stage accordingly.
		 * 
		 * <ul>
		 * <li>The resulting Viewport3D object will have the dimensions of the stage.</li>
		 * <li>The resulting Viewport3D object listens to the Event.RESIZE event of the stage
		 * in order to update its dimensions.</li>
		 * <li>The stage "scaleMode" property is set to StageScaleMode.NO_SCALE.</li>
		 * <li>The stage "align" property is set to StageAlign.TOP_LEFT.</li>
		 * </ul>
		 *  
		 * @param stage
		 * @param antiAliasing
		 * @return 
		 * 
		 */
		public static function setupOnStage(stage : Stage, antiAliasing : int = 0) : Viewport
		{
			var vp : Viewport = new Viewport(stage.stageWidth,
											 stage.stageHeight,
											 antiAliasing);
			
			vp.setupOnStage(stage);
			
			return vp;
		}
		
		private function setupOnStage(stage : Stage, autoResize : Boolean = true) : void
		{
			stage.addChild(this);
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			stage.stage3Ds[0].addEventListener(Event.CONTEXT3D_CREATE, contextCreatedHandler);
			stage.stage3Ds[0].requestContext3D(Context3DRenderMode.AUTO);
			
			if (autoResize)
				stage.addEventListener(Event.RESIZE, stageResizeHandler);
			
			width = stage.stageWidth;
			height = stage.stageHeight;
			
			showLogo();
		}
		
		private function stageResizeHandler(event : Event) : void
		{
			var stage : Stage = event.target as Stage;
			
			if (stage.stageWidth)
				width = stage.stageWidth;
			if (stage.stageHeight)
				height = stage.stageHeight;
			
			resetContext3D();
		}
		
		private function contextCreatedHandler(event : Event) : void
		{
			_context = stage.stage3Ds[0].context3D;
			resetContext3D();
		}
		
		private function resetContext3D(event : Event = null) : void
		{
			if (_context)
			{
				stage.stage3Ds[0].viewPort = new Rectangle(x, y, _width, _height);
				_context.configureBackBuffer(_width, _height, _aa, true);
				
				_renderer = new DirectRenderer(this, _context);
				_query = new RenderingVisitor(_renderer);
			}
		}
		
		/**
		 * Render the specified scene.
		 * @param scene
		 */
		public function render(scene : IScene) : void
		{
			if (_query)
			{
				var time : Number = getTimer();

				_query.reset(_defaultEffect);
				_query.query(scene);
				_renderer.present();
			
				_sceneSize = _query.numNodes;
				
				Factory.sweep();
				
				_time = getTimer() - time;
				_drawTime = _query.drawingTime;
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
			logo.y = stage.stageHeight - logo.height - 5;
		}
	}
}