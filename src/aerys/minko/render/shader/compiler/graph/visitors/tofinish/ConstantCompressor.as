package aerys.minko.render.shader.compiler.graph.visitors.tofinish
{
	
	import flash.utils.Dictionary;
	
	/**
	 * This visitor sorts constants, inserts extract nodes, removes duplicates, 
	 * and try to merge some of them to save some constant registers.
	 * This is mainly useful in the fragment shader, because memory there is
	 * very limited.
	 * 
	 * For example:
	 * 	If the following constants are present into the shader graph:
	 * 			c0 = (0, 1, 3)
	 * 			c1 = (2, 0, 0, 0)
	 * 			c2 = (3, 2, 0)
	 * 
	 *  They will be tranformed to
	 * 			cc0 = (0, 1, 2, 3)
	 * 			c0 = cc0.xyw
	 * 			c1 = cc0.zxxx
	 * 			c2 = cc0.wzx
	 * 
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 */
	public class ConstantCompressor
	{
		/*
		private var _visited			: Vector.<INode>
		private var _stack				: Vector.<INode>;
		
		private var _floatGain			: uint;
		private var _constants			: Vector.<Constant>;
		private var _constantToParents	: Dictionary;
		
		public function ConstantCompressor()
		{
			_floatGain = 0;
		}
		
		public function work(outputPosition : INode,
							 interpolates	: Vector.<INode>,
							 outputColor	: INode,
							 kills			: Vector.<INode>) : void
		{
			
			reset();
			explore(outputPosition);
			for each (var interpolate : INode in interpolates)
				explore(Interpolate(interpolate).arg1);
			
			pack();
			
			groupConstants();
			
			reset();
			explore(outputColor);
			for each (var kill : INode in kills)
				explore(kill);
			pack();
			groupConstants();
			kills = null;
		}
		
		private function reset() : void
		{
			_visited			= new Vector.<INode>();
			_stack				= new Vector.<INode>();
			_constants			= new Vector.<Constant>();
			_constantToParents	= new Dictionary();
		}
		
		private function explore(node : INode) : void
		{
			if (_visited.indexOf(node) != -1)
			{
				if (!(node is Constant))
					return;
			}
			else
				_visited.push(node);
			
			_stack.push(node);
			
			if (node is Constant && node.size <= 4)
			{
				if (_constantToParents[node] == undefined)
				{
					_constantToParents[node] = new Vector.<INode>();
					_constants.push(node);
				}
				
				_constantToParents[node].push(_stack[_stack.length - 2]);
			}
			else if (node is Interpolate)
			{
				// do nothing: constants are handled separately between fragment and vertex shader
				// so it makes no sense grouping them
			}
			else if (node is AbstractOperation)
			{
				var opNode : AbstractOperation = AbstractOperation(node);
				
				explore(opNode.arg1);
				if (opNode.arg2 != null)
					explore(opNode.arg2);
			}
			else if (node is Dummy)
			{
				var dummyNode : Dummy = Dummy(node);
				explore(dummyNode.node);
			}
			
			_stack.pop();
		}
		
		private function pack() : void
		{
			var numConstants : uint = _constants.length;
			
			for (var constantId : int = 0; constantId < numConstants; ++constantId)
			{
				var constNode				: Constant	= _constants[constantId];
				var constDataLength			: uint		= constNode.constants.length;
				var constDataLengthMinus1	: uint		= constDataLength - 1;
				var hasDonePermutation		: Boolean	= false;
				
				// no compression for nodes of size greater than 4.
				if (constDataLength == 1 || constDataLength > 4)
					continue;
				
				// we are now going to sort and remove duplicates from the constant.
				// all modifications we make on the constData are repercuted in the opposite
				// in the components value.
				var constData			: Vector.<Number>	= constNode.constants;
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
						++_floatGain;
					}
				}
				
				// replace it if needed
				if (newConstData.length != constDataLength || hasDonePermutation)
				{
					// compute components
					var componentVal : uint = 0;
					
					for (i = 3; i >= constDataLength; --i)
						componentVal = (componentVal << 4) | 4;
					
					for (; i >= 0 ; --i)
						componentVal = (componentVal << 4) | newConstData.indexOf(constData[i]);
					
					// replace the constant
					var newConstant		: Constant			= new Constant(newConstData);
					var extractChild	: Extract			= new Extract(newConstant, componentVal);
					var parent			: AbstractOperation;
					
					for each (parent in _constantToParents[constNode])
						parent.replaceChildren(constNode, extractChild);
					
					delete _constantToParents[constNode];
					
					_constantToParents[newConstant] = Vector.<INode>([extractChild]);
					_constants[constantId] = newConstant;
				}
			}
		}
		
		private function groupConstants() : void
		{
			var numConstants : uint = _constants.length;
			
			for (var constantId1 : int = 1; constantId1 < numConstants; ++constantId1)
			{
				var constant1		: Constant			= _constants[constantId1];
				var constantData1	: Vector.<Number>	= Constant(constant1).constants.slice();
				var constantSize1	: uint				= constantData1.length;
				
				for (var constantId2 : int = 0; constantId2 < constantId1; ++constantId2)
				{
					var constant2		: Constant			= _constants[constantId2];
					
					// count shared numbers between the two constants we are comparing
					var numMatches		: uint				= 0;
					var constantData2	: Vector.<Number>	= Constant(constant2).constants.slice();
					var constantSize2	: uint				= constantData2.length;
					
					for (var constantComponentId1 : uint = 0; constantComponentId1 < constantSize1; ++constantComponentId1)
						for (var constantComponentId2 : uint = 0; constantComponentId2 < constantSize2; ++constantComponentId2)
							if (constantData1[constantComponentId1] == constantData2[constantComponentId2])
								++numMatches;
					
					// if we don't gain any space by grouping, or if it is not possible, we skip that one
					if (numMatches == 0 || constantSize1 + constantSize2 - numMatches > 4)
						continue;
					
					// group them and build components
					var constantComponent1	: uint				= 0;
					var constantComponent2	: uint				= 0;
					var newConstData		: Vector.<Number>	= new Vector.<Number>();
					var constantComponentId	: uint				= 0;
					
					constantComponentId1 = constantComponentId2 = 0;
					while (constantData1.length != 0 && constantData2.length != 0)
					{
						if (constantData1[0] < constantData2[0])
						{
							constantComponent1 |= constantComponentId++ << (4 * constantComponentId1++);
							newConstData.push(constantData1.shift());
						}
						else if (constantData1[0] > constantData2[0])
						{
							constantComponent2 |= constantComponentId++ << (4 * constantComponentId2++);
							newConstData.push(constantData2.shift());
						}
						else // constantData1[0] = constantData2[0]
						{
							constantComponent1 |= constantComponentId << (4 * constantComponentId1++);
							constantComponent2 |= constantComponentId << (4 * constantComponentId2++);
							newConstData.push(constantData1.shift());
							constantData2.shift()
							++constantComponentId;
							++_floatGain;
						}
					}
					
					while (constantData1.length != 0)
					{
						constantComponent1 |= constantComponentId++ << (4 * constantComponentId1++);
						newConstData.push(constantData1.shift());
					}
					
					while (constantData2.length != 0)
					{
						constantComponent2 |= constantComponentId++ << (4 * constantComponentId2++);
						newConstData.push(constantData2.shift());
					}
					
					// left pad the components with 4's (@see Components class)
					for (; constantComponentId1 < 4; ++constantComponentId1)
						constantComponent1 |= 4 << (4 * constantComponentId1);
					
					for (; constantComponentId2 < 4; ++constantComponentId2)
						constantComponent2 |= 4 << (4 * constantComponentId2);
					
					
					// replace the affected constants with the new one.
					var newConstant		: Constant	= new Constant(newConstData);
					var extractChild1	: Extract	= new Extract(newConstant, constantComponent1);
					var extractChild2	: Extract	= new Extract(newConstant, constantComponent2);
					
					var parent			: AbstractOperation;
					
					for each (parent in _constantToParents[constant1])
						parent.replaceChildren(constant1, extractChild1);
					
					for each (parent in _constantToParents[constant2])
						parent.replaceChildren(constant2, extractChild2);
					
					delete _constantToParents[constant1];
					delete _constantToParents[constant2];
					
					_constantToParents[newConstant] = Vector.<INode>([extractChild1, extractChild2]);
					
					_constants.splice(constantId1, 1); // (we know constant2 < constant1)
					_constants.splice(constantId2, 1);
					_constants.push(newConstant);
					--numConstants;
					
					// start over everything.
					constantId1 = 0;
					break;
				}
			}
		}
		*/
	}
}