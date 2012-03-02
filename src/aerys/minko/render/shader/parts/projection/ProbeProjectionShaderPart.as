package aerys.minko.render.shader.parts.projection
{
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.ShaderPart;
	
	import flash.geom.Rectangle;
	
	/**
	 * @author Romain Gilliotte
	 */	
	public class ProbeProjectionShaderPart extends ShaderPart implements IProjectionShaderPart
	{
		public function ProbeProjectionShaderPart(main : ActionScriptShader)
		{
			super(main);
		}

		public function projectVector(vector	: SFloat,
									  target	: Rectangle = null,
									  zNear		: Number	= 0,
									  zFar		: Number	= 1000) : SFloat
		{
			target ||= new Rectangle(0, 0, 1, 1);
			
			vector = normalize(vector);
			
			var m				: SFloat = rsqrt(multiply(2, add(vector.z, 1)));
			var projectedVector : SFloat = SFloat(vector.xy).scaleBy(m);
			
			// on se ramene au rectangle desire (le resultat de la premiere operation nous donne un truc entre -1 et 1)
			projectedVector
				.incrementBy(1)
				.scaleBy(0.5)
				.scaleBy(float2(target.width, target.height))
				.incrementBy(float2(target.x, target.y));
			
			return projectedVector;
		}
		
		public function unprojectVector(projectedVector : SFloat,
										source			: Rectangle = null) : SFloat
		{
			source ||= new Rectangle(0, 0, 1, 1);
			
			// on se ramene au rectangle entre -1 et 1
			projectedVector = projectedVector
				.decrementBy(float2(source.x, source.y))
				.scaleBy(float2(1 / source.width, 1 / source.height))
				.scaleBy(2)
				.decrementBy(1);
			
			// on retrouve les coordonnes du vecteur initial en resolvant l'equation
			//
			// xt = x / sqrt(2*(z+1))
			// yt = y / sqrt(2*(z+1))
			// x^2 + y^2 + z^2 - 1 = 0
			// 
			// => z^2 + z(2*xt^2 + 2*yt^2) + 2*xt^2 + 2*yt^2 - 1 = 0 (quand z != -1)
			
			var xt2PlusYt2	: SFloat = dotProduct2(projectedVector, projectedVector);
			var delta		: SFloat = multiply(4, add(1, multiply(xt2PlusYt2, subtract(xt2PlusYt2, 2))));
			
			// z = (-b + sqrt(delta)) / (2a)
			var z			: SFloat = multiply(1 / 2, add(negate(xt2PlusYt2), sqrt(delta)));
			var xy			: SFloat = multiply(projectedVector, sqrt(multiply(2, add(z, 1))));
			
			return float3(xy, z);
		}
	}
}