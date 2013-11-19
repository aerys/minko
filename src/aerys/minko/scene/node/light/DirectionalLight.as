package aerys.minko.scene.node.light
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.scene.controller.light.DirectionalLightController;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.type.enum.ShadowMappingType;
	import aerys.minko.type.math.Vector4;

	use namespace minko_scene;
	
	/**
	 * 
	 * @author Romain Gilliotte
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class DirectionalLight extends AbstractLight
	{
		public static const LIGHT_TYPE : uint = 1;
		
		public function get diffuse() : Number
		{
			return lightData.getLightProperty('diffuse') as Number;
		}
		public function set diffuse(v : Number)	: void
		{
			lightData.setLightProperty('diffuse', v);
			
			if (lightData.getLightProperty('diffuseEnabled') != (v != 0))
				lightData.setLightProperty('diffuseEnabled', v != 0);
		}
		
		public function get specular() : Number
		{
			return lightData.getLightProperty('specular') as Number;
		}
		public function set specular(v : Number) : void
		{
			lightData.setLightProperty('specular', v);
			
			if (lightData.getLightProperty('specularEnabled') != (v != 0))
				lightData.setLightProperty('specularEnabled', v != 0);
		}
		
		public function get shininess() : Number
		{
			return lightData.getLightProperty('shininess') as Number;
		}
		public function set shininess(value : Number) : void
		{
			lightData.setLightProperty('shininess', value);
		}
		
		public function get shadowWidth() : Number
		{
			return lightData.getLightProperty('shadowWidth');
		}
		public function set shadowWidth(v : Number) : void
		{
			lightData.setLightProperty('shadowWidth', v);
		}
		
		public function get shadowZFar() : Number
		{
			return lightData.getLightProperty('shadowZFar');
		}
		public function set shadowZFar(v : Number) : void
		{
			lightData.setLightProperty('shadowZFar', v);
		}
		
		public function get shadowQuality() : uint
		{
			return lightData.getLightProperty('shadowQuality');
		}
		public function set shadowQuality(v : uint) : void
		{
			lightData.setLightProperty('shadowQuality', v);
		}
		
		public function get shadowSpread() : uint
		{
			return lightData.getLightProperty('shadowSpread');
		}
		public function set shadowSpread(v : uint) : void
		{
			lightData.setLightProperty('shadowSpread', v);
		}
		
		public function get shadowMapSize() : uint
		{
			return lightData.getLightProperty('shadowMapSize');
		}
		public function set shadowMapSize(v : uint) : void
		{
			lightData.setLightProperty('shadowMapSize', v);
		}
		
		public function get shadowMappingType() : uint
		{
			return lightData.getLightProperty('shadowMappingType');
		}
		public function set shadowMappingType(value : uint) : void
		{
			lightData.setLightProperty('shadowMappingType', value);
		}
        
        public function get shadowBias() : Number
        {
            return lightData.getLightProperty(PhongProperties.SHADOW_BIAS);
        }
        public function set shadowBias(value : Number) : void
        {
            lightData.setLightProperty(PhongProperties.SHADOW_BIAS, value);
        }
		
		public function DirectionalLight(color				: uint		= 0xFFFFFFFF,
									 	 diffuse			: Number	= .6,
										 specular			: Number	= .8,
										 shininess			: Number	= 64,
										 emissionMask		: uint		= 0x1,
										 shadowMappingType	: uint		= 0,
										 shadowMapSize		: uint		= 512,
										 shadowZFar			: Number	= 1000,
										 shadowWidth		: Number	= 20,
										 shadowQuality		: uint		= 0,
										 shadowSpread		: uint		= 1,
                                         shadowBias         : Number    = .002,
										 shadowColor		: uint		= 0x0)
		{
			super(
				new DirectionalLightController(),
				LIGHT_TYPE,
				color,
				emissionMask
			);
			
			this.diffuse			= diffuse;
			this.specular			= specular;
			this.shininess			= shininess;
			this.shadowMappingType	= shadowMappingType;
			this.shadowZFar			= shadowZFar;
			this.shadowWidth		= shadowWidth;
			this.shadowMapSize		= shadowMapSize;
			this.shadowQuality		= shadowQuality;
			this.shadowSpread		= shadowSpread;
            this.shadowBias     	= shadowBias;
			this.shadowColor		= shadowColor;
			
			transform.lookAt(Vector4.ZERO, new Vector4(1, 1, 1));
		}
		
		override minko_scene function cloneNode() : AbstractSceneNode
		{
			var light : DirectionalLight = new DirectionalLight(
				color,
				diffuse,
				specular,
				shininess,
				emissionMask,
				ShadowMappingType.NONE, // @fixme clone light with shadow triggered errors
				shadowMapSize,
				shadowZFar,
				shadowWidth,
				shadowQuality,
				shadowSpread,
                shadowBias
			);
			
			light.name = this.name;
			light.userData.setProperties(userData);
			light.transform.copyFrom(this.transform);
			
			return light;
		}
	}
}
