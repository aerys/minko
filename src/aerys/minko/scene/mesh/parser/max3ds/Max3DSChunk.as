package aerys.minko.scene.mesh.parser.max3ds
{
	import flash.utils.ByteArray;
	
	internal final class Max3DSChunk
	{
		/* CONSTS */
		static public const PRIMARY				: int	= 0x4D4D;
		
		static public const SCENE				: int	= 0x3D3D;
		static public const OBJECT				: int	= 0x4000;
		
		static public const MESH				: int	= 0x4100;
		static public const MESH_VERTICES		: int	= 0x4110;
		static public const MESH_INDICES		: int	= 0x4120;
		static public const MESH_MATERIAL		: int	= 0x4130;
		static public const MESH_MAPPING		: int	= 0x4140;
		
		static public const MATERIAL			: int	= 0xAFFF;
		static public const MATERIAL_NAME		: int	= 0xA000;
		static public const MATERIAL_TEXMAP		: int	= 0xA200;
		static public const MATERIAL_MAPNAME	: int	= 0xA300;
		/* ! CONSTS */
		
		/* VARS */
		protected var _identifier	: int		= 0;
		protected var _length		: int		= 0;
		protected var _endOffset	: int		= 0;
		protected var _data			: ByteArray	= null;
		/* ! VARS */
		
		/* PROPERTIES */
		public function get identifier()		: int		{return (_identifier);}
		public function get length()			: int		{return (_length);}
		public function get data()				: ByteArray	{return (_data);}
		public function get bytesAvailable()	: int
		{
			return (_endOffset > _data.position ? _endOffset - _data.position : 0);
		}
		/* ! PROPERTIES */
		
		/* CONSTRUCTOR */
		public function Max3DSChunk(my_data : ByteArray)
		{
			_data = my_data;

			_endOffset = _data.position;

			_identifier = _data.readUnsignedShort();
			_length = _data.readUnsignedInt();
			
			_endOffset += _length;
		}
		
		public function skip() : void
		{
			_data.position += _length - 6;
		}
		
		public function readString() : String
		{
			var result 	: String 	= "";
			var	c		: int		= 0;
			
			while ((c = _data.readByte()) != 0)
				result += String.fromCharCode(c);

			return (result);
		}
		/* ! CONSTRUCTOR */

	}
}