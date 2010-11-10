package aerys.minko.render
{
	import aerys.common.Factory;
	import aerys.minko.ns.minko;
	import aerys.minko.scene.IScene3D;
	import aerys.minko.type.math.Frustum;
	import aerys.minko.type.math.Transform3D;
	
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DRenderMode;
	import flash.display3D.Context3DTriangleFace;
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
		private var _frustum	: Frustum			= new Frustum();
		
		private var _visitor	: IScene3DVisitor	= null;
		private var _time		: int				= 0;
		
		private var _projection	: Transform3D		= new Transform3D();
		
		private var _context	: Context3D			= new Context3D(Context3DRenderMode.SOFTWARE);
		
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
		
		/*public function get numTriangles() 			: uint	{ return _graphics.numTriangles; }
		public function get numDrawingOperations() 	: uint	{ return _graphics.numDrawingOperations; }*/
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
		public function get projection() : Transform3D
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
		public function get frustum() : Frustum
		{
			if (_update)
				update();
			
			return _frustum;
		}
		
		public function get context() : Context3D
		{
			return _context;
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
		public function Viewport3D(myWidth	: Number,
								   myHeight	: Number,
								   myZFar	: Number	= DEFAUT_ZFAR,
								   myZNear	: Number	= DEFAUT_ZNEAR,
								   myFoV	: Number	= DEFAUT_FOV)
		{
			_width = myWidth;
			_height = myHeight;
			_fov = myFoV;
			_zNear = myZNear;
			_zFar = myZFar;
			
			_visitor = new Scene3DVisitor(this);
			
			_context.setupBackBuffer(_width, _height, 0, true);
			
			_context.setCulling(Context3DTriangleFace.FRONT);
		}

		private function update() : void
		{
			var width	: Number	= _width / 2.;
			var height	: Number	= -_height / 2.;
			
			_projection = Transform3D.perspectiveFovLH(_fov, _width / _height, _zNear, _zFar);
			_frustum.updateFromProjection(_projection);
			//_projection.appendScale(width, height, 1.0);
			
			_rectangle = new Rectangle(0, 0, _width, _height);
			
			_update = false;
		}
		
		public static function setupOnStage(myStage 		: Stage,
											myZFar			: Number	= DEFAUT_ZFAR,
											myZNear			: Number	= DEFAUT_ZNEAR,
											myFoV			: Number	= DEFAUT_FOV) : Viewport3D
		{
			var vp : Viewport3D = new Viewport3D(myStage.stageWidth, myStage.stageHeight, myZFar, myZNear, myFoV);
			
			myStage.align = StageAlign.TOP_LEFT;
			myStage.scaleMode = StageScaleMode.NO_SCALE;
			
			vp.setupOnStage(myStage);
			
			return vp;
		}
		
		private function setupOnStage(myStage : Stage, myAutoResize : Boolean = true) : void
		{
			myStage.stage3Ds[0].attachContext3D(_context);
			myStage.stage3Ds[0].viewPort = new Rectangle(0, 0, _width, _height);
			
			if (myAutoResize)
				myStage.addEventListener(Event.RESIZE, stageResizeHandler);
		}
		
		private function stageResizeHandler(event : Event) : void
		{
			var stage : Stage = event.target as Stage;
			
			if (stage.stageWidth)
				width = stage.stageWidth;
			if (stage.stageHeight)
				height = stage.stageHeight;
			
			_context.setupBackBuffer(_width, _height, 0, true);
			stage.stage3Ds[0].viewPort = new Rectangle(0, 0, _width, _height);
		}
		
		/**
		 * Render the specified scene.
		 * @param myScene
		 */
		public function render(myScene 		: IScene3D,
							   myClearColor	: uint		= 0xff000000,
							   mySweep 		: Boolean 	= true) : void
		{
			var time 		: int 			= getTimer();
			var renderer 	: IRenderer3D 	= _visitor.renderer;

			renderer.clear(myClearColor);
			renderer.transform.reset();
			renderer.transform.projection = _projection;
			_visitor.visit(myScene);
			
			_time = getTimer() - time;

			if (mySweep)
				Factory.sweep();
			
			renderer.present();
		}		
	}
}