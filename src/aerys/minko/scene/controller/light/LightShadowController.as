package aerys.minko.scene.controller.light
{
	import aerys.minko.render.Effect;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.blur.BlurEffect;
	import aerys.minko.render.geometry.primitive.QuadGeometry;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.render.resource.texture.CubeTextureResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.scene.RenderingController;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.camera.AbstractCamera;
	import aerys.minko.scene.node.light.AbstractLight;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.enum.ShadowMappingQuality;
	import aerys.minko.type.enum.ShadowMappingType;
	
	import flashx.textLayout.factory.TruncationOptions;

	public class LightShadowController extends LightController
	{
		/**
		 * A bitmask representing the supported shadow mapping techniques. 
		 */
		private var _shadowMappingSupport	: uint;
		private var _needsToAddBlurPass		: Boolean;
		private var _blurMesh				: Mesh;
		
		public function LightShadowController(lightClass 			: Class,
											  shadowMappingSupport	: uint)
		{
			super(lightClass);
			
			_shadowMappingSupport	= shadowMappingSupport;
			_needsToAddBlurPass		= false;
			_blurMesh				= null;
		}
		
		override protected function lightDataChangedHandler(lightData		: LightDataProvider,
															propertyName	: String,
															bindingName		: String,
															value			: Object) : void
		{
			super.lightDataChangedHandler(lightData, propertyName, bindingName, value);
			
			propertyName = LightDataProvider.getPropertyName(propertyName);
			
			// must update the shadowMap property
			if (propertyName == 'shadowMapSize' ||
				propertyName == 'shadowCastingType' ||
				propertyName == 'shadowQuality')
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
					if (lightType == PointLight.LIGHT_TYPE)
					{
						if (!((shadowMapSize & (~shadowMapSize + 1)) == shadowMapSize
							&& shadowMapSize <= 1024))
							throw new Error(shadowMapSize + ' is an invalid size for cubic shadow maps');
						
						shadowMap = new CubeTextureResource(shadowMapSize);
						lightData.setLightProperty('shadowMap', shadowMap);
					}
					else
					{
						if (!((shadowMapSize & (~shadowMapSize + 1)) == shadowMapSize
							&& shadowMapSize <= 2048))
							throw new Error(shadowMapSize + ' is an invalid size for a shadow map');
						
						shadowMap = new TextureResource(shadowMapSize, shadowMapSize);
						lightData.setLightProperty('shadowMap', shadowMap);
					}
					break;
				case ShadowMappingType.VARIANCE:
				case ShadowMappingType.EXPONENTIAL:
					pushVsmOrEsm(lightType, shadowMapSize, shadowMap);
					break;
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
		
		private function pushVsmOrEsm(lightType		: uint,
									  shadowMapSize : uint,
									  shadowMap		: ITextureResource) : ITextureResource
		{
			if (lightType != PointLight.LIGHT_TYPE)
			{
				if (!((shadowMapSize & (~shadowMapSize + 1)) == shadowMapSize
					&& shadowMapSize <= 2048))
					throw new Error(shadowMapSize + ' is an invalid size for a shadow map');
				
				var quality			: uint				= lightData.getLightProperty('shadowQuality');
				var rawShadowMap	: ITextureResource	= lightData.getLightProperty('rawShadowMap')
														as ITextureResource;
				if (rawShadowMap)
				{
					rawShadowMap.dispose();
					rawShadowMap = null;
				}
				
				if (quality > ShadowMappingQuality.HARD)
				{
					rawShadowMap = new TextureResource(shadowMapSize, shadowMapSize);
					lightData.setLightProperty('rawShadowMap', rawShadowMap);
				}
				shadowMap = new TextureResource(shadowMapSize, shadowMapSize);
				lightData.setLightProperty('shadowMap', shadowMap);
				
				if (quality > ShadowMappingQuality.HARD)
				{
					tryPushShadowBlurPass(rawShadowMap, shadowMap, quality, shadowMapSize);
				}
				else
				{
					tryRemoveBlurPass();
				}
			}
			else
			{
				if (!((shadowMapSize & (~shadowMapSize + 1)) == shadowMapSize
					&& shadowMapSize <= 1024))
					throw new Error(shadowMapSize + ' is an invalid size for cubic shadow maps');
				
				shadowMap = new CubeTextureResource(shadowMapSize);
				lightData.setLightProperty('shadowMap', shadowMap);
			}
			
			return shadowMap;
		}
		
		private function tryGetRenderingController(scene : Scene = null) : RenderingController
		{
			if (!scene)
			{
				var light		: AbstractLight	= this.light;
				if (!light)
					return null;
				scene = light.scene;
			}
			if (!scene)
				return null;
			var controllers	: Vector.<AbstractController>	= scene.getControllersByType(RenderingController);
			if (controllers.length == 0)
				return null;
			
			return RenderingController(controllers[0]);
		}
		
		private function tryRemoveBlurPass() : void
		{
			if (!_blurMesh)
				return;
			
			var renderingCtrl	: RenderingController	= tryGetRenderingController();
			if (!renderingCtrl)
				return;
			
			renderingCtrl.removeMesh(_blurMesh);
		}
		
		private function tryPushShadowBlurPass(rawShadowMap		: ITextureResource,
											   shadowMap		: ITextureResource,
											   shadowQuality	: uint,
											   shadowMapSize	: uint) : void
		{
			var renderingCtrl	: RenderingController			= tryGetRenderingController();
			if (!renderingCtrl)
			{
				_needsToAddBlurPass = true;
				
				return;
			}
			var numPasses		: uint							= shadowQuality >> 1;
			var destTexture		: ITextureResource				= shadowMap;
			var renderTarget	: RenderTarget					= new RenderTarget(
				shadowMapSize, shadowMapSize, destTexture
			);
			var passes			: Vector.<Shader>				= BlurEffect.getBlurPasses(
				shadowMapSize, numPasses, 1,
				rawShadowMap, renderTarget, lightData.lightId + .5);
			var material		: Material						= new Material(new Effect()
				.setExtraPasses(passes));
			_blurMesh											= new Mesh(QuadGeometry.quadGeometry, material);
			
			renderingCtrl.addMesh(_blurMesh);
		}
		
		override protected function lightAddedToScene(scene : Scene) : void
		{
			super.lightAddedToScene(scene);
			if (_needsToAddBlurPass)
			{
				var quality			: uint				= lightData.getLightProperty('shadowQuality');
				var shadowMapSize	: uint				= lightData.getLightProperty('shadowMapSize');
				var rawShadowMap	: ITextureResource	= lightData.getLightProperty('rawShadowMap')
														as ITextureResource;
				var shadowMap		: ITextureResource	= lightData.getLightProperty('shadowMap')
														as ITextureResource;
				
				tryPushShadowBlurPass(rawShadowMap, shadowMap, quality, shadowMapSize);
				_needsToAddBlurPass = false;
			}
		}
		
		override protected function lightRemovedFromScene(scene : Scene) : void
		{
			if (_blurMesh)
			{
				var renderCtrl	: RenderingController	= tryGetRenderingController(scene);
				renderCtrl.removeMesh(_blurMesh);
			}
			
			super.lightRemovedFromScene(scene);
		}
	}
}