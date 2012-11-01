package aerys.minko.scene.node.light
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.resource.texture.CubeTextureResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.scene.controller.light.PointLightController;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.enum.ShadowMappingType;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	use namespace minko_scene;
	
	/**
	 * 
	 * @author Romain Gilliotte
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class PointLight extends AbstractLight
	{
		public static const LIGHT_TYPE : uint = 2;
		
		private static const MAP_NAMES : Vector.<String> = new <String>[
			'shadowMapCube',
			'shadowMapDPFront',
			'shadowMapDPBack'
		];
		
		private static const TMP_VECTOR		: Vector4 = new Vector4();
		
		private var _shadowMapSize	: uint;
		
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
		public function set shininess(v : Number) : void
		{
			lightData.setLightProperty('shininess', v);
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
		
		public function get shadowMapSize() : uint
		{
			return _shadowMapSize;
		}
		public function set shadowMapSize(v : uint) : void
		{
			_shadowMapSize = v;
			
			this.shadowCastingType = this.shadowCastingType
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
		
		override public function set shadowCastingType(v : uint) : void
		{
			var shadowMap : ITextureResource;
			
			// start by clearing current shadow maps.
			for each (var mapName : String in MAP_NAMES)
			{
				shadowMap = lightData.getLightProperty(mapName) as ITextureResource;
				if (shadowMap !== null)
				{
					shadowMap.dispose();
					lightData.removeProperty(mapName);
				}
			}
			
			switch (v)
			{
				case ShadowMappingType.NONE:
					lightData.setLightProperty('shadowCastingType', ShadowMappingType.NONE);
					break;
				
				case ShadowMappingType.DUAL_PARABOLOID:
					if (!((_shadowMapSize & (~_shadowMapSize + 1)) == _shadowMapSize
						&& _shadowMapSize <= 2048))
						throw new Error(_shadowMapSize + ' is an invalid size for dual paraboloid shadow maps');
					
					// set textures and shadowmaptype
					shadowMap = new TextureResource(_shadowMapSize, _shadowMapSize);
					lightData.setLightProperty('shadowMapDPFront', shadowMap);
					shadowMap = new TextureResource(_shadowMapSize, _shadowMapSize);
					lightData.setLightProperty('shadowMapDPBack', shadowMap);
					lightData.setLightProperty('shadowCastingType', ShadowMappingType.DUAL_PARABOLOID);
					break;
				
				case ShadowMappingType.CUBE:
					if (!((_shadowMapSize & (~_shadowMapSize + 1)) == _shadowMapSize
						&& _shadowMapSize <= 1024))
						throw new Error(_shadowMapSize + ' is an invalid size for cubic shadow maps');
					
					shadowMap = new CubeTextureResource(_shadowMapSize);
					lightData.setLightProperty('shadowMapCube', shadowMap);
					lightData.setLightProperty('shadowCastingType', ShadowMappingType.CUBE);
					break;
				
				default: 
					throw new ArgumentError('Invalid shadow casting type.');
			}
		}
		
		public function PointLight(color				: uint		= 0xFFFFFFFF,
								   diffuse				: Number	= .6,
								   specular				: Number	= .8,
								   shininess			: Number	= 64,
								   attenuationDistance	: Number	= 0,
								   emissionMask			: uint		= 0x1,
								   shadowCastingType	: uint		= 0,
								   shadowMapSize		: uint		= 512,
								   shadowMapZNear		: Number	= 0.1,
								   shadowMapZFar		: Number	= 1000)
		{
			_shadowMapSize				= shadowMapSize;
			
			super(
				new PointLightController(),
				LIGHT_TYPE,
				color,
				emissionMask,
				shadowCastingType
			);
			
			this.diffuse				= diffuse;
			this.specular				= specular;
			this.shininess				= shininess;
			this.attenuationDistance	= attenuationDistance;
			this.shadowZNear			= shadowMapZNear;
			this.shadowZFar			= shadowMapZFar;
			
			if ([ShadowMappingType.NONE,
				 ShadowMappingType.DUAL_PARABOLOID,
				 ShadowMappingType.CUBE].indexOf(shadowCastingType) == -1)
				throw new Error('Invalid ShadowMappingType.');
		}
		
		override minko_scene function cloneNode() : AbstractSceneNode
		{
			var light : PointLight = new PointLight(
				color, diffuse, specular, shininess, 
				attenuationDistance, emissionMask, 
				shadowCastingType, shadowMapSize
			);		
			
			light.name = this.name;
			light.transform.copyFrom(this.transform);
			
			return light;
		}
	}
}
