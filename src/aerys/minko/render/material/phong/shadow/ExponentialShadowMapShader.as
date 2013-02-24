package aerys.minko.render.material.phong.shadow
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderSettings;
	import aerys.minko.render.shader.part.DiffuseShaderPart;
	import aerys.minko.render.shader.part.animation.VertexAnimationShaderPart;
	import aerys.minko.render.shader.part.phong.depth.IDepthFromLightShaderPart;
	import aerys.minko.render.shader.part.phong.depth.LinearDepthFromLightShaderPart;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.TriangleCulling;
	import aerys.minko.render.material.phong.PhongProperties;
	
	public final class ExponentialShadowMapShader extends Shader
	{
		private var _vertexAnimationPart	: VertexAnimationShaderPart;
		private var _diffusePart			: DiffuseShaderPart;
		private var _depthShaderPart		: IDepthFromLightShaderPart	= null;
		private var _lightId				: uint;
		private var _clipspacePosition		: SFloat;
		private var _lightSpacePosition		: SFloat;
		private var _face					: uint;
		
		public function ExponentialShadowMapShader(lightId		: uint,
												   face			: uint,
												   priority		: Number,
												   renderTarget	: RenderTarget)
		{
			super(renderTarget, priority);
			
			_lightId				= lightId;
			_face					= face;
			_vertexAnimationPart	= new VertexAnimationShaderPart(this);
			_diffusePart 			= new DiffuseShaderPart(this);
		}
		
		override protected function initializeSettings(settings : ShaderSettings) : void
		{
			super.initializeSettings(settings);
			
			settings.blending			= Blending.OPAQUE;
			settings.enabled 			= meshBindings.getProperty(PhongProperties.CAST_SHADOWS, true);
			settings.triangleCulling	= meshBindings.getProperty(
				BasicProperties.TRIANGLE_CULLING, TriangleCulling.BACK
			);
		}
		
		private function createDepthShaderPart() : void
		{
			_depthShaderPart = new LinearDepthFromLightShaderPart(this);
		}
		
		override protected function getVertexPosition() : SFloat
		{
			createDepthShaderPart();
			
			return _depthShaderPart.getVertexPosition(_lightId, _vertexAnimationPart.getAnimatedVertexPosition(), _face);
		}
		
		/**
		 * @see http://http.developer.nvidia.com/GPUGems3/gpugems3_ch08.html
		 */
		override protected function getPixelColor() : SFloat
		{
			if (_depthShaderPart == null)
			{
				createDepthShaderPart();
			}
			
			var depth				: SFloat	= _depthShaderPart.getPixelColor(_lightId);
			
			if (meshBindings.propertyExists(BasicProperties.ALPHA_THRESHOLD))
			{
				var diffuse			: SFloat	= _diffusePart.getDiffuseColor();
				var alphaThreshold 	: SFloat 	= meshBindings.getParameter(
					BasicProperties.ALPHA_THRESHOLD, 1
				);
				kill(subtract(0.5, lessThan(diffuse.w, alphaThreshold)));
			}
			
			return pack(depth);
		}
	}
}