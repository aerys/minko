package aerys.minko.render.shader.compiler.visitor.writer
{
	import aerys.minko.render.shader.compiler.allocator.Allocator;
	import aerys.minko.render.shader.compiler.register.RegisterMask;
	import aerys.minko.render.shader.compiler.register.RegisterSwizzling;
	import aerys.minko.render.shader.compiler.register.RegisterType;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class WriteByteCode extends AbstractWriter
	{
		protected static const OPCODE_MOVE	: uint = 0x0;
		
		protected var _bytecode			: ByteArray;
		
		public function WriteByteCode(attrAllocator	: Allocator,
									  fsTmpAllocator	: Allocator, 
									  varyingAllocator	: Allocator, 
									  fsConstAllocator	: Allocator, 
									  vsTmpAllocator	: Allocator, 
									  vsConstAllocator	: Allocator)
		{
			super(attrAllocator, fsTmpAllocator, varyingAllocator, fsConstAllocator, vsTmpAllocator, vsConstAllocator);
		}
		
		public function processVertexShader(vertexOps : Vector.<INode>) : ByteArray
		{
			_bytecode = new ByteArray();
			_bytecode.endian = Endian.LITTLE_ENDIAN;
			
			_isVertexShader = true;
			outputHeader();
			for each (var vertexOp:INode in vertexOps)
				visit(vertexOp);
			
			return _bytecode;
		}
		
		public function processFragmentShader(color : INode) : ByteArray
		{
			_bytecode = new ByteArray();
			_bytecode.endian = Endian.LITTLE_ENDIAN;
			
			_isVertexShader = false;
			outputHeader();
			visit(color);
			
			return _bytecode;
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
			
			_bytecode.writeUnsignedInt(OPCODE_MOVE);
			registerWrite(node, mask2)
			
			var argument : INode = node.arg2;
			while (argument is Extract)
				argument = Extract(argument).arg1;
			
			registerReadFromCommon(argument, node, swizzle2);
			registerRead(null, node);
		}
		
		
		
		
		
		
		
		
		
		protected function outputHeader() : void
		{
			_bytecode.writeByte(0xa0);						// tag version
			_bytecode.writeUnsignedInt(1);					// AGAL version, big endian, bit pattern will be 0x01000000
			_bytecode.writeByte(0xa1);						// tag program id
			_bytecode.writeByte(_isVertexShader ? 0 : 1);	// vertex or fragment
		}
		
		override protected function outputMoveIntruction(from		: INode, 
														 to			: AbstractOperation, 
														 andMask	: uint = 0xf):void
		{
			_bytecode.writeUnsignedInt(OPCODE_MOVE);
			registerWrite(to, andMask)
			registerRead(from, to);
			registerRead(null, to);
		}
		
		override protected function outputCommonInstruction(node : AbstractOperation):void
		{
			_bytecode.writeUnsignedInt(node.opCode);
			registerWrite(node);
			registerRead(node.arg1, node);
			registerRead(node.arg2, node);
		}
		
		override protected function outputDestination(registerNumber	: uint,
													  writeMask			: uint,
													  registerType		: uint) : void
		{
			_bytecode.writeShort(registerNumber);			// NNNNNNNNNNNNNNNN = Register number (16 bits)
			_bytecode.writeByte(writeMask);					// ----MMMM = Write mask (4 bits)
			_bytecode.writeByte(registerType);				// ----TTTT = Register type (4 bits)
		}
		
		override protected function outputSamplerSource(index		: uint,
														dimension	: uint,
														wrapping	: uint,
														filter		: uint,
														mipmap		: uint) : void
		{
			_bytecode.writeShort(index);					//	NNNNNNNNNNNNNNNN = Sampler index (16 bits)
			_bytecode.writeShort(0x0);						//	---------------- = Nothing
			_bytecode.writeByte(RegisterType.SAMPLER);		//	----TTTT = Register type, must be 5, Sampler
			_bytecode.writeByte(dimension << 4);			//	DDDD---- = Dimension (0=2D,1=Cube)
			_bytecode.writeByte(wrapping << 4);				//	WWWWSSSS = Wrapping (0=clamp,1=repeat), Special flag bits (must be 0)
			_bytecode.writeByte((filter << 4) | mipmap);	//	FFFFMMMM = Filter (0=nearest,1=linear) (4bits), Mipmap (0=disable,1=nearest,2=linear)
		}
		
		override protected function outputCommonSource(registerNumber				: uint,
													   indirectOffset				: uint,
													   swizzle						: uint,
													   registerType					: uint,
													   indexRegisterType			: uint,
													   indexRegisterComponentSelect	: uint,
													   direct						: Boolean) : void
		{
			_bytecode.writeShort(registerNumber);				//	NNNNNNNNNNNNNNNN = Register number (16 bits)
			_bytecode.writeByte(indirectOffset);				//	OOOOOOOO = Indirect offset (8bits) 
			_bytecode.writeByte(swizzle);						//	SSSSSSSS = Swizzle (8bits, 2bits per component)
			_bytecode.writeByte(registerType);					//	----TTTT = Register type (4 bits)
			_bytecode.writeByte(indexRegisterType);				//	----IIII = Index register type (4 bits)
			_bytecode.writeByte(indexRegisterComponentSelect);	//	------QQ = Index register component select (2 bits)
			_bytecode.writeByte(direct ? 0 : 1 << 7);			//	D------- = Direct=0/Indirect=1
		}
		
		override protected function outputEmptySource() : void
		{
			_bytecode.writeUnsignedInt(0);
			_bytecode.writeUnsignedInt(0);
		}
	}
}