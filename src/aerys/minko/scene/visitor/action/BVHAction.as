package aerys.minko.scene.visitor.action
{
	import aerys.minko.scene.node.IScene;
	import aerys.minko.type.bounding.IBoundedVolume;

	public class BVHAction implements IVisitorAction
	{
		public function get name() : String	{ return "BVHAction"; }
		
		public function BVHAction()
		{
		}
		
		public function enter(scene : IScene) : Boolean
		{
			var bv : IBoundedVolume = scene as IBoundedVolume;
			
			if (bv)
			{
				return true;
			}
			
			return false;
		}
		
		public function visit(scene : IScene) : Boolean
		{
			return true;
		}
		
		public function leave(scene : IScene) : Boolean
		{
			return true;
		}
	}
}