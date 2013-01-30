package aerys.minko.scene.controller.light
{
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.render.resource.texture.CubeTextureResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.enum.ShadowMappingType;

	public class LightShadowController extends LightController
	{
		/**
		 * A bitmask representing the supported shadow mapping techniques. 
		 */
		private var _shadowMappingSupport	: uint;
		
		public function LightShadowController(lightClass 			: Class,
											  shadowMappingSupport	: uint)
		{
			super(lightClass);
			
			_shadowMappingSupport = shadowMappingSupport;
		}
		
		override protected function lightDataChangedHandler(lightData		: LightDataProvider,
															propertyName	: String,
															bindingName		: String,
															value			: Object) : void
		{
			super.lightDataChangedHandler(lightData, propertyName, bindingName, value);
			
			propertyName = LightDataProvider.getPropertyName(propertyName);
			
			// must update the shadowMap property
			if (propertyName == 'shadowMapSize' || propertyName == 'shadowCastingType')
				updateShadowMap();
		}
		
		/**
		 * This method updates the 'shadowMap' data provider property according to the other
		 * settings of the light.
		 */
		protected function updateShadowMap() : void
		{
			var shadowMappingType	: uint				= lightData.getLightProperty('shadowCastingType');
			var shadowMapSize		: uint				= lightData.getLightProperty('shadowMapSize');
			var shadowMap			: ITextureResource	= lightData.getLightProperty('shadowMap')
				as TextureResource;
			var lightType			: uint				= lightData.getLightProperty('type');
			
			if (shadowMappingType != ShadowMappingType.NONE
				&& !(_shadowMappingSupport & shadowMappingType))
				throw new Error('This shadow mapping technique is not supported by this light type');
			
			if (shadowMap)
			{
				lightData.removeLightProperty('shadowMap');
				shadowMap.dispose();
			}
			
			switch (shadowMappingType)
			{
				case ShadowMappingType.NONE:
					break;
				
				case ShadowMappingType.PCF:
					if (!((shadowMapSize & (~shadowMapSize + 1)) == shadowMapSize
						&& shadowMapSize <= 2048))
						throw new Error(shadowMapSize + ' is an invalid size for a shadow map');
					
					shadowMap = new TextureResource(shadowMapSize, shadowMapSize);
					lightData.setLightProperty('shadowMap', shadowMap);
					break;
				
				case ShadowMappingType.CUBE:
					if (!((shadowMapSize & (~shadowMapSize + 1)) == shadowMapSize
						&& shadowMapSize <= 1024))
						throw new Error(shadowMapSize + ' is an invalid size for cubic shadow maps');
					
					shadowMap = new CubeTextureResource(shadowMapSize);
					lightData.setLightProperty('shadowMap', shadowMap);
					break ;
				
				case ShadowMappingType.VARIANCE:
				case ShadowMappingType.EXPONENTIAL:
					if (lightType != PointLight.LIGHT_TYPE)
					{
						if (!((shadowMapSize & (~shadowMapSize + 1)) == shadowMapSize
							&& shadowMapSize <= 2048))
							throw new Error(shadowMapSize + ' is an invalid size for a shadow map');
						shadowMap = new TextureResource(shadowMapSize, shadowMapSize);
						lightData.setLightProperty('shadowMap', shadowMap);
					}
					else
					{
						if (!((shadowMapSize & (~shadowMapSize + 1)) == shadowMapSize
							&& shadowMapSize <= 1024))
							throw new Error(shadowMapSize + ' is an invalid size for cubic shadow maps');
						
						shadowMap = new CubeTextureResource(shadowMapSize);
						lightData.setLightProperty('shadowMap', shadowMap);
					}
					break ;
				
				case ShadowMappingType.DUAL_PARABOLOID:
//					throw new Error('Dual paraboloïd shadow mapping is yet to be implemented.');
					
					var frontShadowMap	: TextureResource	= lightData.getLightProperty(
						'shadowMapFront'
					);
					var backShadowMap	: TextureResource	= lightData.getLightProperty(
						'shadowMapBack'
					);
					
					if (!frontShadowMap || shadowMapSize != frontShadowMap.width)
					{
						if (!((shadowMapSize & (~shadowMapSize + 1)) == shadowMapSize
							&& shadowMapSize <= 2048))
							throw new Error(shadowMapSize + ' is an invalid size for a shadow map');
						
						if (frontShadowMap)
						{
							frontShadowMap.dispose();
							backShadowMap.dispose();
						}
						
						lightData.setLightProperty(
							'shadowMapFront',
							new TextureResource(shadowMapSize, shadowMapSize)
						);
						
						lightData.setLightProperty(
							'shadowMapBack',
							new TextureResource(shadowMapSize, shadowMapSize)
						);
					}
					
					break ;
				
				default: 
					throw new ArgumentError('Invalid shadow mapping technique.');
			}
		}
	}
}