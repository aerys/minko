package aerys.minko.render
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.scene.Group;
	import aerys.minko.scene.ISceneNode;
	import aerys.minko.scene.Scene;
	import aerys.minko.scene.mesh.Mesh;
	import aerys.minko.type.Factory;
	import aerys.minko.type.Signal;
	
	import flash.display.Stage;
	import flash.display.Stage3D;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.display3D.Context3D;
	import flash.events.Event;
	import flash.utils.getTimer;

	public final class Viewport
	{
		use namespace minko_scene;
		
		private var _stage3d		: Stage3D		= null;
		
		private var _renderingList	: RenderingList = new RenderingList();
		
		private var _width			: uint			= 0;
		private var _height			: uint			= 0;
		private var _antiAliasing	: uint			= 0;
		private var _autoResize		: Boolean		= false;
		
		private var _changed		: Signal		= new Signal();
		
		private var _renderingTime	: int			= 0;
		private var _numTriangles	: uint			= 0;
		
		private var _updateRect		: Boolean		= false;
		
		public function get width() : uint
		{
			return _width;
		}
		public function set width(value : uint) : void
		{
			_width = value;
			_updateRect = true;
			_changed.execute(this, "width");
		}
		
		public function get height() : uint
		{
			return _height;
		}
		public function set height(value : uint) : void
		{
			_height = value;
			_updateRect = true;
			_changed.execute(this, "height");
		}
		
		public function get numTriangles() : uint
		{
			return _numTriangles;
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function get renderingTime() : int
		{
			return _renderingTime;
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
			if (_autoResize)
			{
				stage.scaleMode = StageScaleMode.NO_SCALE;
				stage.align = StageAlign.TOP_LEFT;
				stage.addEventListener(Event.RESIZE, stageResizedHandler);
				
				width = stage.stageWidth;
				height = stage.stageHeight;
			}

			_stage3d = stage.stage3Ds[0];
			_stage3d.addEventListener(Event.CONTEXT3D_CREATE, context3dCreatedHandler);
			_stage3d.requestContext3D();
		}
		
		private function stageResizedHandler(event : Event) : void
		{
			var stage : Stage = event.target as Stage;
			
			width = stage.stageWidth;
			height = stage.stageHeight;
		}
		
		private function context3dCreatedHandler(event : Event) : void
		{
			_updateRect = true;
		}
		
		private function updateRectangle() : void
		{
			_stage3d.context3D.configureBackBuffer(
				_width,
				_height,
				_antiAliasing,
				true
			);
		}
		
		public function render(scene : Scene) : void
		{
			var context : Context3D 	= _stage3d.context3D;
			var list	: RenderingList	= scene.renderingList;
			
			_numTriangles = 0;
			_renderingTime = 0;
			
			if (context)
			{
				var time	: int	= getTimer();
				
				if (_updateRect)
					updateRectangle();
				
				_numTriangles = list.render(context);
				context.present();
				_renderingTime = getTimer() - time;
			}
			
			Factory.sweep();
		}
	}
}