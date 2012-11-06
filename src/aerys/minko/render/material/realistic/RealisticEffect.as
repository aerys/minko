package aerys.minko.render.material.realistic
{
	import aerys.minko.render.material.phong.AbstractShadowMappingEffect;
	import aerys.minko.scene.node.Scene;
	
	public class RealisticEffect extends AbstractShadowMappingEffect
	{
		public function RealisticEffect(scene : Scene)
		{
			super(scene, new RealisticShader());
		}
	}
}