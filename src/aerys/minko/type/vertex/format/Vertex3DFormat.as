package aerys.minko.type.vertex.format
{
	public class Vertex3DFormat implements IVertex3DFormat
	{
		public static const XYZ		: IVertex3DFormat	= new Vertex3DFormat(["x", "y", "z"],
																			 [NativeFormat.FLOAT_3]);
		public static const XYZ_RGB	: IVertex3DFormat	= new Vertex3DFormat(["x", "y", "z", "r", "g", "b"],
																			 [NativeFormat.FLOAT_3, NativeFormat.FLOAT_3]);
		public static const XYZ_UV	: IVertex3DFormat	= new Vertex3DFormat(["x", "y", "z", "u", "v"],
																			 [NativeFormat.FLOAT_3, NativeFormat.FLOAT_2]);
		
		private var _offsets	: Object		= new Object();
		private var _formats	: Vector.<int>	= null;
		private var _nbDwords	: int			= 0;
		
		public function get offsets() 			: Object		{ return _offsets; }
		public function get nativeFormats()		: Vector.<int>	{ return _formats; }
		public function get dwordsPerVertex()	: int			{ return _nbDwords; }
		
		public function Vertex3DFormat(properties 		: Array,
									   nativeFormats	: Array)
		{
			var numProperties : int = properties.length;
			var numFormats : int = nativeFormats.length;
			var i : int = 0;
			
			for (i = 0; i < numProperties; ++i)
				_offsets[properties[i]] = i;
			
			_formats = new Vector.<int>(numFormats, true);
			for (i = 0; i < numFormats; ++i)
			{
				var format : int = int(nativeFormats[i])
				
				_formats[i] = format;
				_nbDwords += NativeFormat.NB_DWORDS[int(format - 1)];
			}
		}
	}
}