package aerys.minko.render.shader.part.phong.depth
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.ShaderPart;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.scene.node.light.SpotLight;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public final class LinearDepthFromLightShaderPart extends ShaderPart implements IDepthFromLightShaderPart
	{
		private static const VIEW_MATRICES : Vector.<Matrix4x4> = new <Matrix4x4>[
			new Matrix4x4().view(Vector4.ZERO, Vector4.X_AXIS,		    Vector4.Y_AXIS),		// look at positive x
			new Matrix4x4().view(Vector4.ZERO, new Vector4(-1, 0, 0),	Vector4.Y_AXIS),		// look at negative x
			new Matrix4x4().view(Vector4.ZERO, Vector4.Y_AXIS,		    new Vector4(0, 0, -1)),	// look at positive y
			new Matrix4x4().view(Vector4.ZERO, new Vector4(0, -1, 0),	Vector4.Z_AXIS),		// look at negative y
			new Matrix4x4().view(Vector4.ZERO, Vector4.Z_AXIS,		    Vector4.Y_AXIS),		// look at positive z (identity)
			new Matrix4x4().view(Vector4.ZERO, new Vector4(0, 0, -1),	Vector4.Y_AXIS),		// look at negative z
		];
		
		private var _lightSpacePosition		: SFloat;
		private var _clipspacePosition		: SFloat;
		
		public function LinearDepthFromLightShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function getVertexPosition(lightId : uint, vPosition : SFloat, face : uint = 4) : SFloat
		{
			var lightTypeName		: String 	= LightDataProvider.getLightPropertyName('type', lightId);
			var projectionName		: String 	= LightDataProvider.getLightPropertyName('projection', lightId);
			var lightType			: uint		= sceneBindings.getProperty(lightTypeName);
			var projection			: SFloat	= sceneBindings.getParameter(projectionName, 16);
			var viewMatrix			: Matrix4x4	= VIEW_MATRICES[face];
			var worldToLightName	: String	= LightDataProvider.getLightPropertyName('worldToLocal', lightId);
			var worldToLight		: SFloat	= sceneBindings.getParameter(worldToLightName, 16);
			var position			: SFloat	= vPosition;
			var worldPosition		: SFloat	= localToWorld(position);
			_lightSpacePosition					= multiply4x4(worldPosition, worldToLight);
			_clipspacePosition					= multiply4x4(multiply4x4(_lightSpacePosition, viewMatrix), projection);
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
		
		public function getPixelColor(lightId : uint) : SFloat
		{
			var zFarName			: String	= LightDataProvider.getLightPropertyName('shadowZFar', lightId);
			var zFar				: SFloat	= sceneBindings.getParameter(zFarName, 1);
			var zNearName			: String	= LightDataProvider.getLightPropertyName('shadowZNear', lightId);
			var zNear				: Number	= sceneBindings.getProperty(zNearName, 0);
			var lightSpacePosition	: SFloat	= interpolate(_lightSpacePosition);
			var depth				: SFloat	= length(lightSpacePosition);
			depth								= divide(depth, subtract(zFar, zNear));
			
			return depth;
		}
		
		
		public function getUV(lightId : uint, fsWorldPosition : SFloat) : SFloat
		{
			var lightTypeName			: String	= LightDataProvider.getLightPropertyName('type', lightId);
			var lightType				: uint		= sceneBindings.getProperty(lightTypeName, lightId);
			var transformPropertyName	: String	= lightType == PointLight.LIGHT_TYPE ? 'worldToLocal' : 'worldToUV';
			var transformName			: String	= LightDataProvider.getLightPropertyName(transformPropertyName, lightId);
			var transform				: SFloat 	= sceneBindings.getParameter(transformName, 16);
			var coordinates				: SFloat	= multiply4x4(fsWorldPosition, transform);
			coordinates								= divide(coordinates, coordinates.w);
			
			return coordinates;
		}
		
		public function getDepthForAttenuation(lightId : uint, fsWorldPosition : SFloat) : SFloat
		{
			var worldToLightName	: String	= LightDataProvider.getLightPropertyName('worldToLocal', lightId);
			var zFarName			: String	= LightDataProvider.getLightPropertyName('shadowZFar', lightId);
			var zNearName			: String	= LightDataProvider.getLightPropertyName('shadowZNear', lightId);
			var worldToLight		: SFloat	= sceneBindings.getParameter(worldToLightName, 16);
			var posLightSpace		: SFloat	= multiply4x4(fsWorldPosition, worldToLight);
			var zFar				: SFloat	= sceneBindings.getParameter(zFarName, 1);
			var zNear				: Number	= sceneBindings.getProperty(zNearName, 0);
			var depth				: SFloat	= length(posLightSpace);
			depth								= divide(depth, subtract(zFar, zNear));

			return depth;
		}
	}
}