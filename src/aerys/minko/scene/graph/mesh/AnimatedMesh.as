package aerys.minko.scene.graph.mesh
{
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.graph.mesh.modifier.IMeshModifier;
	import aerys.minko.type.Animation;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;
	
	import flash.events.EventDispatcher;
	
	public class AnimatedMesh extends EventDispatcher implements IMesh, IMeshModifier
	{
		private var _frames		: Vector.<IMesh>		= null;
		private var _current	: IMesh				= null;
		private var _next		: IMesh				= null;
		private var _frame		: Number				= 0.;
		private var _animations	: Vector.<Animation>	= new Vector.<Animation>();
		private var _animation	: Animation			= null;
		private var _isPlaying	: Boolean				= false;
		private var _loop		: Boolean				= true;
		private var _version	: uint					= 0;
		
		public function get version() : uint
		{
			return _version + _current.version;
		}
		
		public function get name() : String
		{
			return null;
		}
		
		public function get vertexStreamList() : VertexStreamList
		{
			return _current.vertexStreamList;
		}
		
		public function get indexStream() : IndexStream
		{
			return _current.indexStream;
		}
		
		public function get frame() : Number
		{
			return _frame;
		}
		
		public function get target() : IMesh
		{
			return _current;
		}
		
		public function set frame(value : Number) : void
		{
			if (value == _frame)
				return ;
			
			++_version;
			
			if (_animation)
			{
				var next : uint	= int(value + 1);
				
				if (!_isPlaying)
					return ;
				
				_frame = value % (_animation.numFrames);
				
				if (next >= _animation.numFrames)
				{
					next = _animation.loop ? next % (_animation.numFrames) : _animation.numFrames;
					_isPlaying = _animation.loop && _loop;
					
					/*if (!_animation.loop || !_loop)
						dispatchEvent(new Animation3DEvent(AnimationEvent.STOP));
					else
						dispatchEvent(new Animation3DEvent(AnimationEvent.LOOP));*/
				}
				_current = _frames[int((_animation.offset + _frame) % _frames.length)];
				_next = _frames[int((_animation.offset + next) % _frames.length)];
			}
			else
			{
				_frame = value % _frames.length;
				_current = _frames[int(_frame)];
				_next = _frames[int((_frame + 1) % _frames.length)];
			}
		}
		
		public function AnimatedMesh(frames : Vector.<IMesh>)
		{
			_frames = frames;
			_current = _frames[0];
		}
		
		public function visited(visitor : ISceneVisitor) : void
		{
			visitor.query(_current);
		}
	}
}