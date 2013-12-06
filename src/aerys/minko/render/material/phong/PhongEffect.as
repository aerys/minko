package aerys.minko.render.material.phong
{
	import aerys.minko.render.DataBindingsProxy;
	import aerys.minko.render.Effect;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.phong.multipass.PhongAdditionalShader;
	import aerys.minko.render.material.phong.multipass.PhongAmbientShader;
	import aerys.minko.render.material.phong.multipass.PhongEmissiveShader;
	import aerys.minko.render.material.phong.multipass.ZPrepassShader;
	import aerys.minko.render.material.phong.shadow.ExponentialShadowMapShader;
	import aerys.minko.render.material.phong.shadow.PCFShadowMapShader;
	import aerys.minko.render.material.phong.shadow.ParaboloidShadowMapShader;
	import aerys.minko.render.material.phong.shadow.VarianceShadowMapShader;
	import aerys.minko.render.resource.texture.CubeTextureResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.light.AmbientLight;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.type.enum.ShadowMappingQuality;
	import aerys.minko.type.enum.ShadowMappingType;
	
    /**
     * <p>The PhongEffect using the Phong lighting model to render the geometry according to
     * the lighting setup of the scene. It supports an infinite number of lights/projected
     * shadows and will automatically switch between singlepass and multipass rendering
     * in order to give the best performances whenever possible.</p>
     * 
     * </p>Because of the Stage3D restrictions regarding the number of shader operations or
     * the number of available registers, the number of lights might be to big to allow them
     * to be rendered in a single pass. In this situation, the PhongEffect will automatically
     * fallback and use multipass rendering.</p>
     * 
     * <p>Multipass rendering is done as follow:</p>
     * <ul>
     * <li>The "base" pass renders objects with one per-pixel directional lights with shadows,
     * the lightmap and the ambient/emissive lighting.</li>
     * <li>Each "additional" pass (one per light) will render a single light with shadows and
     * blend it using additive blending.</li>
     * </ul>
     * 
     * The singlepass rendering will mimic this behavior in order to get preserve consistency.
     * 
     * @author Jean-Marc Le Roux
     * 
     */
	public class PhongEffect extends Effect
	{
		private static var _id : uint = 4242;
		
		private var _diffuseRenderTarget	: RenderTarget;
		private var _specularRenderTarget	: RenderTarget;
        private var _singlePassShader   	: Shader;
        private var _emissiveShader     	: Shader;
        
		public function PhongEffect(singlePassShader	: Shader	= null,
                                    emissiveShader      : Shader    = null)
		{
            super();
			
			++_id;
			
            _singlePassShader 	= singlePassShader || new PhongSinglePassShader(null, 0);
            _emissiveShader 	= emissiveShader || new PhongEmissiveShader(null, null, null, .25);
		}
        
        override protected function initializePasses(sceneBindings	: DataBindingsProxy,
                                                     meshBindings	: DataBindingsProxy) : Vector.<Shader>
        {
            var passes : Vector.<Shader>    = super.initializePasses(sceneBindings, meshBindings);
            
            for (var lightId : uint = 0;
                lightPropertyExists(sceneBindings, lightId, 'enabled')
                    && getLightProperty(sceneBindings, lightId, 'enabled');
                ++lightId)
            {
                if (lightPropertyExists(sceneBindings, lightId, 'shadowMappingType'))
                {
                    var lightType			: uint	= getLightProperty(sceneBindings, lightId, 'type');
                    var shadowMappingType 	: uint	= getLightProperty(
                        sceneBindings, lightId, 'shadowMappingType'
                    );
                    
                    switch (shadowMappingType)
                    {
                        case ShadowMappingType.PCF:
                            if (lightType == PointLight.LIGHT_TYPE)
                                pushCubeShadowMappingPass(sceneBindings, lightId, passes);
                            else
                                pushPCFShadowMappingPass(sceneBindings, lightId, passes);
                            break ;
                        case ShadowMappingType.DUAL_PARABOLOID:
                            pushDualParaboloidShadowMappingPass(sceneBindings, lightId, passes);
                            break ;
                        case ShadowMappingType.VARIANCE:
                            pushVarianceShadowMappingPass(sceneBindings, lightId, passes);
                            break ;
                        case ShadowMappingType.EXPONENTIAL:
                            pushExponentialShadowMappingPass(sceneBindings, lightId, passes);
                            break ;
                    }
                }
            }
            
            passes.push(_singlePassShader);
            
            return passes;
        }
        
        override protected function initializeFallbackPasses(sceneBindings  : DataBindingsProxy,
                                                             meshBindings   : DataBindingsProxy) : Vector.<Shader>
        {
            var passes              : Vector.<Shader>   = new <Shader>[];
            var discardDirectional  : Boolean           = true;
            var ambientEnabled      : Boolean           = meshBindings.propertyExists('lightmap');
			
			if (!_diffuseRenderTarget)
			{
	            var accumulatorSize : uint	= sceneBindings.getProperty('viewportWidth');
				
	            accumulatorSize = 1 << Math.ceil(Math.log(accumulatorSize) * Math.LOG2E);
	            
	            _diffuseRenderTarget = new RenderTarget(
	                accumulatorSize, accumulatorSize,
	                new TextureResource(accumulatorSize, accumulatorSize)
	            );
				
				_specularRenderTarget = new RenderTarget(
					accumulatorSize, accumulatorSize,
					new TextureResource(accumulatorSize, accumulatorSize)
				);
			}
            
			for (var lightId : uint = 0;
                lightPropertyExists(sceneBindings, lightId, 'enabled')
                    && getLightProperty(sceneBindings, lightId, 'enabled');
                ++lightId)
			{
                var lightType : uint = getLightProperty(sceneBindings, lightId, 'type');
                
                if (lightType == AmbientLight.LIGHT_TYPE)
                {
                    ambientEnabled = true;
                    continue;
                }
                
                if (getLightProperty(sceneBindings, lightId, 'diffuseEnabled'))
                    passes.push(
                        new PhongAdditionalShader(lightId, true, false, _diffuseRenderTarget, _id + 7)
                    );
                
                if (getLightProperty(sceneBindings, lightId, 'specularEnabled'))
                    passes.push(
                        new PhongAdditionalShader(lightId, false, true, _specularRenderTarget, _id + 5)
                    );
                
				if (lightPropertyExists(sceneBindings, lightId, 'shadowMappingType'))
				{
					var shadowMappingType 	: uint	= getLightProperty(
						sceneBindings, lightId, 'shadowMappingType'
					);
					
					switch (shadowMappingType)
					{
						case ShadowMappingType.PCF:
							if (lightType == PointLight.LIGHT_TYPE)
								pushCubeShadowMappingPass(sceneBindings, lightId, passes, true);
							else
								pushPCFShadowMappingPass(sceneBindings, lightId, passes, true);
							break ;
						case ShadowMappingType.DUAL_PARABOLOID:
							pushDualParaboloidShadowMappingPass(sceneBindings, lightId, passes, true);
							break ;
						case ShadowMappingType.VARIANCE:
							pushVarianceShadowMappingPass(sceneBindings, lightId, passes, true);
							break ;
						case ShadowMappingType.EXPONENTIAL:
							pushExponentialShadowMappingPass(sceneBindings, lightId, passes, true);
							break ;
					}
				}
			}
            
            if (ambientEnabled)
                passes.push(new PhongAmbientShader(_diffuseRenderTarget, _id + 8));
            
            passes.push(new ZPrepassShader(_diffuseRenderTarget, _id + 9));
			passes.push(new ZPrepassShader(_specularRenderTarget, _id + 6));
            passes.push(new PhongEmissiveShader(_diffuseRenderTarget.textureResource, _specularRenderTarget.textureResource, null, _id));
            
            return passes;
		}
		
		private function pushPCFShadowMappingPass(sceneBindings	: DataBindingsProxy,
                                                  lightId 		: uint,
                                                  passes 		: Vector.<Shader>,
												  fallback 		: Boolean = false) : void
		{
			var textureResource : TextureResource	= getLightProperty(
				sceneBindings, lightId, 'shadowMap'
			);
			var renderTarget	: RenderTarget		= new RenderTarget(
				textureResource.width, textureResource.height, textureResource, 0, 0xffffffff
			);
			
			passes.push(new PCFShadowMapShader(lightId, lightId + 1 + (fallback ? _id : 0), renderTarget));
		}
		
		private function pushDualParaboloidShadowMappingPass(sceneBindings	: DataBindingsProxy,
															 lightId 		: uint,
															 passes 		: Vector.<Shader>,
															 fallback 		: Boolean = false) : void
		{
			var frontTextureResource : TextureResource	= getLightProperty(
				sceneBindings, lightId, 'shadowMapFront'
			);
			var backTextureResource	 : TextureResource	= getLightProperty(
				sceneBindings, lightId, 'shadowMapBack'
			);
			var size				 : uint				= frontTextureResource.width;
			var frontRenderTarget	 : RenderTarget		= new RenderTarget(
				size, size, frontTextureResource, 0, 0xffffffff
			);
			var backRenderTarget	 : RenderTarget		= new RenderTarget(
				size, size, backTextureResource, 0, 0xffffffff
			);
			
			passes.push(
				new ParaboloidShadowMapShader(lightId, true, lightId + 0.5 + (fallback ? _id : 0), frontRenderTarget),
				new ParaboloidShadowMapShader(lightId, false, lightId + 1 + (fallback ? _id : 0), backRenderTarget)
			);
		}
		
		private function pushCubeShadowMappingPass(sceneBindings	: DataBindingsProxy,
												   lightId 			: uint,
												   passes 			: Vector.<Shader>,
												   fallback 		: Boolean = false) : void
		{
			var cubeTexture	: CubeTextureResource	= getLightProperty(
				sceneBindings, lightId, 'shadowMap'
			);
			var textureSize	: uint					= cubeTexture.size;
			
			for (var i : uint = 0; i < 6; ++i)
				passes.push(new PCFShadowMapShader(
					lightId,
					lightId + .1 * i + (fallback ? _id : 0),
					new RenderTarget(textureSize, textureSize, cubeTexture, i, 0xffffffff),
					i
				));
		}
		
		private function pushVarianceShadowMappingPass(sceneBindings	: DataBindingsProxy,
													   lightId 			: uint,
													   passes 			: Vector.<Shader>,
													   fallback 		: Boolean = false) : void
		{
			var lightType	: uint	= getLightProperty(
				sceneBindings, lightId, 'type'
			);
			
			if (lightType != PointLight.LIGHT_TYPE)
			{
				var textureResource : ITextureResource	= null;
				var renderTarget	: RenderTarget		= null;
				
				if (hasShadowBlurPass(sceneBindings, lightId))
					textureResource	= getLightProperty(sceneBindings, lightId, 'rawShadowMap');
				else
					textureResource	= getLightProperty(sceneBindings, lightId, 'shadowMap');
				
				renderTarget		= new RenderTarget(
					textureResource.width, textureResource.height, textureResource, 0, 0xffffffff
				);
				
				passes.push(new VarianceShadowMapShader(lightId, 4, lightId + 1 + (fallback ? _id : 0), renderTarget));
			}
			else
			{
				var cubeTexture		: CubeTextureResource	= getLightProperty(
					sceneBindings, lightId, 'shadowMap'
				);
				var textureSize		: uint					= cubeTexture.size;
				
				for (var i : uint = 0; i < 6; ++i)
					passes.push(new VarianceShadowMapShader(
                        lightId,
                        i,
                        lightId + .1 * i + (fallback ? _id : 0),
                        new RenderTarget(textureSize, textureSize, cubeTexture, i, 0xffffffff)
					));
			}
		}
		
		private function pushExponentialShadowMappingPass(sceneBindings	: DataBindingsProxy,
														  lightId 		: uint,
														  passes 		: Vector.<Shader>,
														  fallback 		: Boolean = false):void
		{
			var lightType	: uint	= getLightProperty(
				sceneBindings, lightId, 'type'
			);
			
			if (lightType != PointLight.LIGHT_TYPE)
			{
				var textureResource : ITextureResource	= null;
				var renderTarget	: RenderTarget		= null;
				
				if (hasShadowBlurPass(sceneBindings, lightId))
					textureResource	= getLightProperty(sceneBindings, lightId, 'rawShadowMap');
				else
					textureResource	= getLightProperty(sceneBindings, lightId, 'shadowMap');
				
				renderTarget = new RenderTarget(
					textureResource.width, textureResource.height, textureResource, 0, 0xffffffff
				);
				
				passes.push(new ExponentialShadowMapShader(lightId, 4, lightId + 1 + (fallback ? _id : 0), renderTarget));
			}
			else
			{
				var cubeTexture		: CubeTextureResource	= getLightProperty(
					sceneBindings, lightId, 'shadowMap'
				);
				var textureSize		: uint					= cubeTexture.size;
				
				for (var i : uint = 0; i < 6; ++i)
					passes.push(new ExponentialShadowMapShader(
						lightId,
						i,
						lightId + .1 * i,
						new RenderTarget(textureSize, textureSize, cubeTexture, i, 0xffffffff)
					));
			}
		}
		
		private function hasShadowBlurPass(sceneBindings	: DataBindingsProxy,
							    		   lightId 			: uint) : Boolean
		{
			var quality	: uint	= getLightProperty(sceneBindings, lightId, 'shadowQuality');
			
			return quality > ShadowMappingQuality.HARD;
		}
		
		private function lightPropertyExists(sceneBindings 	: DataBindingsProxy,
											 lightId 		: uint,
											 propertyName 	: String) : Boolean
		{
			return sceneBindings.propertyExists(
				LightDataProvider.getLightPropertyName(propertyName, lightId)
			);
		}
		
		private function getLightProperty(sceneBindings : DataBindingsProxy,
										  lightId 		: uint,
										  propertyName 	: String) : *
		{
			return sceneBindings.getProperty(
				LightDataProvider.getLightPropertyName(propertyName, lightId)
			);
		}
	}
}
