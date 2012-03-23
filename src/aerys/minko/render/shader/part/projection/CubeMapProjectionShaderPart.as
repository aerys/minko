package aerys.minko.render.shader.part.projection
{
	import aerys.minko.render.shader.PassTemplate;
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
		private static const VIEW_MATRICES : Vector.<Matrix4x4> = Vector.<Matrix4x4>([
			Matrix4x4.lookAt(Vector4.ZERO, Vector4.X_AXIS,		Vector4.Y_AXIS),	 // look at positive x
			Matrix4x4.lookAt(Vector4.ZERO, new Vector4(-1, 0, 0),	Vector4.Y_AXIS),	 // look at negative x
			Matrix4x4.lookAt(Vector4.ZERO, Vector4.Y_AXIS,		new Vector4(0, 0, -1)),	 // look at positive y
			Matrix4x4.lookAt(Vector4.ZERO, new Vector4(0, -1, 0),	Vector4.Z_AXIS),	 // look at negative y
			Matrix4x4.lookAt(Vector4.ZERO, Vector4.Z_AXIS,		Vector4.Y_AXIS),	 // look at positive z
			Matrix4x4.lookAt(Vector4.ZERO, new Vector4(0, 0, -1),	Vector4.Y_AXIS),	 // look at negative z
		]);
		
		private var _side : uint;
		
		public function CubeMapProjectionShaderPart(main : PassTemplate, side : uint)
		{
			super(main);
			
			side = _side;
		}
		
		public function projectVector(vector	: SFloat, 
									  target	: Rectangle	= null, 
									  zNear		: Number	= 5, 
									  zFar		: Number	= 1000) : SFloat
		{
			var projectionMatrix	: Matrix4x4 = Matrix4x4.perspectiveFoV(Math.PI / 4, 1, zNear, zFar);
			var worldToScreenMatrix	: Matrix4x4 = Matrix4x4.multiply(projectionMatrix, VIEW_MATRICES[_side]);
			var worldToScreen		: SFloat	= new SFloat(worldToScreenMatrix);
			
			return multiply4x4(vector, worldToScreen);
		}
		
		public function unprojectVector(projectedVector	: SFloat, 
										source			: Rectangle = null) : SFloat
		{
			throw new Error('Not yet implemented');
		}
	}
}
