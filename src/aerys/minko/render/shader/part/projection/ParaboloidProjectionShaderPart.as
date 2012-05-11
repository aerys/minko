package aerys.minko.render.shader.part.projection
{
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.part.ShaderPart;
	
	import flash.geom.Rectangle;
	
	/**
	 * @author Romain Gilliotte
	 */	
	public class ParaboloidProjectionShaderPart extends ShaderPart implements IProjectionShaderPart
	{
		private static const CLIPSPACE_RECTANGLE : Rectangle = new Rectangle(-1, -1, 2, 2);
		
		private var _front : Boolean;
		
		public function ParaboloidProjectionShaderPart(main 	: Shader,
													   front 	: Boolean)
		{
			super(main);
			
			_front = front;
		}
		
		public function projectVector(vector	: SFloat,
									  target	: Rectangle,
									  zNear		: Number	= 0,
									  zFar		: Number	= 60) : SFloat
		{
			vector = vector.xyz;
			if (!_front)
				vector = multiply(vector, float3(1, 1, -1));

			var length : SFloat = length(vector);
			
			vector = divide(vector, length);
			vector = float3(
				transform2DCoordinates(divide(vector.xy, add(vector.z, 1)), CLIPSPACE_RECTANGLE, target), 
				divide(subtract(length, zNear), zFar - zNear)
			);
			
			return vector;
		}
		
		public function unprojectVector(projectedVector	: SFloat,
										source			: Rectangle) : SFloat
		{
			projectedVector = transform2DCoordinates(projectedVector, source, CLIPSPACE_RECTANGLE);
			
			var xt2_yt2	: SFloat = dotProduct2(projectedVector.xy, projectedVector.xy);
			var z		: SFloat = negate(divide(subtract(xt2_yt2, 1), add(xt2_yt2, 1)));
			var xy		: SFloat = multiply(projectedVector.xy, add(z, 1));
			
			if (!_front)
				z = negate(z);
			
			return float3(xy, z);
		}
		
	}
}
