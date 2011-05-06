package aerys.minko.scene.visitor
{
	import aerys.minko.scene.graph.ICollectable;
	import aerys.minko.scene.graph.ITransformable;
	import aerys.minko.scene.graph.IScene;
	import aerys.minko.scene.visitor.action.IVisitorAction;
	import aerys.minko.type.math.Matrix4x4;

	public class Visitor implements ISceneVisitor
	{
		private var _world		: Matrix4x4					= new Matrix4x4();
		
		private var _frameData	: Object					= null;
		private var _transforms	: Vector.<Matrix4x4>		= new Vector.<Matrix4x4>();
		
		private var _collected	: Object					= new Object();
		
		private var _actions	: Vector.<IVisitorAction>	= new Vector.<IVisitorAction>();
		
		public function get actions() : Vector.<IVisitorAction>		{ return _actions; }
		
		public function get worldTransform() : Matrix4x4
		{
			var numTransforms 	: int = _transforms.length;
			
			// FIXME: make it "lazy"
			_world.identity();
			for (var i : int = 0; i < numTransforms; ++i)
				_world.multiply(_transforms[i]);
			
			return _world;
		}
		
		public function Visitor()
		{
		}
		
		public function visit(scene : IScene) : void
		{
			if (scene is ITransformable)
				queryIObject3D(scene as ITransformable);
			else if (scene is ICollectable)
				queryICollectable(scene as ICollectable);
			else			
				scene.visited(this);
		}
		
		private function queryIObject3D(scene : ITransformable) : void
		{
			_transforms.push(scene.transform);
			scene.visited(this);
			_transforms.pop();
		}

		private function queryICollectable(scene : ICollectable) : void
		{
			_collected[scene.name] = scene.collected(this);
		}
	}
}