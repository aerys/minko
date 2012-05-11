package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;

	public class ConstantPackerVisitor extends AbstractVisitor
	{
		public function ConstantPackerVisitor()
		{
			super();
		}
		
		override protected function visitTraversable(node : AbstractNode, isVertexShader : Boolean) : void
		{
			visitArguments(node, true);
		}
		
		override protected function visitConstant(constant:Constant, isVertexShader:Boolean):void
		{
			var constData				: Vector.<Number>	= constant.value;
			var constDataLength			: uint				= constData.length;
			var constDataLengthMinus1	: uint				= constDataLength - 1;
			var hasDonePermutation		: Boolean			= false;
			
			// no compression for nodes of size greater than 4.
			if (constDataLength == 1 || constDataLength > 4)
				return;
			
			// we are now going to sort and remove duplicates from the constant.
			// all modifications we make on the constData are repercuted in the opposite
			// in the components value.
			var newConstData		: Vector.<Number>	= constData.slice();
			
			// bubble sort
			for (var i : int = 0; i < constDataLengthMinus1; ++i)
			{
				if (newConstData[i] > newConstData[i + 1])
				{
					var tmpFloat : Number = newConstData[i];
					newConstData[i] = newConstData[i + 1];
					newConstData[i + 1] = tmpFloat;
					
					hasDonePermutation = true;
					
					if (i != 0)
						i = i - 2;
				}
			}
			
			// remove duplicates
			for (i = 0; i < newConstData.length - 1; ++i)
			{
				if (newConstData[i] == newConstData[i + 1])
				{
					newConstData.splice(i, 1);
					--i;
				}
			}
			
			// replace it if needed
			if (newConstData.length != constDataLength || hasDonePermutation)
			{
				// compute components
				var componentVal : uint = 0;
				
				for (i = 3; i >= constDataLength; --i)
					componentVal = (componentVal << 8) | 4;
				
				for (; i >= 0 ; --i)
					componentVal = (componentVal << 8) | newConstData.indexOf(constData[i]);
				
				// replace the constant
				constant.value = newConstData;
				swizzleParents(constant, componentVal);
			}
		}
	}
}