package aerys.minko.render.shader.node.operation.manipulation
{
	import aerys.minko.render.renderer.state.Blending;
	import aerys.minko.render.renderer.state.BlendingSource;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.builtin.Add;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	
	public class Blend extends Dummy
	{
		override public function get size() : uint
		{
			return 4;
		}
		
		public function Blend(sourceColor	: INode,
							  destColor		: INode,
							  blendingMode	: uint)
		{
			var sourceBlendingMode	: uint = blendingMode & 0x0000FFFF;
			var destBlendingMode	: uint = (blendingMode & 0xFFFF0000) >>> 16;
			
			var sourceFactor : INode	= blendFactor(sourceColor, destColor, sourceBlendingMode);
			var destFactor	 : INode	= blendFactor(sourceColor, destColor, destBlendingMode);
			
			var source	: INode = blend(sourceColor, sourceFactor);
			var dest	: INode = blend(destColor, destFactor);
			var result	: INode = new Add(source, dest);
			
			var final	: INode = new Combine(
				new Extract(result, Components.XYZ),
				new Extract(destColor, Components.W)
			);
			
			super(final);
			
			if (sourceColor.size != 4 && destColor.size != 4)
				throw new Error('Invalid parameters. Source and destination colors must be of size 4.');
			
			else if (sourceColor.size != 4)
				throw new Error('Invalid parameters. Source color must be of size 4.');
			
			else if (destColor.size != 4)
				throw new Error('Invalid parameters. Destination color must be of size 4.');
		}
		
		private function blend(color : INode, factor : INode) : INode
		{
			var constFactor : Constant = factor as Constant;
			if (constFactor && constFactor.size == 1)
			{
				if (constFactor.constants[0] == 1)
					return color;
				else if (constFactor.constants[0] == 0)
					return new Constant(0);
				else
					return new Multiply(color, constFactor);
			}
			else
			{
				return new Multiply(color, factor);
			}
		}
		
		private function blendFactor(sourceColor	: INode,
									 destColor		: INode,
									 blendingMode	: uint) : INode
		{
			switch (blendingMode)
			{
				case BlendingSource.DESTINATION_ALPHA:
					return new Extract(destColor, Components.W);
					
				case BlendingSource.DESTINATION_COLOR:
					return destColor;
					
				case BlendingSource.ONE:
					return new Constant(1);
					
				case BlendingSource.ONE_MINUS_DESTINATION_ALPHA:
					return new Substract(
						new Constant(1.),
						new Extract(destColor, Components.W)
					);
					
				case BlendingSource.ONE_MINUS_DESTINATION_COLOR:
					return new Substract(
						new Constant(1.),
						destColor
					);
					
				case BlendingSource.ONE_MINUS_SOURCE_ALPHA:
					return new Substract(
						new Constant(1.),
						new Extract(sourceColor, Components.W)
					);
					
				case BlendingSource.SOURCE_ALPHA:
					return new Extract(sourceColor, Components.W);
					
				case BlendingSource.SOURCE_COLOR:
					return sourceColor;
					
				case BlendingSource.ZERO: 
					return new Constant(0);
					
				default: 
					throw new Error('Invalid BlendingMode.');
			}
		}
		
		override public function toString() : String
		{
			return "Blending";
		}
	}
}