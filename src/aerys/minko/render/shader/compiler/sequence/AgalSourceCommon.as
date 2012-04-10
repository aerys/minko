package aerys.minko.render.shader.compiler.sequence
{
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.render.shader.compiler.register.RegisterType;
	
	import flash.utils.ByteArray;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class AgalSourceCommon implements IAgalToken
	{
		private var _registerNumber					: uint;
		private var _indirectOffset					: uint;
		private var _swizzle						: uint;
		private var _registerType					: uint;
		private var _indexRegisterType				: uint;
		private var _indexRegisterComponentSelect	: uint;
		private var _direct							: Boolean;
		
		public function get direct()						: Boolean	{ return _direct;						}
		public function get indexRegisterComponentSelect()	: uint		{ return _indexRegisterComponentSelect;	}
		public function get indexRegisterType()				: uint		{ return _indexRegisterType;			}
		public function get registerType()					: uint		{ return _registerType;					}
		public function get swizzle()						: uint		{ return _swizzle;						}
		public function get indirectOffset()				: uint		{ return _indirectOffset;				}
		public function get registerNumber()				: uint		{ return _registerNumber;				}
		
		public function AgalSourceCommon(registerNumber					: uint,
										 indirectOffset					: uint,
										 swizzle						: uint,
										 registerType					: uint,
										 indexRegisterType				: uint,
										 indexRegisterComponentSelect	: uint,
										 direct							: Boolean)
		{
			_registerNumber					= registerNumber;
			_indirectOffset					= indirectOffset;
			_swizzle						= swizzle;
			_registerType					= registerType;
			_indexRegisterType				= indexRegisterType;
			_indexRegisterComponentSelect	= indexRegisterComponentSelect;
			_direct							= direct;
		}
		
		public function getBytecode(destination : ByteArray) : void
		{
			destination.writeShort(registerNumber);					//	NNNNNNNNNNNNNNNN = Register number (16 bits)
			destination.writeByte(indirectOffset);					//	OOOOOOOO = Indirect offset (8bits)
			destination.writeByte(swizzle);							//	SSSSSSSS = Swizzle (8bits, 2bits per component)
			destination.writeByte(registerType);					//	----TTTT = Register type (4 bits)
			destination.writeByte(indexRegisterType);				//	----IIII = Index register type (4 bits)
			destination.writeByte(indexRegisterComponentSelect);	//	------QQ = Index register component select (2 bits)
			destination.writeByte(direct ? 0 : 1 << 7);				//	D------- = Direct=0/Indirect=1
		}
		
		public function getAgal(isVertexShader : Boolean) : String
		{
			var asmCode : String = RegisterType.stringifyType(registerType, isVertexShader);
			
			if (direct)
			{
				asmCode += registerNumber.toString();
			}
			else
			{
				asmCode += '[';
				asmCode += RegisterType.stringifyType(indexRegisterType, isVertexShader);
				asmCode += registerNumber;
				asmCode += '.';
				asmCode += Components.swizzleToString(indexRegisterComponentSelect).substr(0, 1);
				
				if (indirectOffset != 0)
					asmCode += ' + ' + indirectOffset.toString();
				
				asmCode += ']';
			}
			
			asmCode += '.' + Components.swizzleToString(swizzle);
			
			return asmCode;
		}
	}
}
