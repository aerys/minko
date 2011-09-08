package aerys.minko.render.shader.compiler.visitor.writer
{
	import aerys.minko.render.shader.compiler.allocator.Allocator;
	import aerys.minko.render.shader.compiler.register.RegisterMask;
	import aerys.minko.render.shader.compiler.register.RegisterSwizzling;
	import aerys.minko.render.shader.compiler.register.RegisterType;
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.AbstractConstant;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.Sampler;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.render.shader.node.operation.manipulation.VariadicExtract;
	
	public class AbstractWriter implements IShaderNodeVisitor
	{
		protected var _isVertexShader	: Boolean;
		
		protected var _stack 			: Vector.<INode>;
		protected var _visited			: Vector.<INode>;
		
		protected var _attrAllocator	: Allocator;
		protected var _varyingAllocator	: Allocator;
		
		protected var _fsTmpAllocator	: Allocator;
		protected var _fsConstAllocator	: Allocator;
		
		protected var _vsTmpAllocator	: Allocator;
		protected var _vsConstAllocator	: Allocator;
		
		public function AbstractWriter(attrAllocator	: Allocator,
									   fsTmpAllocator	: Allocator, 
									   varyingAllocator	: Allocator, 
									   fsConstAllocator	: Allocator, 
									   vsTmpAllocator	: Allocator, 
									   vsConstAllocator	: Allocator)
		{
			_stack				= new Vector.<INode>();
			_visited			= new Vector.<INode>();
			
			_attrAllocator		= attrAllocator;
			_varyingAllocator	= varyingAllocator;
			
			_fsTmpAllocator		= fsTmpAllocator;
			_fsConstAllocator	= fsConstAllocator;
			
			_vsTmpAllocator		= vsTmpAllocator
			_vsConstAllocator	= vsConstAllocator;
		}
		
		public function visit(node : INode):void
		{
			if (_visited.indexOf(node) !== -1)
				return;
			
			_visited.push(node);
			_stack.push(node);
			
			if (node is Interpolate)
				visitInterpolateNode(node as Interpolate);
				
			else if (node is Extract)
				visitExtractNode(node as Extract);
				
			else if (node is VariadicExtract)
				visitVariadicExtractNode(node as VariadicExtract);
				
			else if (node is Combine)
				visitCombineNode(node as Combine);
				
			else if (node is AbstractOperation)
				visitAbstractOperationNode(node as AbstractOperation);
			
			_stack.pop();
		}
		
		private function visitInterpolateNode(node : Interpolate) : void 
		{
			if (_isVertexShader)
			{
				node.accept(this);
				
				outputMoveIntruction(node.arg1, node);
			}
		}
		
		private function visitExtractNode(node : Extract) : void
		{
			node.accept(this);
		}
		
		private function visitVariadicExtractNode(node : VariadicExtract) : void
		{
			node.accept(this);
		}
		
//		protected function visitCombineNode(node : Combine) : void
//		{
//			throw new Error('Must be overriden.');
//		}
		
		protected function visitCombineNode(node : Combine) : void
		{
			node.accept(this);
			
			// we know node is not writing to op/oc, because it's a combine node.
			var resultLocalOffset : uint = getAllocatorFor(node).getLocalOffset(node); 
			
			var mask1 : uint;
			mask1 = RegisterMask.createContinuous(node.arg1.size);
			mask1 = RegisterMask.offset(mask1, resultLocalOffset);
			outputMoveIntruction(node.arg1, node, mask1);
			
			var mask2 : uint;
			mask2 = RegisterMask.createContinuous(node.arg2.size);
			mask2 = RegisterMask.offset(mask2, resultLocalOffset + node.arg1.size);
			outputMoveIntruction(node.arg2, node, mask2);
		}
		
		private function visitAbstractOperationNode(node : AbstractOperation) : void
		{
			node.accept(this);
			
			outputCommonInstruction(node);
		}
		
		protected function registerWrite(node		: INode, 
										 andMask	: uint = 0xf) : void
		{
			if (node is Interpolate)
			{
				outputDestination(
					_varyingAllocator.getId(node),					// registerNumber 
					andMask & _varyingAllocator.getWriteMask(node), // writeMask
					RegisterType.VARYING							// registerType
				);
			}
			else if (_stack.length < 2)
			{
				outputDestination(0, RegisterMask.XYZW, RegisterType.OUTPUT);
			}	
			else if (node is AbstractOperation)
			{
				var operationNode:AbstractOperation = node as AbstractOperation;
				if (_isVertexShader)
				{
					outputDestination(
						_vsTmpAllocator.getId(operationNode),
						andMask & _vsTmpAllocator.getWriteMask(operationNode),
						RegisterType.TEMPORARY
					);
				}
				else
				{
					outputDestination(
						_fsTmpAllocator.getId(operationNode),
						andMask & _fsTmpAllocator.getWriteMask(operationNode),
						RegisterType.TEMPORARY
					);
				}
			}
			else
				throw new Error('Unknown Register type');
		}
		
		protected function registerRead(argument	: INode,
										operation	: AbstractOperation) : void
		{
			// usused fields must be written in bytecode, so we output some 8 zero bytes.
			if (argument == null)
			{
				outputEmptySource();
			}
			else if (argument is Sampler)
			{
				var sampler : Sampler = argument as Sampler;
				outputSamplerSource(sampler.samplerId, sampler.dimension, sampler.wrapping, sampler.filter, sampler.mipmap);
			}
			else
			{
				var swizzle : uint = getReadSwizzle(argument, operation);
				while (argument is Extract)
					argument = Extract(argument).arg1;
				
				if (argument is VariadicExtract)
				{
					registerReadFromVariadicExtract(argument as VariadicExtract, operation, swizzle);
				}
				else
				{
					registerReadFromCommon(argument, operation, swizzle);
				}
			}
		}
		
		protected function registerReadFromCommon(argument	: INode,
												operation	: AbstractOperation,
												swizzle		: uint) : void
		{
			// ask the allocator where we are to write the result of the node we are processing
			var argAllocator : Allocator = getAllocatorFor(argument);
			
			outputCommonSource(
				argAllocator.getId(argument),				// registerNumber
				0,											// indirectOffset
				swizzle,									// swizzle
				getRegisterTypeFor(argument),				// registerType
				0,											// indexRegisterType
				0,											// indexRegisterComponentSelect
				true										// direct
			);
		}
		
		private function registerReadFromVariadicExtract(argument	: VariadicExtract,
														 operation	: AbstractOperation,
														 swizzle	: uint) : void
		{
			// ask the allocator where we are to write the result of the node we are processing
			var tableNode		: INode		= argument.arg2;
			var tableAllocator	: Allocator	= getAllocatorFor(tableNode);
			
			
			var indexNode		: INode		= argument.arg1;
			var indexSwizzle	: uint		= getReadSwizzle(indexNode, null);
			while (indexNode is Extract)
				indexNode = Extract(indexNode).arg1;
			
			var indexAllocator	: Allocator	= getAllocatorFor(indexNode);
			
			outputCommonSource(
				indexAllocator.getId(indexNode),			// registerNumber
				tableAllocator.getId(tableNode),			// indirectOffset
				swizzle,									// swizzle
				getRegisterTypeFor(tableNode),				// registerType
				getRegisterTypeFor(indexNode),				// indexRegisterType
				indexSwizzle & 0x3,							// indexRegisterComponentSelect
				false										// direct
			);
		}
		
		protected function getReadSwizzle(argument : INode, operation : INode) : uint
		{
			var swizzle : uint;
			if (argument.size > 4)
			{
				// if we are reading some matrix data, we just put the default value
				// it's not going to be read anyway.
				
				// if argument.size is equal to 4, we have to compute it: the swizzle could
				// be WZXY, XYYZ, or any combination.
				swizzle = RegisterSwizzling.XYZW;	
			}
			else
			{
				// create a continuous swizzle matching the size of the data we want to read.
				swizzle = RegisterSwizzling.createContinuous(argument.size);
				
				// If the operation is component wise, we have to offset the 
				// swizzle of the argument to match the offset the result will be written at.
				if (operation is IComponentWiseOperation && _stack.length > 1)
				{
					var opAllocator	: Allocator = getAllocatorFor(operation);
					var writeOffset	: uint		= opAllocator.getLocalOffset(operation);
					
					swizzle = RegisterSwizzling.writeOffset(writeOffset, swizzle);
				}
				
				// retrieve the effective swizzle of the argument, after manual swizzles
				var extractNode		: Extract	= argument as Extract;
				while (extractNode != null)
				{
					swizzle	= RegisterSwizzling.combine(swizzle, extractNode.swizzle);
					
					argument		= extractNode.arg1;
					extractNode		= extractNode.arg1 as Extract;
				}
				
				// retrieve the concerned allocator, we are sure this is non-null, because it is an
				// argument, and cannot be the root of the tree.
				if (!(argument is VariadicExtract))
				{
					var argAllocator	: Allocator = getAllocatorFor(argument);
					var readOffset		: uint		= argAllocator.getLocalOffset(argument);
					
					swizzle = RegisterSwizzling.readOffset(readOffset, swizzle);
				}
			}
			
			return swizzle;
		}
		
		private function getRegisterTypeFor(node : INode) : uint
		{
			if (node is Extract)
				throw new Error('Extract nodes are not allocated to memory.');
			
			if (node is VariadicExtract)
				throw new Error('VariadicExtract nodes are not allocated to memory.');
			
			if (node is Attribute)
				return RegisterType.ATTRIBUTE;
			
			if (node is AbstractConstant)
				return RegisterType.CONSTANT;
			
			if (node is Sampler)
				return RegisterType.SAMPLER;
			
			if (node is Interpolate)
				return RegisterType.VARYING;
			
			if (node is AbstractOperation)
				return RegisterType.TEMPORARY;
			
			throw new Error('Unknown node type');
		}
		
		protected function getAllocatorFor(node : INode) : Allocator
		{
			if (node is Extract)
				throw new Error('Extract nodes are not allocated to memory.');
			
			if (node is VariadicExtract)
				throw new Error('VariadicExtract nodes are not allocated to memory.');
			
			if (node is Sampler)
				throw new Error('No allocator for samplers.');
			
			if (node is Attribute)
				return _attrAllocator;
			
			if (node is AbstractConstant)
				return _isVertexShader ? _vsConstAllocator : _fsConstAllocator;
			
			if (node is Interpolate)
				return _varyingAllocator;
			
			if (node is AbstractOperation)
				return _isVertexShader ? _vsTmpAllocator : _fsTmpAllocator;
			
			throw new Error('No allocator was found for node : ' + node.toString() + '. Go fix your shader.');
		}
		
		
		protected function outputMoveIntruction(from : INode, to : AbstractOperation, andMask : uint = 0xf) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function outputCommonInstruction(node : AbstractOperation) : void 
		{
			throw new Error('Must be overriden');	   
		}
		
		protected function outputDestination(registerNumber	: uint,
											 writeMask		: uint,
											 registerType	: uint) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function outputSamplerSource(index		: uint,
											   dimension	: uint,
											   wrapping		: uint,
											   filter		: uint,
											   mipmap		: uint) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function outputCommonSource(registerNumber				: uint,
											  indirectOffset				: uint,
											  swizzle						: uint,
											  registerType					: uint,
											  indexRegisterType				: uint,
											  indexRegisterComponentSelect	: uint,
											  direct						: Boolean) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function outputEmptySource() : void
		{
			throw new Error('Must be overriden');
		}
		
		
	}
}