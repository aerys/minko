package aerys.minko.render.material.phong
{
	import aerys.minko.ns.minko_lighting;
	import aerys.minko.render.DataBindingsProxy;
	import aerys.minko.render.Effect;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.blur.BlurEffect;
	import aerys.minko.render.effect.blur.BlurQuality;
	import aerys.minko.render.geometry.primitive.QuadGeometry;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.resource.texture.CubeTextureResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.compiler.ShaderCompilerError;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.scene.RenderingController;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.light.AmbientLight;
	import aerys.minko.scene.node.light.DirectionalLight;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.type.enum.ShadowMappingQuality;
	import aerys.minko.type.enum.ShadowMappingType;
	
	public class PhongEffect extends Effect
	{
		use namespace minko_lighting;
		
		public function PhongEffect(renderingShader : Shader = null)
		{
            super();
		}
        
        override protected function initializePasses(sceneBindings	: DataBindingsProxy,
                                                     meshBindings	: DataBindingsProxy) : Vector.<Shader>
        {
            var passes : Vector.<Shader>    = super.initializePasses(sceneBindings, meshBindings);
            
            for (var lightId : uint = 0;
                lightPropertyExists(sceneBindings, lightId, 'enabled');
                ++lightId)
            {
                if (lightPropertyExists(sceneBindings, lightId, 'shadowCastingType'))
                {
                    var lightType			: uint	= getLightProperty(sceneBindings, lightId, 'type');
                    var shadowMappingType 	: uint	= getLightProperty(
                        sceneBindings, lightId, 'shadowCastingType'
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
            
            passes.push(new PhongSinglePassShader(null, 0));
            
            return passes;
        }
        
        override protected function initializeFallbackPasses(sceneBindings  : DataBindingsProxy,
                                                             meshBindings   : DataBindingsProxy) : Vector.<Shader>
        {
            var passes              : Vector.<Shader>   = super.initializeFallbackPasses(sceneBindings, meshBindings);
            var discardDirectional  : Boolean           = true;
            
			for (var lightId : uint = 0;
                lightPropertyExists(sceneBindings, lightId, 'enabled');
                ++lightId)
			{
                var lightType   : uint	= getLightProperty(sceneBindings, lightId, 'type');
                
                if (lightType != AmbientLight.LIGHT_TYPE
                    && getLightProperty(sceneBindings, lightId, 'enabled'))
                {
                    if (lightType == DirectionalLight.LIGHT_TYPE && discardDirectional)
                        discardDirectional = false;
                    else
                        passes.push(new PhongAdditionalShader(null, 0, lightId));
                }
                
				if (lightPropertyExists(sceneBindings, lightId, 'shadowCastingType'))
				{
					var shadowMappingType 	: uint	= getLightProperty(
						sceneBindings, lightId, 'shadowCastingType'
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
            
            passes.push(new PhongBaseShader(null, .5));
            
            return passes;
		}
		
		private function pushPCFShadowMappingPass(sceneBindings	: DataBindingsProxy,
													 lightId 		: uint,
													 passes 		: Vector.<Shader>) : void
		{
			var textureResource : TextureResource	= getLightProperty(
				sceneBindings, lightId, 'shadowMap'
			);
			var renderTarget	: RenderTarget		= new RenderTarget(
				textureResource.width, textureResource.height, textureResource, 0, 0xffffffff
			);
			
			passes.push(new PCFShadowMapShader(lightId, lightId + 1, renderTarget));
		}
		
		private function pushDualParaboloidShadowMappingPass(sceneBindings	: DataBindingsProxy,
															 lightId 		: uint,
															 passes 		: Vector.<Shader>) : void
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
				new ParaboloidShadowMapShader(lightId, true, lightId + 0.5, frontRenderTarget),
				new ParaboloidShadowMapShader(lightId, false, lightId + 1, backRenderTarget)
			);
		}
		
		private function pushCubeShadowMappingPass(sceneBindings	: DataBindingsProxy,
												   lightId 			: uint,
												   passes 			: Vector.<Shader>) : void
		{
			var cubeTexture	: CubeTextureResource	= getLightProperty(
				sceneBindings, lightId, 'shadowMap'
			);
			var textureSize	: uint					= cubeTexture.size;
			
			for (var i : uint = 0; i < 6; ++i)
				passes.push(new PCFShadowMapShader(
					lightId,
					lightId + .1 * i,
					new RenderTarget(textureSize, textureSize, cubeTexture, i, 0xffffffff),
					i
				));
		}
		
		private function pushVarianceShadowMappingPass(sceneBindings	: DataBindingsProxy,
													   lightId 			: uint,
													   passes 			: Vector.<Shader>) : void
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
				
				passes.push(new VarianceShadowMapShader(lightId, 4, lightId + 1, renderTarget));
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
                        lightId + .1 * i,
                        new RenderTarget(textureSize, textureSize, cubeTexture, i, 0xffffffff)
					));
			}
		}
		
		private function pushExponentialShadowMappingPass(sceneBindings	: DataBindingsProxy,
														  lightId 		: uint,
														  passes 		: Vector.<Shader>):void
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
				
				passes.push(new ExponentialShadowMapShader(lightId, 4, lightId + 1, renderTarget));
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
