package aerys.minko.render.material.realistic
{
	import aerys.minko.render.material.phong.AbstractShadowMappingEffect;
	
	public class RealisticEffect extends AbstractShadowMappingEffect
	{
		public function RealisticEffect()
		{
			super(new RealisticShader());
		}
	}
}