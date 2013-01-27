package aerys.minko.render.material.phong
{
	import aerys.minko.ns.minko_lighting;
	import aerys.minko.render.Effect;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.Viewport;
	import aerys.minko.render.resource.texture.CubeTextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.light.AbstractLight;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.enum.ShadowMappingType;
	
	import flash.display.BitmapData;
	
	public class AbstractShadowMappingEffect extends Effect
	{
		use namespace minko_lighting;
		
		private var _scene				: Scene;
		private var _renderingPass		: Shader;
		private var _watchedProperties	: Vector.<String>;
		
		public function get scene() : Scene
		{
			return _scene;
		}
		
		public function AbstractShadowMappingEffect(scene 			: Scene, 
													renderingShader : Shader)
		{
            super();
            
			_renderingPass		= renderingShader;
			_watchedProperties	= new Vector.<String>();
			_scene				= scene;
			
            _scene.descendantAdded.add(sceneDescendantAddedHandler);
            updateNextFrame();
		}
        
        private function updateNextFrame() : void
        {
            if (!_scene.enterFrame.hasCallback(sceneEnterFrameHandler))
                _scene.enterFrame.add(sceneEnterFrameHandler);
        }
		
		private function sceneEnterFrameHandler(scene		: Scene,
                                                viewport	: Viewport,
                                                destination	: BitmapData, 
                                                timer		: uint) : void
		{
            _scene.enterFrame.remove(sceneEnterFrameHandler);
            
			updatePasses();
		}
        
        private function sceneDescendantAddedHandler(parent     : Group,
                                                     descendant : ISceneNode) : void
        {
            if (descendant is AbstractLight
                || (descendant is Group
                    && (descendant as Group).getDescendantsByType(AbstractLight).length))
                updateNextFrame();
        }
		
		private function propertyChangedHandler(sceneBindings	: DataBindings, 
												propertyName	: String,
												oldValue		: Object,
												newValue		: Object) : void
		{
            updateNextFrame();
		}
		
		private function updatePasses() : void
		{
			var passes			: Vector.<Shader>	= new <Shader>[];
			var sceneBindings	: DataBindings		= _scene.bindings;
			var shader			: Shader			= null;
			var renderTarget	: RenderTarget		= null;
			var lightId 		: uint 				= 0;
			
			while (_watchedProperties.length != 0)
				sceneBindings.removeCallback(_watchedProperties.pop(), propertyChangedHandler);
			
			while (lightPropertyExists(lightId++, 'type'))
			{
				var shadowMappingPropertyName : String = LightDataProvider.getLightPropertyName(
					'shadowCastingType',
					lightId
				);
				
				_watchedProperties.push(shadowMappingPropertyName);
				sceneBindings.addCallback(shadowMappingPropertyName, propertyChangedHandler);
				
				if (sceneBindings.propertyExists(shadowMappingPropertyName))
				{
					var shadowMappingType : uint = sceneBindings.getProperty(
						shadowMappingPropertyName
					);
					
					switch (shadowMappingType)
					{
						case ShadowMappingType.PCF:
							pushMatrixShadowMappingPass(lightId, passes);
							break ;
						case ShadowMappingType.CUBE:
							pushCubeShadowMappingPass(lightId, passes);
							break ;
						case ShadowMappingType.DUAL_PARABOLOID:
							pushDualParaboloidShadowMappingPass(lightId, passes);
							break ;
						case ShadowMappingType.VARIANCE:
							pushVarianceShadowMappingPass(lightId, passes);
							break ;
						case ShadowMappingType.EXPONENTIAL:
							pushExponentialShadowMappingPass(lightId, passes);
							break ;
					}
				}
			}
		
			passes.push(_renderingPass);
		
			setPasses(passes);
		}
		
		private function pushMatrixShadowMappingPass(lightId : uint, passes : Vector.<Shader>) : void
		{
			var textureResource : TextureResource	= getLightProperty(lightId, 'shadowMap');
			var renderTarget	: RenderTarget		= new RenderTarget(
				textureResource.width, textureResource.height, textureResource, 0, 0xffffffff
			);
			
			passes.push(new PCFShadowMapShader(lightId, lightId + 1, renderTarget));
		}
		
		private function pushDualParaboloidShadowMappingPass(lightId : uint, passes : Vector.<Shader>) : void
		{
			var frontTextureResource : TextureResource	= getLightProperty(lightId, 'shadowMapFront');
			var backTextureResource	 : TextureResource	= getLightProperty(lightId, 'shadowMapBack');
			var size				 : uint				= frontTextureResource.width;
			var frontRenderTarget	 : RenderTarget		= new RenderTarget(size, size, frontTextureResource, 0, 0xffffffff);
			var backRenderTarget	 : RenderTarget		= new RenderTarget(size, size, backTextureResource, 0, 0xffffffff);
			
			passes.push(
				new ParaboloidShadowMapShader(lightId, true, lightId + 0.5, frontRenderTarget),
				new ParaboloidShadowMapShader(lightId, false, lightId + 1, backRenderTarget)
			);
		}
		
		private function pushCubeShadowMappingPass(lightId : uint, passes : Vector.<Shader>) : void
		{
			var textureResource	: CubeTextureResource	= getLightProperty(lightId, 'shadowMap');
			var size			: uint					= textureResource.size;
			var renderTarget0	: RenderTarget			= new RenderTarget(size, size, textureResource, 0, 0xffffffff);
			var renderTarget1	: RenderTarget			= new RenderTarget(size, size, textureResource, 1, 0xffffffff);
			var renderTarget2	: RenderTarget			= new RenderTarget(size, size, textureResource, 2, 0xffffffff);
			var renderTarget3	: RenderTarget			= new RenderTarget(size, size, textureResource, 3, 0xffffffff);
			var renderTarget4	: RenderTarget			= new RenderTarget(size, size, textureResource, 4, 0xffffffff);
			var renderTarget5	: RenderTarget			= new RenderTarget(size, size, textureResource, 5, 0xffffffff);
			
			passes.push(
				new CubeShadowMapShader(lightId, 0, lightId + 0.1, renderTarget0),
				new CubeShadowMapShader(lightId, 1, lightId + 0.2, renderTarget1),
				new CubeShadowMapShader(lightId, 2, lightId + 0.3, renderTarget2),
				new CubeShadowMapShader(lightId, 3, lightId + 0.4, renderTarget3),
				new CubeShadowMapShader(lightId, 4, lightId + 0.5, renderTarget4),
				new CubeShadowMapShader(lightId, 5, lightId + 0.6, renderTarget5)
			);
		}
		
		private function pushVarianceShadowMappingPass(lightId : uint, passes : Vector.<Shader>) : void
		{
			var lightType			: uint				= getLightProperty(lightId, 'type');
			if (lightType != PointLight.LIGHT_TYPE)
			{
				var textureResource : TextureResource	= getLightProperty(lightId, 'shadowMap');
				var renderTarget	: RenderTarget		= new RenderTarget(
					textureResource.width, textureResource.height, textureResource, 0, 0xffffffff
				);
				
				passes.push(new VarianceShadowMapShader(lightId, 4, lightId + 1, renderTarget));
			}
			else
			{
				var cubeTexture		: CubeTextureResource	= getLightProperty(lightId, 'shadowMap');
				var size			: uint					= cubeTexture.size;
				var renderTarget0	: RenderTarget			= new RenderTarget(size, size, cubeTexture, 0, 0xffffffff);
				var renderTarget1	: RenderTarget			= new RenderTarget(size, size, cubeTexture, 1, 0xffffffff);
				var renderTarget2	: RenderTarget			= new RenderTarget(size, size, cubeTexture, 2, 0xffffffff);
				var renderTarget3	: RenderTarget			= new RenderTarget(size, size, cubeTexture, 3, 0xffffffff);
				var renderTarget4	: RenderTarget			= new RenderTarget(size, size, cubeTexture, 4, 0xffffffff);
				var renderTarget5	: RenderTarget			= new RenderTarget(size, size, cubeTexture, 5, 0xffffffff);
				
				passes.push(
					new VarianceShadowMapShader(lightId, 0, lightId + 0.1, renderTarget0),
					new VarianceShadowMapShader(lightId, 1, lightId + 0.2, renderTarget1),
					new VarianceShadowMapShader(lightId, 2, lightId + 0.3, renderTarget2),
					new VarianceShadowMapShader(lightId, 3, lightId + 0.4, renderTarget3),
					new VarianceShadowMapShader(lightId, 4, lightId + 0.5, renderTarget4),
					new VarianceShadowMapShader(lightId, 5, lightId + 0.6, renderTarget5)
				);
			}

		}
		
		private function pushExponentialShadowMappingPass(lightId:uint, passes:Vector.<Shader>):void
		{
			var lightType			: uint				= getLightProperty(lightId, 'type');
			if (lightType != PointLight.LIGHT_TYPE)
			{
				var textureResource : TextureResource	= getLightProperty(lightId, 'shadowMap');
				var renderTarget	: RenderTarget		= new RenderTarget(
					textureResource.width, textureResource.height, textureResource, 0, 0xffffffff
				);
			
				passes.push(new ExponentialShadowMapShader(lightId, 4, lightId + 1, renderTarget));
			}
			else
			{
				var cubeTexture		: CubeTextureResource	= getLightProperty(lightId, 'shadowMap');
				var size			: uint					= cubeTexture.size;
				var renderTarget0	: RenderTarget			= new RenderTarget(size, size, cubeTexture, 0, 0xffffffff);
				var renderTarget1	: RenderTarget			= new RenderTarget(size, size, cubeTexture, 1, 0xffffffff);
				var renderTarget2	: RenderTarget			= new RenderTarget(size, size, cubeTexture, 2, 0xffffffff);
				var renderTarget3	: RenderTarget			= new RenderTarget(size, size, cubeTexture, 3, 0xffffffff);
				var renderTarget4	: RenderTarget			= new RenderTarget(size, size, cubeTexture, 4, 0xffffffff);
				var renderTarget5	: RenderTarget			= new RenderTarget(size, size, cubeTexture, 5, 0xffffffff);
				
				passes.push(
					new ExponentialShadowMapShader(lightId, 0, lightId + 0.1, renderTarget0),
					new ExponentialShadowMapShader(lightId, 1, lightId + 0.2, renderTarget1),
					new ExponentialShadowMapShader(lightId, 2, lightId + 0.3, renderTarget2),
					new ExponentialShadowMapShader(lightId, 3, lightId + 0.4, renderTarget3),
					new ExponentialShadowMapShader(lightId, 4, lightId + 0.5, renderTarget4),
					new ExponentialShadowMapShader(lightId, 5, lightId + 0.6, renderTarget5)
				);
			}
		}
		
		private function lightPropertyExists(lightId : uint, propertyName : String) : Boolean
		{
			return _scene.bindings.propertyExists(
				LightDataProvider.getLightPropertyName(propertyName, lightId)
			);
		}
		
		private function getLightProperty(lightId : uint, propertyName : String) : *
		{
			return _scene.bindings.getProperty(
				LightDataProvider.getLightPropertyName(propertyName, lightId)
			);
		}
	}
}
