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
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.binding.DataBindings;
	
	import flash.display.BitmapData;
	
	public class AbstractShadowMappingEffect extends Effect
	{
		use namespace minko_lighting;
		
		private const SHADOW_FACTORIES : Vector.<Function> = new <Function>[
			manageNoShadowing,				// 0: ShadowMappingType.NONE
			manageMatrixShadowing,			// 1: ShadowMappingType.MATRIX
			manageDualParaboloidShadowing,	// 2: ShadowMappingType.DUAL_PARABOLOID
			manageCubicShadowing			// 3: ShadowMappingType.CUBIC
		];
		
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
			_renderingPass		= renderingShader;
			_watchedProperties	= new Vector.<String>();
			_scene				= scene;
			
            scene.enterFrame.add(sceneEnterFrameHandler);
		}
		
		private function sceneEnterFrameHandler(scene		: Scene,
                                                viewport	: Viewport,
                                                destination	: BitmapData, 
                                                timer		: uint) : void
		{
            scene.enterFrame.remove(sceneEnterFrameHandler);
            
			updatePasses();
		}
		
		private function propertyChangedHandler(sceneBindings	: DataBindings, 
												propertyName	: String,
												oldValue		: Object,
												newValue		: Object) : void
		{
            scene.enterFrame.add(sceneEnterFrameHandler);
		}
		
		private function updatePasses() : void
		{
			var passes			: Vector.<Shader>	= new <Shader>[];
			var sceneBindings	: DataBindings		= _scene.bindings;
			var shader			: Shader			= null;
			var renderTarget	: RenderTarget		= null;
			
			while (_watchedProperties.length != 0)
				sceneBindings.removeCallback(_watchedProperties.pop(), propertyChangedHandler);
			
			for (var lightId : uint = 0; ; ++lightId)
			{
				if (!lightPropertyExists(lightId, 'type'))
					break ;
				
				var shadowCastingPropertyName : String = LightDataProvider.getLightPropertyName(
					'shadowCastingType',
					lightId
				);
				
				_watchedProperties.push(shadowCastingPropertyName);
				sceneBindings.addCallback(shadowCastingPropertyName, propertyChangedHandler);
				
				if (sceneBindings.propertyExists(shadowCastingPropertyName))
					SHADOW_FACTORIES[sceneBindings.getProperty(shadowCastingPropertyName)](lightId, passes);
			}
			
			passes.push(_renderingPass);
			
			setPasses(passes);
		}
		
		private function manageNoShadowing(lightId : uint, passes : Vector.<Shader>) : void
		{
			// nothing to do here, no extra rendering is necessary
		}
		
		private function manageMatrixShadowing(lightId : uint, passes : Vector.<Shader>) : void
		{
			var textureResource : TextureResource	= getLightProperty(lightId, 'shadowMap');
			var renderTarget	: RenderTarget		= new RenderTarget(
				textureResource.width, textureResource.height, textureResource, 0, 0xffffffff
			);
			
			passes.push(new MatrixShadowMapShader(lightId, lightId + 1, renderTarget));
		}
		
		private function manageDualParaboloidShadowing(lightId : uint, passes : Vector.<Shader>) : void
		{
			var frontTextureResource : TextureResource	= getLightProperty(lightId, 'shadowMapDPFront');
			var backTextureResource	 : TextureResource	= getLightProperty(lightId, 'shadowMapDPBack');
			var size				 : uint				= frontTextureResource.width;
			var frontRenderTarget	 : RenderTarget		= new RenderTarget(size, size, frontTextureResource, 0, 0xffffffff);
			var backRenderTarget	 : RenderTarget		= new RenderTarget(size, size, backTextureResource, 0, 0xffffffff);
			
			passes.push(
				new ParaboloidShadowMapShader(lightId, true, lightId + 0.5, frontRenderTarget),
				new ParaboloidShadowMapShader(lightId, false, lightId + 1, backRenderTarget)
			);
		}
		
		private function manageCubicShadowing(lightId : uint, passes : Vector.<Shader>) : void
		{
			var textureResource	: CubeTextureResource	= getLightProperty(lightId, 'shadowMapCube');
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
