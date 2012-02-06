package aerys.minko.render
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.Signal;
	
	import flash.display.Stage;
	import flash.display.Stage3D;
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
		
		private var _visitingTime	: int			= 0;
		private var _renderingTime	: int			= 0;
		private var _numTriangles	: uint			= 0;
		private var _sceneSize		: uint			= 0;
		
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
			return _sceneSize;
		}
		
		public function get numTriangles() : uint
		{
			return _numTriangles;
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function get visitingTime() : int
		{
			return _visitingTime;
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

			_stage3d = stage.stage3Ds[0];
			_stage3d.addEventListener(Event.CONTEXT3D_CREATE, context3dCreatedHandler);
			_stage3d.requestContext3D();
		}
		
		private function context3dCreatedHandler(event : Event) : void
		{
			updateRectangle();
		}
		
		private function updateRectangle() : void
		{
			if (_stage3d && _stage3d.context3D)
			{
				_stage3d.context3D.configureBackBuffer(
					_width,
					_height,
					_antiAliasing,
					true
				);
			}
		}
		
		public function render(scene : IScene, list : RenderingList = null) : void
		{
			_numTriangles = 0;
			list ||= _renderingList;
			list.clear();
			
			var context : Context3D = _stage3d.context3D;
			var time 	: int 		= getTimer();
			
			// visit
			_sceneSize = 0;
			visitRecursive(scene, list);
			_visitingTime = getTimer() - time;
			
			// render
			renderList(list);
		}
		
		public function renderList(list : RenderingList) : void
		{
			var context : Context3D = _stage3d.context3D;
			
			if (context)
			{
				var time	: int	= getTimer();
				
				_numTriangles = _renderingList.render(context);
				context.present();
				_renderingTime = getTimer() - time;
			}
		}
		
		private function visitRecursive(scene : IScene, list : RenderingList) : void
		{
			++_sceneSize;
			
			if (scene is Group)
			{
				var group 		: Group 			= scene as Group;
				var children 	: Vector.<IScene>	= group._children;
				var numChildren : int 				= children.length;
				
				for (var childrenId : int = 0; childrenId < numChildren; ++childrenId)
					visitRecursive(children[childrenId], list);
			}
			else if (scene is Mesh)
			{
				var mesh		: Mesh							= scene as Mesh;
				var passes		: Vector.<ActionScriptShader>	= mesh.effect.passes;
				var drawCalls	: Vector.<DrawCall>				= mesh._calls;
				var numPasses 	: int 							= passes.length;
				
				for (var i : int = 0; i < numPasses; ++i)
					list.pushDrawCall(passes[i].state, drawCalls[i]);
			}
		}
	}
}