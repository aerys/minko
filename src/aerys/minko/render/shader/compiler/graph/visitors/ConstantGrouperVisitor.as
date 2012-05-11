package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Interpolate;

	public class ConstantGrouperVisitor extends AbstractVisitor
	{
		private var _vertexConstants	: Vector.<Constant>;
		private var _fragmentConstants	: Vector.<Constant>;
		
		public function ConstantGrouperVisitor()
		{
			super();
			
			_vertexConstants	= new Vector.<Constant>();
			_fragmentConstants	= new Vector.<Constant>();
		}
		
		override protected function finish() : void
		{
			mergeConstantsIn(_vertexConstants);
			mergeConstantsIn(_fragmentConstants);
			
			_vertexConstants.length		= 0;
			_fragmentConstants.length	= 0;
			
			super.finish();
		}
		
		private function mergeConstantsIn(constants : Vector.<Constant>) : void
		{
			var numConstants : uint = constants.length;
			
			for (var constantId1 : int = 1; constantId1 < numConstants; ++constantId1)
			{
				var constant1		: Constant			= constants[constantId1];
				var constantData1	: Vector.<Number>	= Constant(constant1).value;
				var constantSize1	: uint				= constantData1.length;
				
				for (var constantId2 : int = 0; constantId2 < constantId1; ++constantId2)
				{
					var constant2		: Constant			= constants[constantId2];
					var constantData2	: Vector.<Number>	= Constant(constant2).value;
					var constantSize2	: uint				= constantData2.length;
					
					// count shared numbers between the two constants we are comparing
					var numMatches		: uint				= countMatchesBetweenConstants(constantData1, constantData2);
					
					// if we don't gain any space by grouping, or if it is not possible, we skip that one
					if (numMatches == 0 || constantSize1 + constantSize2 - numMatches > 4)
						continue;
					
					// group them and build components
					var constantComponent1		: uint				= 0;
					var constantComponent2		: uint				= 0;
					var newConstData			: Vector.<Number>	= new Vector.<Number>();
					var constantComponentId		: uint				= 0;
					
					var constantComponentId1	: uint = 0;
					var constantComponentId2	: uint = 0;
					while (constantData1.length != 0 && constantData2.length != 0)
					{
						if (constantData1[0] < constantData2[0])
						{
							constantComponent1 |= constantComponentId++ << (8 * constantComponentId1++);
							newConstData.push(constantData1.shift());
						}
						else if (constantData1[0] > constantData2[0])
						{
							constantComponent2 |= constantComponentId++ << (8 * constantComponentId2++);
							newConstData.push(constantData2.shift());
						}
						else // constantData1[0] = constantData2[0]
						{
							constantComponent1 |= constantComponentId << (8 * constantComponentId1++);
							constantComponent2 |= constantComponentId << (8 * constantComponentId2++);
							newConstData.push(constantData1.shift());
							constantData2.shift()
							++constantComponentId;
						}
					}
					
					while (constantData1.length != 0)
					{
						constantComponent1 |= constantComponentId++ << (8 * constantComponentId1++);
						newConstData.push(constantData1.shift());
					}
					
					while (constantData2.length != 0)
					{
						constantComponent2 |= constantComponentId++ << (8 * constantComponentId2++);
						newConstData.push(constantData2.shift());
					}
					
					// left pad the components with 4's (@see Components class)
					for (; constantComponentId1 < 4; ++constantComponentId1)
						constantComponent1 |= 4 << (8 * constantComponentId1);
					
					for (; constantComponentId2 < 4; ++constantComponentId2)
						constantComponent2 |= 4 << (8 * constantComponentId2);
					
					
					// replace the affected constants with the new one.
					var newConstant		: Constant	= new Constant(newConstData);
					
					replaceInParentsAndSwizzle(constant1, newConstant, constantComponent1);
					replaceInParentsAndSwizzle(constant2, newConstant, constantComponent2);
					
					constants.splice(constantId1, 1); // (we know constant2 < constant1)
					constants.splice(constantId2, 1);
					constants.push(newConstant);
					--numConstants;
					
					// start over everything.
					constantId1 = 0;
					break;
				}
			}
		}
		
		private function countMatchesBetweenConstants(value1 : Vector.<Number>, 
													  value2 : Vector.<Number>) : uint
		{
			var size1		: uint = value1.length;
			var size2		: uint = value2.length;
			var numMatches	: uint = 0;
			
			for (var constantComponentId1 : uint = 0; constantComponentId1 < size1; ++constantComponentId1)
				for (var constantComponentId2 : uint = 0; constantComponentId2 < size2; ++constantComponentId2)
					if (value1[constantComponentId1] == value2[constantComponentId2])
						++numMatches;
			
			return numMatches;
		}
		
		override protected function visitTraversable(node : AbstractNode, isVertexShader : Boolean) : void
		{
			if (node is Interpolate)
				isVertexShader = true;
			
			visitArguments(node, isVertexShader);
		}
		
		override protected function visitConstant(constant : Constant, isVertexShader : Boolean) : void
		{
			if (isVertexShader)
				_vertexConstants.push(constant);
			else
				_fragmentConstants.push(constant);
		}
	}
}