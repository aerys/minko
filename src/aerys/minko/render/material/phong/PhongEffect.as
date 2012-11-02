package aerys.minko.render.material.phong
{
	import aerys.minko.render.shader.Shader;
	import aerys.minko.scene.node.Scene;
	
	public class PhongEffect extends AbstractShadowMappingEffect
	{
		public function PhongEffect(scene : Scene, shader : Shader = null)
		{
			super(scene, shader || new PhongShader());
		}
	}
}