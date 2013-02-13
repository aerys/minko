package aerys.minko.render.material.phong
{
	import aerys.minko.ns.minko_lighting;
	import aerys.minko.render.DataBindingsProxy;
	import aerys.minko.render.Effect;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.geometry.primitive.QuadGeometry;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.resource.texture.CubeTextureResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.effect.blur.BlurEffect;
	import aerys.minko.render.effect.blur.BlurQuality;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.scene.RenderingController;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.type.enum.ShadowMappingQuality;
	import aerys.minko.type.enum.ShadowMappingType;
	
	public class PhongEffect extends Effect
	{
		use namespace minko_lighting;
		
		private static const DEFAUT_SHADER	: Shader	= new PhongShader();
		
		private var _renderingShader	: Shader;
		private var _scene				: Scene;
		
		public function PhongEffect(renderingShader : Shader = null, scene : Scene = null)
		{
            super();
            
			_renderingShader	= renderingShader || DEFAUT_SHADER;
			_scene				= scene;
		}
		
		override protected function initializePasses(sceneBindings	: DataBindingsProxy,
													 meshBindings	: DataBindingsProxy) : Vector.<Shader>
		{
			var passes			: Vector.<Shader>	= super.initializePasses(sceneBindings, meshBindings);
			
			var shader			: Shader			= null;
			var renderTarget	: RenderTarget		= null;
			
			for (var lightId : uint = 0; ; ++lightId)
			{
				if (!lightPropertyExists(sceneBindings, lightId, 'enabled'))
					break ;
				
				if (lightPropertyExists(sceneBindings, lightId, 'shadowCastingType'))
				{
					var shadowMappingType : uint = getLightProperty(
						sceneBindings, lightId, 'shadowCastingType'
					);
					
					switch (shadowMappingType)
					{
						case ShadowMappingType.PCF:
							pushMatrixShadowMappingPass(sceneBindings, lightId, passes);
							break ;
						case ShadowMappingType.CUBE:
							pushCubeShadowMappingPass(sceneBindings, lightId, passes);
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
			
			passes.push(_renderingShader);
			
			return passes;
		}
		
		private function pushMatrixShadowMappingPass(sceneBindings	: DataBindingsProxy,
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
				passes.push(new CubeShadowMapShader(
					lightId,
					i,
					lightId + .1 * i,
					new RenderTarget(textureSize, textureSize, cubeTexture, i, 0xffffffff)
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
				
				if (willAddShadowBlurPass(sceneBindings, lightId))
					textureResource	= getLightProperty(sceneBindings, lightId, 'rawShadowMap');
				else
					textureResource	= getLightProperty(sceneBindings, lightId, 'shadowMap');
				renderTarget		= new RenderTarget(
					textureResource.width, textureResource.height, textureResource, 0, 0xffffffff
				);
				
				passes.push(new VarianceShadowMapShader(lightId, 4, lightId + 1, renderTarget));
				tryAddShadowBlurPass(sceneBindings, lightId);
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
				
				if (willAddShadowBlurPass(sceneBindings, lightId))
					textureResource	= getLightProperty(sceneBindings, lightId, 'rawShadowMap');
				else
					textureResource	= getLightProperty(sceneBindings, lightId, 'shadowMap');
				renderTarget		= new RenderTarget(
					textureResource.width, textureResource.height, textureResource, 0, 0xffffffff
				);
				
				passes.push(new ExponentialShadowMapShader(lightId, 4, lightId + 1, renderTarget));
				tryAddShadowBlurPass(sceneBindings, lightId);
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
		
		private function willAddShadowBlurPass(sceneBindings	: DataBindingsProxy,
							    			   lightId 			: uint) : Boolean
		{
			var quality	: uint	= getLightProperty(sceneBindings, lightId, 'shadowQuality');
			
			return _scene && quality != ShadowMappingQuality.HARD;
		}
		
		private function tryAddShadowBlurPass(sceneBindings	: DataBindingsProxy,
											  lightId 		: uint) : ITextureResource
		{
			var quality			: uint							= getLightProperty(sceneBindings, lightId, 'shadowQuality');
			var texture			: TextureResource				= getLightProperty(sceneBindings, lightId, 'rawShadowMap');
			if (!_scene || quality == ShadowMappingQuality.HARD)
			{
				return texture;
			}
			
			var controllers		: Vector.<AbstractController>	= _scene.getControllersByType(RenderingController);
			if (controllers.length == 0)
				return texture;
			
			var numPasses		: uint							= quality >> 1;
			var destTexture		: TextureResource				= getLightProperty(sceneBindings, lightId, 'shadowMap');
			var renderingCtrl	: RenderingController			= RenderingController(controllers[0]);
			var renderTarget	: RenderTarget					= new RenderTarget(destTexture.width, destTexture.height, destTexture, 0, 0xffffffff, true, quality >> 1);
			var passes			: Vector.<Shader>				= BlurEffect.getBlurPasses(
				texture.width, quality, 1,
				texture, renderTarget, lightId + 2);
			var material		: Material						= new Material(new Effect().setExtraPasses(passes));
			var mesh			: Mesh							= new Mesh(QuadGeometry.quadGeometry, material);
			
			renderingCtrl.addMesh(mesh);
			
			return destTexture;
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
