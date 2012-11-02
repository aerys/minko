package aerys.minko.render.material.phong
{
	import aerys.minko.ns.minko_lighting;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderInstance;
	import aerys.minko.render.shader.ShaderSettings;
	import aerys.minko.render.shader.part.DiffuseShaderPart;
	import aerys.minko.render.shader.part.animation.VertexAnimationShaderPart;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.TriangleCulling;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public class CubeShadowMapShader extends Shader
	{
		use namespace minko_lighting;
		
		private static const VIEW_MATRICES : Vector.<Matrix4x4> = new <Matrix4x4>[
			new Matrix4x4().view(Vector4.ZERO, Vector4.X_AXIS,		Vector4.Y_AXIS),		// look at positive x
			new Matrix4x4().view(Vector4.ZERO, new Vector4(-1, 0, 0),	Vector4.Y_AXIS),		// look at negative x
			new Matrix4x4().view(Vector4.ZERO, Vector4.Y_AXIS,		new Vector4(0, 0, -1)),	// look at positive y
			new Matrix4x4().view(Vector4.ZERO, new Vector4(0, -1, 0),	Vector4.Z_AXIS),		// look at negative y
			new Matrix4x4().view(Vector4.ZERO, Vector4.Z_AXIS,		Vector4.Y_AXIS),		// look at positive z (identity)
			new Matrix4x4().view(Vector4.ZERO, new Vector4(0, 0, -1),	Vector4.Y_AXIS),		// look at negative z
		];
		
		private var _vertexAnimationPart	: VertexAnimationShaderPart;
		private var _diffusePart			: DiffuseShaderPart;
		private var _lightId				: uint;
		private var _side					: uint;
		private var _positionFromLight		: SFloat;
		
		public function CubeShadowMapShader(lightId			: uint, 
											side			: uint, 
											priority		: Number,
											renderTarget	: RenderTarget)
		{
			super(renderTarget, priority);
			
			_vertexAnimationPart	= new VertexAnimationShaderPart(this);
			_diffusePart			= new DiffuseShaderPart(this);
			_lightId				= lightId;
			_side					= side;
		}
		
		override protected function initializeSettings(passConfig : ShaderSettings) : void
		{
			passConfig.triangleCulling	= meshBindings.getConstant(BasicProperties.TRIANGLE_CULLING, TriangleCulling.BACK);
			passConfig.blending			= Blending.NORMAL;
			passConfig.enabled			= meshBindings.getConstant(PhongProperties.CAST_SHADOWS, true);
		}
		
		override protected function getVertexPosition() : SFloat
		{
			// retrieve matrices
			var worldToLocalName	: String	= LightDataProvider.getLightPropertyName('worldToLocal', _lightId);
			var projectionName		: String	= LightDataProvider.getLightPropertyName('projection', _lightId);
			var worldToLocal		: SFloat	= sceneBindings.getParameter(worldToLocalName, 16);
			var viewMatrix			: Matrix4x4	= VIEW_MATRICES[_side];
			var projection			: SFloat	= sceneBindings.getParameter(projectionName, 16);
			
			// compute position
			var position			: SFloat	= localToWorld(_vertexAnimationPart.getAnimatedVertexPosition());
			var positionFromLight	: SFloat	= multiply4x4(position, worldToLocal);
			var positionOnScreen	: SFloat	= multiply4x4(multiply4x4(positionFromLight, viewMatrix), projection);
			
			// apply transformation for linear z buffering
			positionOnScreen = float4(
				positionOnScreen.xy, 
				multiply(positionOnScreen.z, positionOnScreen.w),
				positionOnScreen.w
			); 
			
			// interpolate position from light
			_positionFromLight = interpolate(positionFromLight);
			
			return positionOnScreen;
		}
		
		override protected function getPixelColor() : SFloat
		{
			// retrieve zNear and zFar
			var zNearName	: String	= LightDataProvider.getLightPropertyName('shadowZNear', _lightId);
			var zFarName	: String	= LightDataProvider.getLightPropertyName('shadowZFar', _lightId);
			var zNear		: SFloat	= sceneBindings.getParameter(zNearName, 1);
			var zFar		: SFloat	= sceneBindings.getParameter(zFarName, 1);
			
			// compute distance
			var distance	: SFloat	= length(_positionFromLight.xyz);
			
			if (meshBindings.propertyExists(BasicProperties.ALPHA_THRESHOLD))
			{
				var diffuse			: SFloat	= _diffusePart.getDiffuseColor();
				var alphaThreshold 	: SFloat 	= meshBindings.getParameter('alphaThreshold', 1);
				
				kill(subtract(0.5, lessThan(diffuse.w, alphaThreshold)));
			}
			
			return pack(divide(subtract(distance, zNear), subtract(zFar, zNear)));
		}
	}
}
