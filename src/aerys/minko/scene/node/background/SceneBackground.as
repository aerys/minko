package aerys.minko.scene.node.background
{
	import aerys.minko.render.Effect;
	import aerys.minko.render.geometry.primitive.QuadGeometry;
	import aerys.minko.render.material.basic.BasicMaterial;
	import aerys.minko.render.material.basic.BasicProperties;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.background.BackgroundLayerShader;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.type.enum.FrustumCulling;
	
	public class SceneBackground extends Mesh
	{
		private var _texture	: TextureResource	= new TextureResource(2048, 2048);
		private var _material	: BasicMaterial		= new BasicMaterial();
		
		public function SceneBackground()
		{
			super (QuadGeometry.doubleSidedQuadGeometry, _material);
			
			frustumCulling = FrustumCulling.DISABLED;
			_material.effect = new Effect(new BackgroundLayerShader());
			_material.setProperty(BasicProperties.DIFFUSE_MAP, _texture);
		}
		
		public function get texture() : TextureResource
		{
			return _texture;
		}
	}
}