package aerys.minko.render.shader.compiler
{
	import flash.utils.ByteArray;

	public class CRC32
	{
		private static const POLYNOMIAL 		: uint			= 0x04c11db7;
		private static const CRC_TABLE			: Vector.<uint>	= new Vector.<uint>(256, true);
		private static const TMP_BYTEARRAY		: ByteArray		= new ByteArray();
		
		private static var CRC_TABLE_READY		: Boolean		= false;
		
		private static function generateCrcTable() : void
		{
			for (var i : uint = 0; i < 256; ++i)
			{
				var crcAccum : uint = (i << 24) & 0xffffffff;
				
				for (var j : uint = 0; j < 8; ++j)
				{
					if (crcAccum & 0x80000000)
						crcAccum = (crcAccum << 1) ^ POLYNOMIAL;
					else
						crcAccum = (crcAccum << 1);
				}
				
				CRC_TABLE[i] = crcAccum;
			}
			
			CRC_TABLE_READY = true;
		}
		
		public static function computeForByteArray(source : ByteArray) : uint
		{
			var length	 : uint = source.length;
			var crcAccum : uint = uint(-1);
			
			if (!CRC_TABLE_READY)
				generateCrcTable();
			
			source.position = 0;
			
			for (var j : uint = 0; j < length; j++)
			{
				var i : uint = ((crcAccum >> 24) ^ source.readUnsignedByte()) & 0xFF;
				crcAccum = (crcAccum << 8) ^ CRC_TABLE[i];
			}
			crcAccum = ~crcAccum;
			
			return crcAccum;
		}
		
		public static function computeForString(s : String) : uint
		{
			TMP_BYTEARRAY.length = TMP_BYTEARRAY.position = 0;
			TMP_BYTEARRAY.writeUTFBytes(s);
			return computeForByteArray(TMP_BYTEARRAY); 
		}
		
		public static function computeForNumberVector(v : Vector.<Number>) : uint
		{
			var length		: uint		= v.length;
			
			TMP_BYTEARRAY.length = TMP_BYTEARRAY.position = 0;
			
			for (var i : uint = 0; i < length; ++i)
				TMP_BYTEARRAY.writeDouble(v[i]);
			
			return computeForByteArray(TMP_BYTEARRAY);
		}
	}
}