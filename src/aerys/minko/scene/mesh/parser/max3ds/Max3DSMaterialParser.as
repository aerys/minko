package aerys.minko.scene.mesh.parser.max3ds
{
	internal final class Max3DSMaterialParser extends AbstractMax3DSParser
	{
		/* VARS */
		protected var _name				: String	= null;
		protected var _textureFilename	: String	= null;
		/* ! VARS */
		
		public function get name()				: String	{return (_name);}
		public function get textureFilename()	: String	{return (_textureFilename);}
		
		public function Max3DSMaterialParser(myChunk : Max3DSChunk)
		{
			super (myChunk);
		}
		
		override protected function initialize() : void
		{
			super.initialize();
			
			parseFunctions[Max3DSChunk.MATERIAL] = enterChunk;
			parseFunctions[Max3DSChunk.MATERIAL_NAME] = parseName;
			parseFunctions[Max3DSChunk.MATERIAL_TEXMAP] = enterChunk;
			parseFunctions[Max3DSChunk.MATERIAL_MAPNAME] = parseTextureFilename;
		}
		
		protected function parseName(my_chunk : Max3DSChunk) : void
		{
			_name = my_chunk.readString();
		}
		
		protected function parseTextureFilename(my_chunk : Max3DSChunk) : void
		{
			_textureFilename = my_chunk.readString();
		}
		
	}
}