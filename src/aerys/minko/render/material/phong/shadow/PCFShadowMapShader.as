package aerys.minko.render.material.phong.shadow
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderSettings;
	import aerys.minko.render.shader.part.DiffuseShaderPart;
	import aerys.minko.render.shader.part.animation.VertexAnimationShaderPart;
	import aerys.minko.render.shader.part.phong.depth.LinearDepthFromLightShaderPart;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.TriangleCulling;
	import aerys.minko.render.material.phong.PhongProperties;
	
	public class PCFShadowMapShader extends Shader
	{
		private var _vertexAnimationPart	: VertexAnimationShaderPart;
		private var _diffusePart			: DiffuseShaderPart;
        
		private var _lightId				: uint;
		private var _clipspacePosition		: SFloat;
		private var _depthShaderPart		: LinearDepthFromLightShaderPart;
		private var _side					: uint;
		
		public function PCFShadowMapShader(lightId		: uint,
										   priority		: Number,
										   renderTarget	: RenderTarget,
										   side			: uint = 4)
		{
			super(renderTarget, priority);
			
			_vertexAnimationPart 	= new VertexAnimationShaderPart(this);
			_depthShaderPart		= new LinearDepthFromLightShaderPart(this);
			_diffusePart 			= new DiffuseShaderPart(this);
			_lightId 				= lightId;
			_side					= side;
		}
		
		override protected function initializeSettings(settings : ShaderSettings) : void
		{
			super.initializeSettings(settings);

			settings.blending = Blending.OPAQUE;
			settings.enabled = meshBindings.getProperty(PhongProperties.CAST_SHADOWS, false);
			settings.triangleCulling = meshBindings.getProperty(
                BasicProperties.TRIANGLE_CULLING, TriangleCulling.BACK
            );
		}
		
		override protected function getVertexPosition() : SFloat
		{
			return _depthShaderPart.getVertexPosition(
				_lightId,
				_vertexAnimationPart.getAnimatedVertexPosition(),
				_side
			);
		}
		
		override protected function getPixelColor() : SFloat
		{
			if (meshBindings.propertyExists(BasicProperties.ALPHA_THRESHOLD))
			{
				var diffuse			: SFloat	= _diffusePart.getDiffuseColor();
				var alphaThreshold 	: SFloat 	= meshBindings.getParameter(
					BasicProperties.ALPHA_THRESHOLD, 1
				);
				
				kill(subtract(0.5, lessThan(diffuse.w, alphaThreshold)));
			}
			
			return pack(_depthShaderPart.getPixelColor(_lightId));
		}
	}
}
