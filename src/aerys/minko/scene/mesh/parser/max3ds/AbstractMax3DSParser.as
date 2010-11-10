package aerys.minko.scene.mesh.parser.max3ds
{
	
	
	internal class AbstractMax3DSParser
	{
		private var _functions	: Array	= null;
		
		protected function get parseFunctions() : Array
		{
			return (_functions);
		}
		
		public function AbstractMax3DSParser(myChunk : Max3DSChunk = null)
		{
			if (myChunk)
				parseChunk(myChunk);
		}
		
		protected function initialize() : void
		{
			_functions = new Array();
			// override && fill _AFunctions here
		}
		
		protected function finalize() : void
		{
			// NOTHING
		}
		
		final protected function parseChunk(myChunk : Max3DSChunk) : void
		{
			var parseFunction 	: Function 	= null;

			initialize();	
			parseFunction = _functions[myChunk.identifier];
			
			if (parseFunction == null)
			{
				myChunk.skip();
				
				return ;
			}
			
			parseFunction(myChunk);
			
			enterChunk(myChunk);
			
			finalize();
		}

		final protected function enterChunk(myChunk : Max3DSChunk) : void
		{
			while (myChunk.bytesAvailable)
			{
				var chunk 			: Max3DSChunk 	= new Max3DSChunk(myChunk.data);
				var parseFunction	: Function 		= _functions[chunk.identifier];
				
				if (parseFunction == null)
					chunk.skip();
				else if (parseFunction != enterChunk)
					parseFunction(chunk);
			}
		}
		
	}
}