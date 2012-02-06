package aerys.minko.render
{
	import aerys.minko.scene.Visitor;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.type.Signal;
	
	import flash.display.Stage;
	import flash.display.Stage3D;
	import flash.display3D.Context3D;
	import flash.events.Event;
	import flash.utils.getTimer;

	public final class Viewport
	{
		private var _stage3d		: Stage3D	= null;
		
		private var _renderer		: Renderer	= null;
		private var _visitor		: Visitor	= null;
		
		private var _width			: uint		= 0;
		private var _height			: uint		= 0;
		private var _antiAliasing	: uint		= 0;
		private var _autoResize		: Boolean	= false;
		
		private var _changed		: Signal	= new Signal();
		
		private var _renderingTime	: int		= 0;
		private var _drawingTime	: int		= 0;
		
		public function get width() : uint
		{
			return _width;
		}
		public function set width(value : uint) : void
		{
			_width = value;
			_changed.execute(this, "width");
		}
		
		public function get height() : uint
		{
			return _height;
		}
		public function set height(value : uint) : void
		{
			_height = value;
			_changed.execute(this, "height");
		}
		
		public function get sceneSize() : uint
		{
			return _visitor ? _visitor.numVisitedNodes : 0;
		}
		
		public function get numTriangles() : uint
		{
			return _renderer ? _renderer.numTriangles : 0;
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function get renderingTime() : int
		{
			return _renderingTime;
		}
		
		public function get drawingTime() : int
		{
			return _drawingTime;
		}
		
		public function Viewport(stage	 		: Stage,
								 antiAliasing	: uint	= 0,
								 width			: uint 	= 0,
								 height			: uint	= 0)
		{
			this.width = width;
			this.height = height;
			
			initialize(stage);
		}
		
		private function initialize(stage : Stage) : void
		{
			_autoResize = _width == 0 && _height == 0;

			_stage3d = stage.stage3Ds[0];
			_stage3d.addEventListener(Event.CONTEXT3D_CREATE, context3dCreatedHandler);
			_stage3d.requestContext3D();
		}
		
		private function context3dCreatedHandler(event : Event) : void
		{
			_renderer = new Renderer(_stage3d.context3D);
			_visitor = new Visitor(this, _renderer);
			
			updateRectangle();
		}
		
		private function updateRectangle() : void
		{
			if (_renderer)
			{
				_renderer.configureBackBuffer(
					_width,
					_height,
					_antiAliasing,
					true
				);
			}
		}
		
		public function render(scene : IScene) : void
		{
			if (_visitor)
			{
				var time : int = getTimer();
				
				_renderer.clear();
				
				_visitor.reset();
				_visitor.visit(scene);
//				_visitor.visitRecursive(scene);
				
				_renderer.render();
				_renderingTime = getTimer() - time;
				
				time = getTimer();
				_renderer.present();
				_drawingTime = getTimer() - time;
			}
		}
	}
}