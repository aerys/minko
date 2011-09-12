package aerys.minko.scene.action
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.data.CameraData;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.mesh.modifier.IMeshModifier;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.type.bounding.IBoundingVolume;
	
	public final class BoundingVolumeAction implements IAction
	{
		private static const TYPE	: uint	= ActionType.RECURSE;
		
		public function get type() : uint		{ return TYPE;	}
		
		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			if (renderer)
			{
				var bv 		: IBoundingVolume 	= scene as IBoundingVolume;
				var camData : CameraData 		= visitor.worldData[CameraData] as CameraData;
				
				if (camData && !camData.frustrum.testBoundedVolume(bv, visitor.transformData.localToView))
					return false;
				
				visitor.visit((scene as IMeshModifier).target);
			}
			
			return true;
		}
	}
}