package aerys.minko.render.shader.compiler.sequence
{
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	
	import flash.utils.ByteArray;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class AgalInstruction implements IAgalToken
	{
		private var _opCode			: uint;
		private var _destination	: AgalDestination;
		private var _source0		: IAgalToken;
		private var _source1		: IAgalToken;
		
		public function get opCode()		: uint				{ return _opCode;		}
		public function get destination()	: AgalDestination	{ return _destination;	}
		public function get source0()		: IAgalToken		{ return _source0;		}
		public function get source1()		: IAgalToken		{ return _source1;		}
		
		public function AgalInstruction(opCode		: uint,
										destination : AgalDestination,
										source0		: IAgalToken,
										source1		: IAgalToken)
		{
			_opCode			= opCode;
			_destination	= destination;
			_source0		= source0;
			_source1		= source1;
		}
		
		public function getBytecode(bytes : ByteArray) : void
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
			
			if (_opCode != Instruction.KIL)
			{
				asmCode += _destination.getAgal(isVertexShader);
				asmCode += ', ';
			}
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
