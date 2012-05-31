package aerys.minko.type.loader
{
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.Signal;
	import aerys.minko.type.loader.parser.IParser;
	import aerys.minko.type.loader.parser.ParserOptions;
	
	import flash.events.Event;
	import flash.events.ProgressEvent;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	import flash.utils.getQualifiedClassName;
	
	public class SceneLoader implements ILoader
	{
		private static const REGISTERED_PARSERS : Vector.<Class> = new <Class>[];
		
		private static const STATE_IDLE		: uint = 0;
		private static const STATE_LOADING	: uint = 1;
		private static const STATE_PARSING	: uint = 2;
		private static const STATE_COMPLETE	: uint = 3;
		
		private var _currentState			: int; 
		
		private var _progress				: Signal;
		private var _complete				: Signal;
		private var _error					: Signal;
		
		private var _data					: ISceneNode;
		private var _parser					: IParser;
		private var _numDependencies		: uint;
		private var _dependencies			: Vector.<ILoader>;
		private var _parserOptions			: ParserOptions;
		
		private var _currentProgress		: Number;
		private var _currentProgressChanged : Boolean;
		private var _isComplete				: Boolean;
		
		public function get error() : Signal
		{
			return _error;
		}
		
		public function get progress() : Signal
		{
			return _progress;
		}
		
		public function get complete() : Signal
		{
			return _complete;
		}
		
		public function get isComplete() : Boolean
		{
			return _isComplete;
		}
		
		public function get data() : ISceneNode
		{
			return _data;
		}
		
		public static function registerParser(parserClass : Class) : void
		{
			if (REGISTERED_PARSERS.indexOf(parserClass) != -1)
				return;
			
			if (!(new parserClass(new ParserOptions()) is IParser))
				throw new Error('Parsers must implement the IParser interface.');
			
			REGISTERED_PARSERS.push(parserClass);
		}
		
		public function SceneLoader(parserOptions : ParserOptions)
		{
			_currentState	= STATE_IDLE;
			
			_progress		= new Signal('SceneLoader.progress');
			_complete		= new Signal('SceneLoader.complete');
			_data			= null;
			_isComplete		= false;
			
			_parserOptions	= parserOptions || new ParserOptions();
		}
		
		public function load(urlRequest : URLRequest) : void
		{
			if (_currentState != STATE_IDLE)
				throw new Error('This controller is already loading an asset.');
			
			_currentState = STATE_LOADING;
			
			var urlLoader : URLLoader = new URLLoader();
			urlLoader.dataFormat = URLLoaderDataFormat.BINARY;
			urlLoader.addEventListener(ProgressEvent.PROGRESS, loadProgressHandler);
			urlLoader.addEventListener(Event.COMPLETE, loadCompleteHandler);
			urlLoader.load(urlRequest);
		}
		
		private function loadProgressHandler(e : ProgressEvent) : void
		{
			_progress.execute(this, 0.5 * e.bytesLoaded / e.bytesTotal);
		}
		
		private function loadCompleteHandler(e : Event) : void
		{
			_currentState = STATE_IDLE;
			
			loadBytes(URLLoader(e.currentTarget).data);
		}
		
		public function loadClass(classObject : Class) : void
		{
			loadBytes(new classObject());
		}
		
		public function loadBytes(byteArray : ByteArray) : void
		{
			var startPosition : uint = byteArray.position;
			
			if (_currentState != STATE_IDLE)
				throw new Error('This loader is already loading an asset.');
			
			_currentState = STATE_PARSING;
			_progress.execute(this, 0.5);
			
			if (_parserOptions != null && _parserOptions.parser != null)
			{
				_parser = new (_parserOptions.parser)(_parserOptions);
			}
			else
			{
				// search a parser by testing registered parsers.
				var numRegisteredParser : uint = REGISTERED_PARSERS.length;
				for (var parserId : uint = 0; parserId < numRegisteredParser; ++parserId)
				{
					_parser = new REGISTERED_PARSERS[parserId](_parserOptions);
					byteArray.position = startPosition;
					if (_parser.isParsable(byteArray))
						break;
				}
				
				if (parserId == numRegisteredParser)
					throw new Error('No parser could be found for this datatype');
			}

			byteArray.position = startPosition;
			
			_dependencies	 = _parser.getDependencies(byteArray);
			_numDependencies = 0;
			
			if (_dependencies != null)
				for each (var dependency : ILoader in _dependencies)
					if (!dependency.isComplete)
					{
						dependency.error.add(decrementDependencyCounter);
						dependency.complete.add(decrementDependencyCounter);
						
						_numDependencies++;
					}
			
			if (_numDependencies == 0)
				parse();
		}
		
		private function decrementDependencyCounter(...args) : void
		{
			--_numDependencies;
			
			if (_numDependencies == 0)
				parse();
		}
		
		private function parse() : void
		{
			_parser.error.add(onParseError);
			_parser.progress.add(onParseProgress);
			_parser.complete.add(onParseComplete);
			_parser.parse();
		}
		
		private function onParseError(parser : IParser) : void
		{
			_isComplete = true;
		}
		
		private function onParseProgress(parser : IParser, progress : Number) : void
		{
			_progress.execute(this, 0.5 * (1 + progress));
		}
		
		private function onParseComplete(parser : IParser, loadedData : ISceneNode) : void
		{
			_isComplete = true;
			_data = loadedData;
			_complete.execute(this, loadedData);
		}
	}
}
