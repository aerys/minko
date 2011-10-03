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
	 * @author Romain Gilliotte
	 */	
	public class Convolution5 extends Dummy
	{
		
		/**
		 * 
		 * @param source
		 * @param uv
		 * @param pixelSize
		 * @param matrix
		 */
		public function Convolution5(source		: Sampler, 
									 uv			: INode, 
									 pixelSize	: Number, 
									 matrix		: Vector.<Number>)
		{
			// define some constants
			var pixelOffsets		: Constant = new Constant(- pixelSize, 0, pixelSize);
			var pixelOffsetsKnight	: Constant = new Constant(- 2 * pixelSize, 2 * pixelSize, pixelSize, -pixelSize);
			var pixelOffsetsDouble	: Constant = new Constant(- 2 * pixelSize, 0, 2 * pixelSize);
			
			// compute each surounding pixel uv
			var uvs : Array = [
				// center
				uv,
				
				// first ring
				new Add(uv, new Extract(pixelOffsets, Components.XX)),
				new Add(uv, new Extract(pixelOffsets, Components.XY)),
				new Add(uv, new Extract(pixelOffsets, Components.XZ)),
				new Add(uv, new Extract(pixelOffsets, Components.YX)),
				new Add(uv, new Extract(pixelOffsets, Components.YZ)),
				new Add(uv, new Extract(pixelOffsets, Components.ZX)),
				new Add(uv, new Extract(pixelOffsets, Components.ZY)),
				new Add(uv, new Extract(pixelOffsets, Components.ZZ)),
				
				// second ring corners and sides
				new Add(uv, new Extract(pixelOffsetsDouble, Components.XX)),
				new Add(uv, new Extract(pixelOffsetsDouble, Components.XY)),
				new Add(uv, new Extract(pixelOffsetsDouble, Components.XZ)),
				new Add(uv, new Extract(pixelOffsetsDouble, Components.YX)),
				new Add(uv, new Extract(pixelOffsetsDouble, Components.YZ)),
				new Add(uv, new Extract(pixelOffsetsDouble, Components.ZX)),
				new Add(uv, new Extract(pixelOffsetsDouble, Components.ZY)),
				new Add(uv, new Extract(pixelOffsetsDouble, Components.ZZ)),
				
				// second ring knights
				new Add(uv, new Extract(pixelOffsetsKnight, Components.XZ)),
				new Add(uv, new Extract(pixelOffsetsKnight, Components.XW)),
				new Add(uv, new Extract(pixelOffsetsKnight, Components.YZ)),
				new Add(uv, new Extract(pixelOffsetsKnight, Components.YW)),
				new Add(uv, new Extract(pixelOffsetsKnight, Components.ZX)),
				new Add(uv, new Extract(pixelOffsetsKnight, Components.ZY)),
				new Add(uv, new Extract(pixelOffsetsKnight, Components.WX)),
				new Add(uv, new Extract(pixelOffsetsKnight, Components.WY)),
			];
			
			// apply convolution matrix on each component.
			var result : Sum = new Sum();
			for (var i : int = 0; i < 25; ++i)
			{
				if (matrix[i] == 0)
					continue;
				
				var term : INode = new Multiply(
					new Constant(matrix[i]),
					new Texture(uvs[i], source)
				);
				
				result.addTerm(term);
			}
			
			super(result);
			
			if (matrix.length != 25)
				throw new Error('Convolution matrix must be of size 25 (5x5)');
		}
	}
}
