package aerys.minko.render
{
	import aerys.common.Factory;
	import aerys.minko.ns.minko;
	import aerys.minko.render.renderer.IRenderer3D;
	import aerys.minko.render.visitor.IScene3DVisitor;
	import aerys.minko.render.visitor.Scene3DVisitor;
	import aerys.minko.scene.IScene3D;
	import aerys.minko.type.math.Frustum3D;
	import aerys.minko.type.math.Matrix4x4;
	
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
	 * The Viewport class describes the 2D space where each frame is renderer.
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public class Viewport3D extends Sprite
	{
		use namespace minko;
		
		public static const DEFAUT_FOV		: Number	= Math.PI / 4.;
		public static const DEFAUT_ZFAR		: Number	= 1000.;
		public static const DEFAUT_ZNEAR	: Number	= .1;
		
		private var _update		: Boolean			= true;
		
		private var _width		: Number			= 0;
		private var _height		: Number			= 0;
		private var _fov		: Number			= 0.;
		private var _zNear		: Number			= 0.;
		private var _zFar		: Number			= 0.;
		
		private var _rectangle	: Rectangle			= null;
		private var _frustum	: Frustum3D			= new Frustum3D();
		
		private var _visitor	: IScene3DVisitor	= null;
		private var _time		: int				= 0;
		
		private var _projection	: Matrix4x4			= new Matrix4x4();
		
		private var _renderer	: IRenderer3D		= null;
		private var _context	: Context3D			= null;
		
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
				_update = true;
				_width = value;
			}
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
				_update = true;
				_height = value;
			}
		}
		
		/**
		 *
		 * @return The disance of the near clipping plane.
		 *
		 */
		public function get zNear() : Number
		{
			return _zNear;
		}
		
		public function set zNear(value : Number) : void
		{
			if (value != _zNear)
			{
				_update = true;
				_zNear = value;
			}
		}
		
		/**
		 *
		 * @return The distance of the far clipping plane.
		 *
		 */
		public function get zFar() : Number
		{
			return _zFar;
		}
		
		public function set zFar(value : Number) : void
		{
			if (value != _zFar)
			{
				_update = true;
				_zFar = value;
			}
		}
		
		public function get fieldOfView() : Number
		{
			return _fov;
		}
		
		public function set fieldOfView(value : Number) : void
		{
			if (value != _fov)
			{
				_update = true;
				_fov = value;
			}
		}
		
		public function get numTriangles() 			: uint	{ return _visitor ? _visitor.renderer.numTriangles
																			  : 0; }
		
		public function get renderingTime() 		: uint	{ return _time; }
		
		/**
		 * The Rectangle object that bounds the viewport.
		 * @return A Rectagle object.
		 *
		 */
		minko function get rectangle() : Rectangle
		{
			if (_update)
				update();
			
			return _rectangle;
		}
		
		/**
		 * Indicates the projection transform that will project 3D view-space vertices on the 2D screen.
		 * @return A Matrix3D object that describes the projection transform.
		 *
		 */
		public function get projection() : Matrix4x4
		{
			if (_update)
				update();
			
			return _projection;
		}
		
		/**
		 * The Frustum object defined by the viewport's properties.
		 * @return A Frustum object.
		 *
		 */
		public function get frustum() : Frustum3D
		{
			if (_update)
				update();
			
			return _frustum;
		}
		
		public function get drawingTime() : int
		{
			return _visitor ? _visitor.renderer.drawingTime
							: 0;
		}
		
		public function get renderMode() : String
		{
			if (_context)
				return _context.driverInfo.split(/^(\w+) Description=(.*) Driver=.*$/gs)[1];
			
			return null;
		}
		
		public function get driver() : String
		{
			if (_context)
				return _context.driverInfo.split(/^(\w+) Description=(.*) Driver=.*$/gs)[2];
			
			return null;
		}
		
		/**
		 * Creates a new Viewport object.
		 *
		 * @param myWidth The width of the viewport.
		 * @param myHeight The height of the viewport.
		 * @param myFoV The field of view (in radians) on the Y axis.
		 * @param myZNear The near clipping plane.
		 * @param myZFar The far clipping plane.
		 *
		 */
		public function Viewport3D(width	: Number,
								   height	: Number,
								   zFar		: Number	= DEFAUT_ZFAR,
								   zNear	: Number	= DEFAUT_ZNEAR,
								   fov		: Number	= DEFAUT_FOV)
		{
			_width = width;
			_height = height;
			_fov = fov;
			_zNear = zNear;
			_zFar = zFar;
		}

		private function update() : void
		{
			var width	: Number	= _width / 2.;
			var height	: Number	= -_height / 2.;
			
			Matrix4x4.perspectiveFoVLH(_fov,
									   _width / _height,
									   _zNear,
									   _zFar,
									   _projection);
			
			_frustum.updateFromProjection(_projection, _width, _height);
			_rectangle = new Rectangle(0, 0, _width, _height);
			
			_update = false;
		}
		
		public static function setupOnStage(stage 	: Stage,
											zFar	: Number	= DEFAUT_ZFAR,
											zNear	: Number	= DEFAUT_ZNEAR,
											fov		: Number	= DEFAUT_FOV) : Viewport3D
		{
			var vp : Viewport3D = new Viewport3D(stage.stageWidth, stage.stageHeight, zFar, zNear, fov);
			
			stage.align = StageAlign.TOP_LEFT;
			stage.scaleMode = StageScaleMode.NO_SCALE;
			
			vp.setupOnStage(stage);
			
			return vp;
		}
		
		private function setupOnStage(stage : Stage, autoResize : Boolean = true) : void
		{
			stage.addChild(this);
			
			stage.stage3Ds[0].addEventListener(Event.CONTEXT3D_CREATE, resetContext3D);
			stage.stage3Ds[0].viewPort = new Rectangle(0, 0, _width, _height);
			stage.stage3Ds[0].requestContext3D(Context3DRenderMode.AUTO);

			if (autoResize)
				stage.addEventListener(Event.RESIZE, stageResizeHandler);
		}
		
		private function stageResizeHandler(event : Event) : void
		{
			var stage : Stage = event.target as Stage;
			
			if (stage.stageWidth)
				width = stage.stageWidth;
			if (stage.stageHeight)
				height = stage.stageHeight;
			
			stage.stage3Ds[0].viewPort = new Rectangle(0, 0, _width, _height);
			
			resetContext3D();
		}
		
		private function resetContext3D(event : Event = null) : void
		{
			_context = stage.stage3Ds[0].context3D;
			_context.configureBackBuffer(_width, _height, 0, true);
		}
		
		/**
		 * Render the specified scene.
		 * @param myScene
		 */
		public function render(scene 	: IScene3D,
							   sweep 	: Boolean 	= true) : void
		{
			var time : int = getTimer();
			
			if (_context)
			{
				if (!_visitor)
					_visitor = new Scene3DVisitor(_renderer);
				
				_renderer.clear(0);
				_renderer.transform.reset();
				_visitor.visit(scene);
				
				_renderer.present();
			}
			
			_time = getTimer() - time;

			if (sweep)
				Factory.sweep();			
		}		
	}
}