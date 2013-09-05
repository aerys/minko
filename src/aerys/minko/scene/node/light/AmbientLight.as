package aerys.minko.scene.node.light
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.controller.light.LightController;
	import aerys.minko.scene.node.AbstractSceneNode;

	use namespace minko_scene;
	
	public class AmbientLight extends AbstractLight
	{
		public static const LIGHT_TYPE : uint = 0;
		
		public function get ambient() : Number
		{
			return lightData.getLightProperty('ambient') as Number;
		}
		public function set ambient(v : Number)	: void
		{
			lightData.setLightProperty('ambient', v);
		}
		
		public function AmbientLight(color			: uint		= 0xFFFFFFFF, 
									 ambient		: Number	= .4,
									 emissionMask	: uint		= 0x1)
		{
			super(
				new LightController(AmbientLight),
				LIGHT_TYPE,
				color,
				emissionMask
			);
			
			this.ambient = ambient;
		}
		
		override minko_scene function cloneNode() : AbstractSceneNode
		{
			var light : AmbientLight = new AmbientLight(color, ambient, emissionMask);
			
			light.name = this.name;
			light.userData.setProperties(userData);
			light.transform.copyFrom(this.transform);
			
			return light;
		}
	}
}
