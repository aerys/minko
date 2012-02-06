package aerys.minko.scene
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.Renderer;
	import aerys.minko.render.Viewport;
	import aerys.minko.render.shader.IShader;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.mesh.Mesh;

	public final class Visitor
	{
		use namespace minko_scene;
		
		private var _viewport		: Viewport			= null;
		private var _renderer		: Renderer			= null;
		private var _numVisitedNode	: uint				= 0;
		private var _stack			: Vector.<IScene>	= new Vector.<IScene>();
		
		public function get renderer() : Renderer
		{
			return _renderer;
		}
		
		public function get viewport() : Viewport
		{
			return _viewport;
		}
		
		public function get numVisitedNodes() : uint
		{
			return _numVisitedNode;
		}
		
		public function Visitor(viewport : Viewport,
								renderer : Renderer)
		{
			_viewport = viewport;
			_renderer = renderer;
		}
		
		public function visit(scene : IScene) : void
		{
			var stackSize : int = 1;
			
			_stack[0] = scene;
			
			while (stackSize != 0)
			{
				--stackSize;
				++_numVisitedNode;
				
				scene = _stack[stackSize];
				
				if (scene is Group)
				{
					var group : Group = scene as Group;
					var children : Vector.<IScene>	= group._children;
					var numChildren : int = children.length;
					
					for (var childrenId : int = 0; childrenId < numChildren; ++childrenId)
					{
						_stack[stackSize] = children[childrenId];
						stackSize++;
					}
				}
				else if (scene is Mesh)
				{
					var mesh		: Mesh				= scene as Mesh;
					var passes		: Vector.<IShader>	= mesh.effect.passes;
					var drawCalls	: Vector.<DrawCall>	= mesh._calls;
					var numPasses 	: int 				= passes.length;
					
					for (var i : int = 0; i < numPasses; ++i)
						renderer.pushDrawCall(passes[i].state, drawCalls[i]);
				}
			}
		}
		
		public function visitRecursive(scene : IScene) : void
		{
			if (scene is Group)
			{
				var group : Group = scene as Group;
				var children : Vector.<IScene>	= group._children;
				var numChildren : int = children.length;
				
				for (var childrenId : int = 0; childrenId < numChildren; ++childrenId)
					visitRecursive(children[childrenId]);
			}
			else if (scene is Mesh)
			{
				var mesh		: Mesh				= scene as Mesh;
				var passes		: Vector.<IShader>	= mesh.effect.passes;
				var drawCalls	: Vector.<DrawCall>	= mesh._calls;
				var numPasses 	: int 				= passes.length;
				
				for (var i : int = 0; i < numPasses; ++i)
					renderer.pushDrawCall(passes[i].state, drawCalls[i]);
			}
		}
		
		public function reset() : void
		{
			_numVisitedNode = 0;
		}
	}
}