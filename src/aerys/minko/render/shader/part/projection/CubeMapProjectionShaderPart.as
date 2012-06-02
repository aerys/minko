package aerys.minko.render.shader.part.projection
{
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.part.ShaderPart;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Rectangle;
	
	/**
	 * The CubeMapProjectionShaderPart provides the methods to project vectors
	 * on a cube map.
	 * 
	 * @author Romain Gilliotte
	 */	
	public class CubeMapProjectionShaderPart extends ShaderPart implements IProjectionShaderPart
	{
		private static const VIEW_MATRICES : Vector.<Matrix4x4> = new <Matrix4x4>[
			new Matrix4x4().view(Vector4.ZERO, Vector4.X_AXIS,		Vector4.Y_AXIS),	 // look at positive x
			new Matrix4x4().view(Vector4.ZERO, new Vector4(-1, 0, 0),	Vector4.Y_AXIS),	 // look at negative x
			new Matrix4x4().view(Vector4.ZERO, Vector4.Y_AXIS,		new Vector4(0, 0, -1)),	 // look at positive y
			new Matrix4x4().view(Vector4.ZERO, new Vector4(0, -1, 0),	Vector4.Z_AXIS),	 // look at negative y
			new Matrix4x4().view(Vector4.ZERO, Vector4.Z_AXIS,		Vector4.Y_AXIS),	 // look at positive z
			new Matrix4x4().view(Vector4.ZERO, new Vector4(0, 0, -1),	Vector4.Y_AXIS),	 // look at negative z
		];
		
		private var _side : uint;
		
		public function CubeMapProjectionShaderPart(main : Shader, side : uint)
		{
			super(main);
			
			side = _side;
		}
		
		public function projectVector(vector	: SFloat, 
									  target	: Rectangle, 
									  zNear		: Number	= 5, 
									  zFar		: Number	= 1000) : SFloat
		{
			var projection		: Matrix4x4 = new Matrix4x4().perspectiveFoV(Math.PI / 4, 1, zNear, zFar);
			var worldToScreen	: Matrix4x4 = new Matrix4x4().copyFrom(VIEW_MATRICES[_side])
															 .append(projection);
			
			return multiply4x4(vector, worldToScreen);
		}
		
		public function unprojectVector(projectedVector	: SFloat, 
										source			: Rectangle) : SFloat
		{
			throw new Error('Not yet implemented');
		}
	}
}
