package aerys.minko.render.shader.part.depth
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.ShaderPart;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.scene.node.light.SpotLight;
	
	public final class ProjectedDepthShaderPart extends ShaderPart implements IDepthShaderPart
	{
		private var _clipspacePosition		: SFloat;
		
		public function ProjectedDepthShaderPart(main:Shader)
		{
			super(main);
		}
		
		public function getVertexPosition(lightId:uint, vPosition:SFloat, face : uint = 5):SFloat
		{
			var lightTypeName		: String 	= LightDataProvider.getLightPropertyName('type', lightId);
			var worldToScreenName	: String 	= LightDataProvider.getLightPropertyName('worldToScreen', lightId);
			var lightType			: uint		= sceneBindings.getProperty(lightTypeName);
			var worldToScreen		: SFloat 	= sceneBindings.getParameter(worldToScreenName, 16);
			var position			: SFloat	= vPosition;
			var worldPosition		: SFloat	= localToWorld(position);
			_clipspacePosition					= multiply4x4(worldPosition, worldToScreen);
			
			if (lightType == SpotLight.LIGHT_TYPE || lightType == PointLight.LIGHT_TYPE)
			{
				return float4(
					_clipspacePosition.xy,
					multiply(_clipspacePosition.z, _clipspacePosition.w),
					_clipspacePosition.w
				);
			}
			else
				return _clipspacePosition;
		}
		
		public function getUV(lightId : uint, worldPosition : SFloat) : SFloat
		{
			var worldToUVName		: String	= LightDataProvider.getLightPropertyName('worldToUV', lightId);
			var worldToUV			: SFloat 	= sceneBindings.getParameter(worldToUVName, 16);
			var coordinates			: SFloat	= multiply4x4(worldPosition, worldToUV);
			coordinates							= divide(coordinates, coordinates.w);
			
			return coordinates;
		}

		
		public function getPixelColor(lightId:uint):SFloat
		{
			var clipSpacePosition	: SFloat	= interpolate(_clipspacePosition);
			var depth				: SFloat	= divide(clipSpacePosition.z, clipSpacePosition.w);
			
			return depth;
		}
		
		public function getDepthForAttenuation(lightId:uint, worldPosition:SFloat):SFloat
		{
			var worldToUVName	: String	= LightDataProvider.getLightPropertyName('worldToUV', lightId);
			var worldToUV		: SFloat 	= sceneBindings.getParameter(worldToUVName, 16);
			var uv 				: SFloat	= multiply4x4(worldPosition, worldToUV);
			uv								= divide(uv, uv.w);
			
			return uv;
		}
	}
}