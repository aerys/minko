package aerys.minko.render.shader.compiler.sequence
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.render.shader.compiler.register.RegisterType;
	
	import flash.utils.ByteArray;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class AgalDestination implements IAgalToken
	{
		private var _registerNumber	: uint;
		private var _writeMask		: uint;
		private var _registerType	: uint;
		
		public function get registerNumber() : uint
		{
			return _registerNumber;
		}

		public function get writeMask() : uint
		{
			return _writeMask;
		}

		public function get registerType() : uint
		{
			return _registerType;
		}
		
		public function AgalDestination(registerNumber	: uint,
										writeMask		: uint,
										registerType	: uint)
		{
			_registerNumber	= registerNumber;
			_writeMask		= writeMask;
			_registerType	= registerType;
		}
		
		public function getBytecode(destination : ByteArray) : void
		{
			destination.writeShort(registerNumber);	// NNNNNNNNNNNNNNNN = Register number (16 bits)
			destination.writeByte(writeMask);		// ----MMMM = Write mask (4 bits)
			destination.writeByte(registerType);	// ----TTTT = Register type (4 bits)
		}
		
		public function getAgal(isVertexShader : Boolean) : String
		{
			var asmCode : String = RegisterType.stringifyType(registerType, isVertexShader);
			
			if (registerType != RegisterType.OUTPUT)
				asmCode += registerNumber;
			
			asmCode += '.' + Components.maskToString(writeMask);
			
			return asmCode;
		}
	}
}