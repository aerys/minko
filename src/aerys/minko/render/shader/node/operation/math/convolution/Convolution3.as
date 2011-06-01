package aerys.minko.render.shader.node.operation.math.convolution
{
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.leaf.Sampler;
	import aerys.minko.render.shader.node.operation.builtin.Add;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Texture;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.math.Sum;
	
	/**
	 * 
	 * 
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 */	
	public class Convolution3 extends Dummy
	{
		
		/**
		 * 
		 * @param source
		 * @param uv
		 * @param pixelSize
		 * @param matrix
		 */
		public function Convolution3(source		: Sampler, 
									uv			: INode, 
									pixelSize	: Number, 
									matrix		: Vector.<Number>)
		{
			// define some constants
			var pixelOffsets	: Constant = new Constant(- pixelSize, 0, pixelSize);
			
			// compute each surounding pixel uv
			var uvs : Array = [
				new Add(uv, new Extract(pixelOffsets, Components.XX)),
				new Add(uv, new Extract(pixelOffsets, Components.XY)),
				new Add(uv, new Extract(pixelOffsets, Components.XZ)),
				new Add(uv, new Extract(pixelOffsets, Components.YX)),
				uv,
				new Add(uv, new Extract(pixelOffsets, Components.YZ)),
				new Add(uv, new Extract(pixelOffsets, Components.ZX)),
				new Add(uv, new Extract(pixelOffsets, Components.ZY)),
				new Add(uv, new Extract(pixelOffsets, Components.ZZ)),
			];
			
			// apply convolution matrix on each component.
			var result : Sum = new Sum();
			for (var i : int = 0; i < 9; ++i)
			{
				if (matrix[i] == 0)
					continue;
				
				result.addTerm(
					new Multiply(
						new Constant(matrix[i]),
						new Texture(uvs[i], source)
					)
				);
			}
			
			super(result);
			
			if (matrix.length != 9)
				throw new Error('Convolution matrix must be of size 9 (3x3)');
		}
	}
}
