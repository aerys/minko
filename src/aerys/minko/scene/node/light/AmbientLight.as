package aerys.minko.scene.node.light
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.enum.ShadowMappingType;

	use namespace minko_scene;
	
	public class AmbientLight extends AbstractLight
	{
		public static const TYPE : uint = 0;
		
		public function get ambient() : Number
		{
			return getProperty('ambient') as Number;
		}
		
		public function set ambient(v : Number)	: void
		{
			setProperty('ambient', v);
		}
		
		override public function set shadowCastingType(v : uint) : void
		{
			if (v != ShadowMappingType.NONE)
				throw new Error('An ambient light cannot emit shadows.');
			
			setProperty('shadowCastingType', ShadowMappingType.NONE);
		}
		
		public function AmbientLight(color			: uint		= 0xFFFFFFFF, 
									 ambient		: Number	= .4,
									 emissionMask	: uint		= 0x1)
		{
			super(color, emissionMask, ShadowMappingType.NONE, TYPE);
			
			this.ambient = ambient;
		}
		
		override minko_scene function cloneNode() : AbstractSceneNode
		{
			var light : AmbientLight = new AmbientLight(color, ambient, emissionMask);
			
			light.name = this.name;
			light.transform.copyFrom(this.transform);
			
			return light;
		}
	}
}
