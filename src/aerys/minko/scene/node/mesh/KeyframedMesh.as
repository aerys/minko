package aerys.minko.scene.node.mesh
{
	import aerys.minko.render.effect.Style;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.action.mesh.MeshAction;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.scene.node.IStylableScene;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	
	import flash.utils.getTimer;
	
	public class KeyframedMesh extends AbstractScene implements IMesh, IStylableScene
	{
		private var _style				: Style;
		
		private var _startTime			: uint;
		private var _isPlaying			: Boolean;
		private var _playHeadTime		: uint;
		
		
		
		
		private var _times				: Vector.<uint>;
		private var _duration			: uint;
		
		private var _indexStream 		: IndexStream;
		private var _vertexStreams		: Vector.<VertexStream>;
		
		
		
		
		
		
		
		
		
		
		public function get style()			: Style					{ return _style; }
		public function get styleEnabled()	: Boolean				{ return true; }
		public function get version()		: uint					{ return 0; }
		public function get indexStream()	: IndexStream			{ return _indexStream; }
		public function get vertexStreams()	: Vector.<VertexStream>	{ return _vertexStreams; }
		
		public function get vertexStream()	: IVertexStream
		{
			var playHeadTime : uint = (_isPlaying ? getTimer() - _startTime : _playHeadTime) % _duration;
			
			var timeCount : uint = _times.length;
			for (var timeId : uint = 0; timeId < timeCount; ++timeId)
				if (_times[timeId] >= playHeadTime)
					break;
			
			if (timeId == 0)
				return _vertexStreams[0];
			else if (timeId == timeCount)
				return _vertexStreams[timeCount - 1];
			else
			{
//				var lastTime	 : uint = _times[timeId - 1];
//				var nextTime	 : uint = _times[timeId];
//				var ratio		 : uint = (playHeadTime - lastTime) / (nextTime - lastTime);
				
				return _vertexStreams[timeId - 1];
			}
		}
		
		public function get playHeadTime():uint
		{
			return _playHeadTime;
		}

		public function set playHeadTime(value:uint):void
		{
			_playHeadTime = value;
		}

		public function KeyframedMesh(vertexStreams : Vector.<VertexStream>,
									  times			: Vector.<uint>,
									  duration		: uint,
									  indexStream	: IndexStream = null)
		{
			super();
			
			_isPlaying		= false;
			_indexStream	= indexStream;
			_vertexStreams	= vertexStreams;
			
			_times			= times;
			_duration		= duration;
			
			// a keyframed mesh is rendered using a the common mesh action.
			// only the shader needs to change
			actions[0]		= MeshAction.meshAction;
			
			if (!_indexStream)
				_indexStream = new IndexStream(null, vertexStream.length, vertexStream.dynamic);
		}
		
		public function start() : void
		{
			_startTime = getTimer();
			_isPlaying = true;
		}
		
		public function stop() : void
		{
			_isPlaying = false;
			
		}
		
	}
}