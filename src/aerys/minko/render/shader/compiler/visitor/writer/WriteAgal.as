package aerys.minko.render.shader.compiler.visitor.writer
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.shader.compiler.allocator.Allocator;
	import aerys.minko.render.shader.compiler.register.RegisterMask;
	import aerys.minko.render.shader.compiler.register.RegisterSwizzling;
	import aerys.minko.render.shader.compiler.register.RegisterType;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Sampler;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;

	use namespace minko_shader;

	public class WriteAgal extends AbstractWriter
	{

		protected var _asmCode			: String;

		public function WriteAgal(attrAllocator		: Allocator,
								  fsTmpAllocator	: Allocator,
								  varyingAllocator	: Allocator,
								  fsConstAllocator	: Allocator,
								  vsTmpAllocator	: Allocator,
								  vsConstAllocator	: Allocator)
		{
			super(attrAllocator, fsTmpAllocator, varyingAllocator, fsConstAllocator, vsTmpAllocator, vsConstAllocator);
		}

		public function processVertexShader(vertexOps : Vector.<INode>) : String
		{
			_asmCode		= '';
			_isVertexShader	= true;

			for each (var vertexOp : INode in vertexOps)
				visit(vertexOp);

			return _asmCode;
		}

		public function processFragmentShader(color : INode) : String
		{
			_asmCode		= '';
			_isVertexShader = false;

			visit(color);

			return _asmCode;
		}

		override protected function visitCombineNode(node : Combine) : void
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

			var swizzle2 : uint = getReadSwizzle(node.arg2, node);
			swizzle2 = RegisterSwizzling.writeOffset(node.arg1.size, swizzle2);

			_asmCode += 'mov ';
			registerWrite(node, mask2)
			_asmCode += ', ';

			var argument : INode = node.arg2;
			while (argument is Extract)
				argument = Extract(argument).arg1;

			registerReadFromCommon(argument, node, swizzle2);
			_asmCode += "\n";
		}

		override protected function outputMoveIntruction(from		: INode,
														 to			: AbstractOperation,
														 andMask	: uint = 0xf):void
		{
			_asmCode += 'mov ';
			registerWrite(to, andMask)
			_asmCode += ', ';
			registerRead(from, to);
			_asmCode += "\n";
		}

		override protected function outputCommonInstruction(node : AbstractOperation):void
		{
			_asmCode += node.instructionName
			_asmCode += " ";
			registerWrite(node);
			_asmCode += ', ';
			registerRead(node.arg1, node);

			if (node.arg2)
			{
				_asmCode += ', ';
				registerRead(node.arg2, node);
			}

			_asmCode += "\n";
		}

		override protected function outputDestination(registerNumber	: uint,
													  writeMask			: uint,
													  registerType		: uint) : void
		{
			_asmCode += RegisterType.stringifyType(registerType, _isVertexShader);

			if (registerType != RegisterType.OUTPUT)
				_asmCode += registerNumber;

			_asmCode += RegisterMask.stringify(writeMask);
		}

		override protected function outputSamplerSource(index		: uint,
														dimension	: uint,
														wrapping	: uint,
														filter		: uint,
														mipmap		: uint) : void
		{
			_asmCode += 'fs';
			_asmCode += index + ' <';
			_asmCode += Sampler._SAMPLER_FILTER_STRINGS[filter] + ',';
			_asmCode += Sampler._SAMPLER_MIPMAP_STRINGS[mipmap] + ',';
			_asmCode += Sampler._SAMPLER_WRAPPING_STRINGS[wrapping] + ',';
			_asmCode += Sampler._SAMPLER_DIMENSION_STRINGS[dimension];
			_asmCode += '>';
		}

		override protected function outputCommonSource(registerNumber				: uint,
													   indirectOffset				: uint,
													   swizzle						: uint,
													   registerType					: uint,
													   indexRegisterType			: uint,
													   indexRegisterComponentSelect	: uint,
													   direct						: Boolean) : void
		{
			_asmCode += RegisterType.stringifyType(registerType, _isVertexShader);

			if (direct)
			{
				_asmCode += registerNumber.toString();
			}
			else
			{
				_asmCode += '[';
				_asmCode += RegisterType.stringifyType(indexRegisterType, _isVertexShader);
				_asmCode += registerNumber;
				_asmCode += RegisterSwizzling.stringify(indexRegisterComponentSelect).substr(0, 2);

				if (indirectOffset != 0)
					_asmCode += ' + ' + indirectOffset.toString();

				_asmCode += ']';
			}

			_asmCode += RegisterSwizzling.stringify(swizzle);
		}

		override protected function outputEmptySource() : void
		{
			// do nothing
		}
	}
}
