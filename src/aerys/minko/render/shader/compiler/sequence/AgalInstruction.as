package aerys.minko.render.shader.compiler.sequence
{
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	
	import flash.utils.ByteArray;

	public class AgalInstruction
	{
		private var _opCode			: uint;
		private var _destination	: AgalDestination;
		private var _source0		: IAgalSource;
		private var _source1		: IAgalSource;
		
		public function get opCode()		: uint				{ return _opCode;		}
		public function get destination()	: AgalDestination	{ return _destination;	}
		public function get source0()		: IAgalSource		{ return _source0;		}
		public function get source1()		: IAgalSource		{ return _source1;		}
		
		public function AgalInstruction(opCode		: uint,
										destination : AgalDestination,
										source0		: IAgalSource,
										source1		: IAgalSource)
		{
			_opCode			= opCode;
			_destination	= destination;
			_source0		= source0;
			_source1		= source1;
		}
		
		public function getByteCode(bytes : ByteArray) : void
		{
			bytes.writeUnsignedInt(opCode);
			_destination.getBytecode(bytes);
			_source0.getBytecode(bytes);
			_source1.getBytecode(bytes);
		}
		
		public function getAgal(isVertexShader : Boolean) : String
		{
			var asmCode : String = Instruction.NAME[_opCode];
			
			asmCode += " ";
			asmCode += _destination.getAgal(isVertexShader);
			asmCode += ', ';
			asmCode += _source0.getAgal(isVertexShader);
			
			if (!(_source1 is AgalSourceEmpty))
			{
				asmCode += ', ';
				asmCode += _source1.getAgal(isVertexShader);
			}
			
			asmCode += "\n";
			
			return asmCode;
		}
		
		
	}
}
