package aerys.minko.scene.node.light
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.scene.controller.light.DirectionalLightController;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.enum.ShadowMappingType;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	use namespace minko_scene;
	
	/**
	 * @author Romain Gilliotte
	 */
	public class DirectionalLight extends AbstractLight
	{
		private static const LIGHT_TYPE : uint = 1;
		
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
		
		public function get shadowMapSize() : uint
		{
			return lightData.getLightProperty('shadowMapSize');
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
		
		public function set shininess(v : Number) : void
		{
			lightData.setLightProperty('shininess', v);
		}
		
		public function set shadowMapSize(v : uint) : void
		{
			lightData.setLightProperty('shadowMapSize', v);
			
			this.shadowCastingType = this.shadowCastingType;
		}
		override public function set shadowCastingType(v : uint) : void
		{
			var shadowMap		: TextureResource	= lightData.getLightProperty('shadowMap') as TextureResource;
			var shadowMapSize	: uint				= this.shadowMapSize;
			
			if (shadowMap)
			{
				lightData.removeProperty('shadowMap');
				shadowMap.dispose(); 
			}
			
			switch (v)
			{
				case ShadowMappingType.NONE:
					lightData.setLightProperty('shadowCastingType', ShadowMappingType.NONE);
					break;
				
				case ShadowMappingType.MATRIX:
					if (!((shadowMapSize & (~shadowMapSize + 1)) == shadowMapSize && shadowMapSize <= 2048))
						throw new Error(shadowMapSize + ' is an invalid size for a shadow map');
					
					shadowMap = new TextureResource(shadowMapSize, shadowMapSize);
					lightData.setLightProperty('shadowMap', shadowMap);
					lightData.setLightProperty('shadowCastingType', ShadowMappingType.MATRIX);
					break;
				
				default: 
					throw new ArgumentError('Invalid shadow casting type.');
			}
		}
		
		public function DirectionalLight(color						: uint		= 0xFFFFFFFF,
									 	 diffuse					: Number	= .6,
										 specular					: Number	= .8,
										 shininess					: Number	= 64,
										 emissionMask				: uint		= 0x1,
										 shadowCasting				: uint		= 0,
										 shadowMapSize				: uint		= 512,
										 shadowMapMaxZ				: Number	= 1000,
										 shadowMapWidth				: Number	= 20,
										 shadowMapQuality			: uint		= 0,
										 shadowMapSamplingDistance	: uint		= 1)
		{
			super(
				new DirectionalLightController(),
				LIGHT_TYPE,
				color,
				emissionMask,
				shadowCasting
			);
			
			this.diffuse					= diffuse;
			this.specular					= specular;
			this.shininess					= shininess;
			this.shadowZFar				= shadowMapMaxZ;
			this.shadowWidth				= shadowMapWidth;
			this.shadowMapSize				= shadowMapSize;
			this.shadowQuality			= shadowMapQuality;
			this.shadowSpread	= shadowMapSamplingDistance;
			
			if ([ShadowMappingType.NONE, ShadowMappingType.MATRIX].indexOf(shadowCasting) == -1)
				throw new Error('Invalid ShadowMappingType.');
			
			transform.lookAt(Vector4.ZERO, new Vector4(1, -1, 1));
		}
		
		override minko_scene function cloneNode() : AbstractSceneNode
		{
			var light : DirectionalLight = new DirectionalLight(
				color,
				diffuse,
				specular,
				shininess,
				emissionMask,
				shadowCastingType,
				shadowMapSize
			);
			
			light.name = this.name;
			light.transform.copyFrom(this.transform);
			
			return light;
		}
	}
}
