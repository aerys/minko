package aerys.minko.scene.node.light
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.scene.controller.light.SpotLightController;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.type.math.Vector4;
	
	use namespace minko_scene;
	
	/**
	 * 
	 * @author Romain Gilliotte
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class SpotLight extends AbstractLight
	{
		public static const LIGHT_TYPE	: uint  = 3;
		
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
		
		public function get innerRadius() : Number
		{
			return lightData.getLightProperty('innerRadius') as Number;
		}
		public function set innerRadius(v : Number) : void
		{
			lightData.setLightProperty('innerRadius', v);
			
			if (lightData.getLightProperty('smoothRadius') != (innerRadius != outerRadius))
				lightData.setLightProperty('smoothRadius', innerRadius != outerRadius)
		}
		
		public function get outerRadius() : Number
		{
			return lightData.getLightProperty('outerRadius') as Number;
		}
		public function set outerRadius(v : Number) : void
		{
			lightData.setLightProperty('outerRadius', v);
			
			if (lightData.getLightProperty('smoothRadius') != (innerRadius != outerRadius))
				lightData.setLightProperty('smoothRadius', innerRadius != outerRadius)
		}
		
		public function get attenuationDistance() : Number
		{
			return lightData.getLightProperty('attenuationDistance') as Number;
		}
		public function set attenuationDistance(v : Number) : void
		{
			lightData.setLightProperty('attenuationDistance', v);
			
			if (lightData.getLightProperty('attenuationEnabled') != (v != 0))
				lightData.setLightProperty('attenuationEnabled', v != 0);
		}
		
		public function get shadowZNear() : Number
		{
			return lightData.getLightProperty('shadowZNear');
		}
		public function set shadowZNear(v : Number) : void
		{
			lightData.setLightProperty('shadowZNear', v);
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
		
		public function set shininess(v : Number) : void
		{
			lightData.setLightProperty('shininess', v);
		}
		
		public function get shadowMapSize() : uint
		{
			return lightData.getLightProperty('shadowMapSize') as uint;
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
		
		public function get attenuationPolynomial() : Vector4
		{
			return lightData.getLightProperty('attenuationPolynomial');
		}
		public function set attenuationPolynomial(value : Vector4) : void
		{
			lightData.setLightProperty('attenuationPolynomial', value);
			
			var notEqual	: Boolean = value.equals(Vector4.ZERO);
			if (lightData.getLightProperty('attenuationEnabled') != notEqual)
				lightData.setLightProperty('attenuationEnabled', notEqual);
		}
		
		public function SpotLight(color					: uint		= 0xFFFFFFFF,
								  diffuse				: Number	= .6,
								  specular				: Number	= .8,
								  shininess				: Number	= 64,
								  attenuationDistance	: Number	= 0,
								  outerRadius			: Number	= 1.57079632679,
								  innerRadius			: Number	= 0,
								  emissionMask			: uint		= 0x1,
								  shadowMappingType		: uint		= 0,
								  shadowMapSize			: uint		= 512,
								  shadowZNear			: Number	= 0.1,
								  shadowZFar			: Number	= 1000,
								  shadowQuality			: uint		= 0,
								  shadowSpread			: uint		= 1,
                                  shadowBias            : Number    = 1. / 256. / 256.,
								  shadowColor			: uint		= 0x0)
		{
			super(
				new SpotLightController(),
				LIGHT_TYPE,
				color,
				emissionMask
			);
			
			this.diffuse				= diffuse;
			this.specular				= specular;
			this.shininess				= shininess;
			this.innerRadius			= innerRadius;
			this.outerRadius			= outerRadius;
			this.attenuationDistance	= attenuationDistance;
			this.shadowMappingType		= shadowMappingType;
			this.shadowZNear			= shadowZNear;
			this.shadowZFar				= shadowZFar;
			this.shadowMapSize			= shadowMapSize;
			this.shadowQuality			= shadowQuality;
			this.shadowSpread	        = shadowSpread;
            this.shadowBias             = shadowBias;

            if (shadowColor != 0)
				this.shadowColor		= shadowColor;
			
			transform.lookAt(Vector4.Z_AXIS, Vector4.ZERO);
		}
		
		override minko_scene function cloneNode() : AbstractSceneNode
		{
			var light : SpotLight = new SpotLight(
				color,
				diffuse,
				specular,
				shininess,
				attenuationDistance, 
				outerRadius,
				innerRadius,
				emissionMask, 
				shadowMappingType,
				shadowMapSize,
				shadowZNear,
				shadowZFar,
				shadowQuality,
				shadowSpread,
				shadowBias,
				shadowColor
			); 
			
			light.name = this.name;
			light.userData.setProperties(userData);
			light.transform.copyFrom(this.transform);
			
			return light;
		}
	}
}
