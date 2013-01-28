package aerys.minko.render.material.phong
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderSettings;
	import aerys.minko.render.shader.part.DiffuseShaderPart;
	import aerys.minko.render.shader.part.animation.VertexAnimationShaderPart;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.light.SpotLight;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.TriangleCulling;
	
	public class PCFShadowMapShader extends Shader
	{
		private var _vertexAnimationPart	: VertexAnimationShaderPart;
		private var _diffusePart			: DiffuseShaderPart;
        
		private var _lightId				: uint;
		private var _clipspacePosition		: SFloat;
		
		public function PCFShadowMapShader(lightId		: uint,
											  priority		: Number,
											  renderTarget	: RenderTarget)
		{
			super(renderTarget, priority);
			
			_lightId = lightId;
			
			_vertexAnimationPart = new VertexAnimationShaderPart(this);
			_diffusePart = new DiffuseShaderPart(this);
		}
		
		override protected function initializeSettings(settings : ShaderSettings) : void
		{
			super.initializeSettings(settings);
			
			settings.blending = Blending.NORMAL;
			settings.enabled = meshBindings.getProperty(PhongProperties.CAST_SHADOWS, true);
			settings.triangleCulling = meshBindings.getProperty(
                BasicProperties.TRIANGLE_CULLING, TriangleCulling.BACK
            );
		}
		
		override protected function getVertexPosition() : SFloat
		{
			var lightTypeName		: String = LightDataProvider.getLightPropertyName(
                'type', _lightId
            );
			var worldToScreenName	: String = LightDataProvider.getLightPropertyName(
                'worldToScreen', _lightId
            );
			
			var lightType			: uint	 = sceneBindings.getProperty(lightTypeName);
			var worldToScreen		: SFloat = sceneBindings.getParameter(worldToScreenName, 16);
			var vertexPosition		: SFloat = localToWorld(
                _vertexAnimationPart.getAnimatedVertexPosition()
            );
			
			_clipspacePosition = multiply4x4(vertexPosition, worldToScreen);
			
			if (lightType == SpotLight.LIGHT_TYPE)
				return float4(
                    _clipspacePosition.xy,
                    multiply(_clipspacePosition.z, _clipspacePosition.w),
                    _clipspacePosition.w
                );
			else
				return _clipspacePosition;
		}
		
		/**
		 * @see http://www.mvps.org/directx/articles/linear_z/linearz.htm Linear Z-buffering
		 */		
		override protected function getPixelColor() : SFloat
		{
			var iClipspacePosition	: SFloat = interpolate(_clipspacePosition);
			
			if (meshBindings.propertyExists(BasicProperties.ALPHA_THRESHOLD))
			{
				var diffuse			: SFloat	= _diffusePart.getDiffuseColor();
				var alphaThreshold 	: SFloat 	= meshBindings.getParameter(
					BasicProperties.ALPHA_THRESHOLD, 1
				);
				
				kill(subtract(0.5, lessThan(diffuse.w, alphaThreshold)));
			}
			
			return pack(iClipspacePosition.z);
		}
	}
}
