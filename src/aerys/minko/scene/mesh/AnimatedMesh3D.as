package aerys.minko.scene.mesh
{
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.scene.mesh.modifier.IMeshModifier3D;
	import aerys.minko.type.Animation3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.stream.VertexStreamList3D;
	
	import flash.events.EventDispatcher;
	
	public class AnimatedMesh3D extends EventDispatcher implements IMesh3D, IMeshModifier3D
	{
		private var _frames		: Vector.<IMesh3D>		= null;
		private var _current	: IMesh3D				= null;
		private var _next		: IMesh3D				= null;
		private var _frame		: Number				= 0.;
		private var _animations	: Vector.<Animation3D>	= new Vector.<Animation3D>();
		private var _animation	: Animation3D			= null;
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
		
		public function get vertexStreamList() : VertexStreamList3D
		{
			return _current.vertexStreamList;
		}
		
		public function get indexStream() : IndexStream3D
		{
			return _current.indexStream;
		}
		
		public function get frame() : Number
		{
			return _frame;
		}
		
		public function get target() : IMesh3D
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
		
		public function AnimatedMesh3D(frames : Vector.<IMesh3D>)
		{
			_frames = frames;
			_current = _frames[0];
		}
		
		public function accept(visitor : IScene3DQuery) : void
		{
			visitor.query(_current);
		}
	}
}