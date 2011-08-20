package aerys.minko.type.animation.timeline
{
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.ITransformableScene;
	import aerys.minko.type.math.Matrix4x4;

	public class TransformMatrixLinearTimeline implements ITimeline
	{
		private var _id			: String;
		private var _target		: String;
		
		private var _times		: Vector.<uint>;
		private var _matrices	: Vector.<Matrix4x4>
		
		public function get id()		: String	{ return _id; }
		public function get target()	: String	{ return _target; }
		public function get duration()	: uint		{ return _times[_times.length - 1]; }
		
		public function TransformMatrixLinearTimeline(id		: String,
													  target 	: String,
												      times 	: Vector.<uint>,
													  matrices	: Vector.<Matrix4x4>)
		{
			_id			= id;
			_target		= target;
			_times		= times;
			_matrices	= matrices;
		}
		
		public function updateAt(t : uint, scene : IScene) : void
		{
			var out			: Matrix4x4 = ITransformableScene(scene).transform;
			var i			: uint;
			var timesLength : uint = _times.length;
			
			// FIXME Replace me by a dichotomy
			for (i = 0; i < timesLength; ++i)
				if (_times[i] >= t)
					break;
			
			if (i == 0)
			{
				Matrix4x4.copy(_matrices[0], out);
			}
			else if (i == timesLength)
			{
				Matrix4x4.copy(_matrices[timesLength - 1], out);
			}
			else
			{
				var previousTime		: Number	= _times[i - 1];
				var nextTime			: Number	= _times[i];
				var interpolationRatio	: Number	= (t - previousTime) / (nextTime - previousTime);
				
				var previousMatrix		: Matrix4x4 = _matrices[i - 1];
				var nextMatrix			: Matrix4x4 = _matrices[i];
				
				Matrix4x4.copy(previousMatrix, out);
				previousMatrix.interpolateTo(nextMatrix, 1 - interpolationRatio);
			}
		}
		
		public function cloneReversed() : ITimeline
		{
			var id			: String				= _id + '_reversed';
			var target		: String				= _target;
			var matrices	: Vector.<Matrix4x4>	= _matrices.slice().reverse();
			var times		: Vector.<uint>			= _times.slice().reverse();
			
			return new TransformMatrixLinearTimeline(id, target, times, matrices);
		}
	}
}
