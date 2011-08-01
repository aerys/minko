package aerys.minko.scene.action.mesh.modifier
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.mesh.modifier.IMeshModifier;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class MeshModifierAction implements IAction
	{
		public function get type() : uint		{ return ActionType.RECURSE; }
		
		public function prefix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
		
		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			visitor.visit((scene as IMeshModifier).target);
			
			return true;
		}
		
		public function postfix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
	}
}