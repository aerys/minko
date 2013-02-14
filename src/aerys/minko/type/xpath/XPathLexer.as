package aerys.minko.type.xpath
{
	public final class XPathLexer
	{
		private static const OPERATORS	: Vector.<String>	= new <String>[
			'//', '/', '[', ']', '..', '.', '~=', '?=', '=', '@', '*', '(', ')',
			'>=', '>', '<=', '<', '==', '='
		];
		
		private var _string	: String;
		
		public function XPathLexer(str : String)
		{
			_string = str;
		}
		
		public function getToken(doNext : Boolean = false) : String
		{
			var token	: String	= null;
			
			if (!_string)
				return null;
			
			_string = _string.replace(/^\s+/, '');
			
			var nextOpIndex : int = int.MAX_VALUE;
			
			for each (var op : String in OPERATORS)
			{
				var opIndex : int = _string.indexOf(op);
				
				if (opIndex > 0 && opIndex < nextOpIndex)
					nextOpIndex = opIndex;
				
				if (opIndex == 0)
				{
					token = op;
					break ;
				}
			}
			
			if (!token)
				token = _string.substring(0, nextOpIndex);
			
			if (doNext)
				nextToken(token);
			
			return token;
		}
		
		public function getValueToken() : Object
		{
			var value : Object	= null;
			
			_string = _string.replace(/^\s+/, '');
			
			if (_string.charAt(0) == "'")
			{
				var endOfStringIndex : int = _string.indexOf("'", 1);
				
				if (endOfStringIndex < 0)
					throw new Error("Unterminated string expression.");
				
				var stringValue	: String	=  _string.substring(1, endOfStringIndex);
				
				_string = _string.substring(endOfStringIndex + 1);
				
				value = stringValue;
			}
			else
			{
				var token : String	= getToken(true);
				
				if (token == 'true')
					value = true;
				else if (token == 'false')
					value = false;
				else if (token.indexOf('0x') == 0)
					value = parseInt(token, 16);
			}
			
			return value;
		}
		
		public function nextToken(token : String) : void
		{
			_string = _string.substring(_string.indexOf(token) + token.length);
		}
		
		public function checkNextToken(expected : String) : void
		{
			var token : String = getToken(true);
			
			if (token != expected)
				throwParseError(expected, token);
		}
		
		public function throwParseError(expected	: String,
										 got		: String) : void
		{
			throw new Error(
				'Parse error: expected \'' + expected + '\', got \'' + got + '\'.'
			);
		}
	}
}