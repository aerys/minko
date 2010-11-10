package aerys.minko.scene.mesh.parser.max3ds
{
	internal class Max3DSObjectParser extends AbstractMax3DSParser
	{
		/* VARS */
		private var _name	: String	= null;
		/* ! VARS */
	
		public function get name() : String
		{
			return (_name);
		}
		
		public function set name(value : String) : void
		{
			_name = value;
		}
	
		public function Max3DSObjectParser(myChunk : Max3DSChunk, myName : String)
		{
			super (myChunk);
			
			_name = myName;
		}
	}
}