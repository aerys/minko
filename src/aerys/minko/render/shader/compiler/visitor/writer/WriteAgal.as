package aerys.minko.render.shader.compiler.visitor.writer
{
	import aerys.minko.render.shader.compiler.allocator.Allocator;
	import aerys.minko.render.shader.compiler.register.RegisterMask;
	import aerys.minko.render.shader.compiler.register.RegisterSwizzling;
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
	
	public class WriteAgal implements IShaderNodeVisitor
	{
		protected static const _SAMPLER_FILTER_STRINGS:Vector.<String> =
			Vector.<String>(['nearest', 'linear']);
		
		protected static const _SAMPLER_MIPMAP_STRINGS:Vector.<String> =
			Vector.<String>(['mipnone', 'mipnearest', 'miplinear']);
		
		protected static const _SAMPLER_WRAPPING_STRINGS:Vector.<String> =
			Vector.<String>(['clamp', 'repeat']);
		
		protected static const _SAMPLER_DIMENSION_STRINGS:Vector.<String> = 
			Vector.<String>(['2d', 'cube', '3d']);
		
		protected var _asmCode			: String;
		protected var _isVertexShader	: Boolean;

		protected var _stack 	: Vector.<INode>;
		protected var _visited	: Vector.<INode>;
		
		protected var _attrAllocator	: Allocator;
		protected var _varyingAllocator	: Allocator;

		protected var _fsTmpAllocator	: Allocator;
		protected var _fsConstAllocator	: Allocator;
		
		protected var _vsTmpAllocator	: Allocator;
		protected var _vsConstAllocator	: Allocator;
		
		public function WriteAgal(attrAllocator		: Allocator,
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
		
		public function processVertexShader(vertexOps : Vector.<INode>) : String
		{
			_asmCode = '';
			_isVertexShader = true;
			for each (var vertexOp : INode in vertexOps)
				visit(vertexOp);
			
			return _asmCode;
		}
		
		public function processFragmentShader(color : INode) : String
		{
			_asmCode = '';
			_isVertexShader = false;
			visit(color);
			return _asmCode;
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
				
				_asmCode += 'mov ' + registerWrite(node) + ', ' 
					+ registerRead(node.arg1, node) + "\t\t// interpolate\n";
			}
		}
		
		protected function visitSwizzleNode(node : Extract) : void
		{
			node.accept(this);
		}
		
		protected function visitCombineNode(node : Combine) : void
		{
			node.accept(this);
			
			var allocator			: Allocator	= getAllocatorFor(node);
			var resultLocalOffset	: uint		= allocator.getLocalOffset(node); 
			
			var mask1 : uint, mask2 : uint;
			
			mask1 = RegisterMask.createContinuous(node.arg1.size);
			mask1 = RegisterMask.offset(mask1, resultLocalOffset);
			_asmCode += node.instructionName + ' ' + registerWrite(node, mask1)
				+ ', ' + registerRead(node.arg1, node) + "\t\t// combine\n";
			
			mask2 = RegisterMask.createContinuous(node.arg2.size);
			mask2 = RegisterMask.offset(mask2, resultLocalOffset + node.arg1.size);
			_asmCode += 'mov ' + registerWrite(node, mask2)
				+ ', ' + registerRead(node.arg2, node) + "\t\t// combine\n";
		}
		
		protected function visitAbstractOperationNode(node : AbstractOperation) : void
		{
			node.accept(this);
			
			_asmCode += node.instructionName + ' ' + registerWrite(node);
			_asmCode += ', ' + registerRead(node.arg1, node);
			if (node.arg2 !== null)
				_asmCode += ', ' + registerRead(node.arg2, node);
			
			_asmCode += "\n";
		}
		
		protected function registerWrite(node		: INode, 
										 andMask	: uint = 0xf) : String
		{
			if (node is Interpolate)
				return 'v' + _varyingAllocator.getId(node) 
					+ stringifyMask(andMask & _varyingAllocator.getWriteMask(node));
			
			if (_stack.length < 2)
				return (_isVertexShader ? 'op' : 'oc') + stringifyMask(RegisterMask.XYZW);
			
			if (node is AbstractOperation)
			{
				var operationNode:AbstractOperation = node as AbstractOperation;
				if (_isVertexShader)
					return 'vt' + _vsTmpAllocator.getId(operationNode) 
						+ stringifyMask(andMask & _vsTmpAllocator.getWriteMask(operationNode));
				else
					return 'ft' + _fsTmpAllocator.getId(operationNode) 
						+ stringifyMask(andMask & _fsTmpAllocator.getWriteMask(operationNode));
			}
			
			throw new Error('Unknown type');
		}
		
		protected function registerRead(argument	: INode,
										operation	: AbstractOperation) : String
		{
			// Samplers do no use dynamic allocation, so we treat them appart
			if (argument is Sampler)
			{
				var samplerArgument : Sampler = Sampler(argument);
				
				return 'fs' + samplerArgument.samplerId						// register 
					+ stringifySwizzle(swizzle) 							// swizzle
					+ ' <' + stringifySamplerConf(samplerArgument) + '>';	// sampler configuration
			}
			
//			trace('---------- reading from', argument, 'for', operation);
			
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
//				trace('continuous swizzle', stringifySwizzle(swizzle), 'for an argument of size', argument.size);
				
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
						
//						trace('write offseted swizzle', stringifySwizzle(swizzle), 'with a write offset of', writeOffset);
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
					
//					trace('custom swizzle modded swizzle:', stringifySwizzle(swizzle), '-- modifier was', stringifySwizzle(swizzleNode.swizzle));
					argument		= swizzleNode.arg1;
					swizzleNode		= swizzleNode.arg1 as Extract;
				}
			}
			
			// retrieve the concerned allocator, we are sure this is non-null, because it is an
			// argument, and cannot be the root of the tree.
			var argAllocator	: Allocator = getAllocatorFor(argument);
			
			// ask the allocator where we are to write the result of the node we are processing
			var registerName	: String	= getRegisterNameFor(argument);
			var registerId		: String	= argAllocator.getId(argument).toString();
			var readOffset		: uint		= argAllocator.getLocalOffset(argument);
			
			swizzle = RegisterSwizzling.readOffset(readOffset, swizzle);
			
//			trace('final, after read offseting', stringifySwizzle(swizzle), 'by a value of', readOffset);
			
			return registerName + registerId + stringifySwizzle(swizzle);
		}
		
		protected function getRegisterNameFor(node : INode) : String
		{
			if (node is Attribute)
				return 'va';
			
			if (node is AbstractConstant)
				return _isVertexShader ? 'vc' : 'fc';
			
			if (node is Sampler)
				return 'fs';
			
			if (node is Interpolate)
				return 'v';
			
			if (node is AbstractOperation)
				return _isVertexShader ? 'vt' : 'ft';
			
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
		
		protected function stringifySwizzle(swizzle : uint):String 
		{
			return swizzle != RegisterSwizzling.XYZW ? '.' + RegisterSwizzling.STRINGS[swizzle] : '';
		}
		
		protected function stringifyMask(mask : uint):String
		{
			return mask != RegisterMask.XYZW ? '.' + RegisterMask.STRINGS[mask] : '';
		}
		
		protected function stringifySamplerConf(sampler : Sampler):String
		{
			return _SAMPLER_FILTER_STRINGS[sampler.filter] + ','
				+ _SAMPLER_MIPMAP_STRINGS[sampler.mipmap] + ','
				+ _SAMPLER_WRAPPING_STRINGS[sampler.wrapping] + ','
				+ _SAMPLER_DIMENSION_STRINGS[sampler.dimension];
		}
		
	}
}