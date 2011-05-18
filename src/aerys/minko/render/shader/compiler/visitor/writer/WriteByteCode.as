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
	import aerys.minko.render.shader.node.operation.IAlignedOperation;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class WriteByteCode implements IShaderNodeVisitor
	{
		protected static const OPCODE_MOVE	: uint = 0x0;
		
		protected var _bytecode			: ByteArray;
		protected var _isVertexShader	: Boolean;
		
		protected var _stack 	: Vector.<INode>;
		protected var _visited	: Vector.<INode>;
		
		protected var _attrAllocator	: Allocator;
		protected var _varyingAllocator	: Allocator;
		
		protected var _fsTmpAllocator	: Allocator;
		protected var _fsConstAllocator	: Allocator;
		
		protected var _vsTmpAllocator	: Allocator;
		protected var _vsConstAllocator	: Allocator;
		
		public function WriteByteCode(attrAllocator		: Allocator,
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
		
		public function processVertexShader(vertexOps : Vector.<INode>) : ByteArray
		{
			_bytecode = new ByteArray();
			_bytecode.endian = Endian.LITTLE_ENDIAN;
			
			_isVertexShader = true;
			writeHeader();
			for each (var vertexOp:INode in vertexOps)
				visit(vertexOp);
			
			return _bytecode;
		}
		
		public function processFragmentShader(color : INode) : ByteArray
		{
			_bytecode = new ByteArray();
			_bytecode.endian = Endian.LITTLE_ENDIAN;
			
			_isVertexShader = false;
			writeHeader();
			visit(color);
			
			return _bytecode;
		}
		
		protected function writeHeader() : void
		{
			_bytecode.writeByte(0xa0 );						// tag version
			_bytecode.writeUnsignedInt(1);					// AGAL version, big endian, bit pattern will be 0x01000000
			_bytecode.writeByte(0xa1);						// tag program id
			_bytecode.writeByte(_isVertexShader ? 0 : 1);	// vertex or fragment
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
				visitSwizzleNode(node as Extract);
				
			else if (node is Combine)
				visitCombineNode(node as Combine);
				
			else if (node is AbstractOperation)
				visitAbstractOperationNode(node as AbstractOperation);
			
			_stack.pop();
		}
		
		
		protected function visitInterpolateNode(node : Interpolate) : void 
		{
			if (_isVertexShader)
			{
				node.accept(this);
				
				_bytecode.writeUnsignedInt(OPCODE_MOVE);
				registerWrite(node); 
				registerRead(node.arg1, node);
				registerRead(null, node);
				
			}
		}
		
		protected function visitSwizzleNode(node : Extract) : void
		{
			node.accept(this);
		}
		
		protected function visitCombineNode(node : Combine) : void
		{
			node.accept(this);
			
			// we know node is not writing to op/oc, because it's a combine node.
			var resultLocalOffset : uint = getAllocatorFor(node).getLocalOffset(node); 
			
			var mask1 : uint, mask2 : uint;
			
			mask1 = RegisterMask.createContinuous(node.arg1.size);
			mask1 = RegisterMask.offset(mask1, resultLocalOffset);
			
			_bytecode.writeUnsignedInt(node.opCode);
			registerWrite(node, mask1)
			registerRead(node.arg1, node);
			registerRead(null, node);
			
			mask2 = RegisterMask.createContinuous(node.arg2.size);
			mask2 = RegisterMask.offset(mask2, resultLocalOffset + node.arg1.size);
			
			_bytecode.writeUnsignedInt(node.opCode);
			registerWrite(node, mask2)
			registerRead(node.arg2, node)
			registerRead(null, node);
		}
		
		protected function visitAbstractOperationNode(node : AbstractOperation) : void
		{
			node.accept(this);
			
			_bytecode.writeUnsignedInt(node.opCode);
			registerWrite(node);
			registerRead(node.arg1, node);
			registerRead(node.arg2, node);
		}
		
		protected function registerWrite(node		: INode, 
										 andMask	: uint = 0xf) : void
		{
			if (node is Interpolate)
			{
				_bytecode.writeShort(_varyingAllocator.getId(node));
				_bytecode.writeByte(andMask & _varyingAllocator.getWriteMask(node));
				_bytecode.writeByte(RegisterType.VARYING);
			}
			else if (_stack.length < 2)
			{
				_bytecode.writeShort(0);
				_bytecode.writeByte(RegisterMask.XYZW);
				_bytecode.writeByte(RegisterType.OUTPUT);
			}
			else if (node is AbstractOperation)
			{
				var operationNode:AbstractOperation = node as AbstractOperation;
				if (_isVertexShader)
				{
					_bytecode.writeShort(_vsTmpAllocator.getId(operationNode));
					_bytecode.writeByte(andMask & _vsTmpAllocator.getWriteMask(operationNode));
					_bytecode.writeByte(RegisterType.TEMPORARY);
				}
				else
				{
					_bytecode.writeShort(_fsTmpAllocator.getId(operationNode));
					_bytecode.writeByte(andMask & _fsTmpAllocator.getWriteMask(operationNode));
					_bytecode.writeByte(RegisterType.TEMPORARY);
				}
			}
		}
		
		protected function registerRead(argument	: INode,
										operation	: AbstractOperation) : void
		{
			// usused fields must be written in bytecode, so we output some 8 zero bytes.
			if (argument == null)
			{
				_bytecode.writeUnsignedInt(0);
				_bytecode.writeUnsignedInt(0);
				return;
			}
			
			// Samplers do no use dynamic allocation, so we treat them appart
			if (argument is Sampler)
			{
				var samplerArgument : Sampler = Sampler(argument);
				
				_bytecode.writeShort(samplerArgument.samplerId);
				_bytecode.writeShort(0x0);
				_bytecode.writeByte(RegisterType.SAMPLER);
				
				_bytecode.writeByte(samplerArgument.dimension << 4);
				_bytecode.writeByte(samplerArgument.wrapping << 4);
				_bytecode.writeByte(samplerArgument.mipmap | (samplerArgument.filter << 4));
				
				return;
			}
			
			var swizzle : uint;
			if (argument.size > 4)
			{
				// if we are reading some matrix weird data, we just put some default value
				// here, it's not going to be read anyway.
				swizzle = RegisterSwizzling.XYZW;	
			}
			else
			{
				// create a continuous swizzle matching the size of the data we want to read.
				swizzle = RegisterSwizzling.createContinuous(argument.size);
				
				// If the operation is component wise, we have to offset the 
				// swizzle of the argument to match the offset the result will be written at.
				if (operation is IComponentWiseOperation)
				{
					if (opAllocator != null && _stack.length > 1)
					{
						var writeOffset	: uint		= 0;
						var opAllocator	: Allocator = getAllocatorFor(operation);
						
						writeOffset = opAllocator.getLocalOffset(operation);
						swizzle = RegisterSwizzling.writeOffset(writeOffset, swizzle);
					}
					// else, we are at the root of the graph, and for some obscure reason,
					// when the Program3D gets executed, an error is raised if we do something 
					// like this:
					//		mul o[cp].xy, whatever, whatever
					//		sub o[cp].zw, whatever, whatever
					// 
					// This behavious is patched by adding a RootWrapper node at the root of the graph.
					// (which is just a simple "mov" node)
				}
				
				// retrieve the effective swizzle of the argument, after manual swizzles
				var swizzleNode		: Extract	= argument as Extract;
				while (swizzleNode != null)
				{
					swizzle	= RegisterSwizzling.combine(swizzle, swizzleNode.swizzle);
					
					argument		= swizzleNode.arg1;
					swizzleNode		= swizzleNode.arg1 as Extract;
				}
			}
			
			// retrieve the concerned allocator, we are sure this is non-null, because it is an
			// argument, and cannot be the root of the tree.
			var argAllocator	: Allocator = getAllocatorFor(argument);
			
			// ask the allocator where we are to write the result of the node we are processing
			var registerType	: uint		= getRegisterTypeFor(argument);
			var registerId		: uint		= argAllocator.getId(argument);
			var readOffset		: uint		= argAllocator.getLocalOffset(argument);
			
			swizzle = RegisterSwizzling.readOffset(readOffset, swizzle);
			
			_bytecode.writeShort(registerId);	// register id
			_bytecode.writeByte(0);				// indirect offset
			_bytecode.writeByte(swizzle);		// swizzling
			_bytecode.writeByte(registerType);	// register type
			_bytecode.writeByte(0);				// direct-indirect
			_bytecode.writeByte(0);				// index register component select
			_bytecode.writeByte(0);				// index register type
		}
		
		protected function getRegisterTypeFor(node : INode) : uint
		{
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
			
			return null;
		}
		
		protected function getAllocatorFor(node : INode) : Allocator
		{
			if (node is Extract)
				throw new Error('Swizzle nodes are not allocated to memory.');
			
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
		
	}
}