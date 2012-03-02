package aerys.minko.scene.controller
{
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.Signal;
	
	import avmplus.getQualifiedClassName;

	public class AbstractController
	{
		private var _mode			: uint					= 0;
		
		private var _lastTime		: Number				= 0.0;
		private var _lastTarget		: ISceneNode			= null;
		
		private var _targetType		: Class					= null;
		private var _targets		: Vector.<ISceneNode>	= new <ISceneNode>[];
		
		private var _ticked			: Signal				= new Signal();
		private var _targetAdded	: Signal				= new Signal();
		private var _targetRemoved	: Signal				= new Signal();
		
		public function get mode() : uint
		{
			return _mode;
		}
		
		public function get numTargets() : uint
		{
			return _targets.length;
		}
		
		public function get ticked() : Signal
		{
			return _ticked;
		}
		
		public function get targetAdded() : Signal
		{
			return _targetAdded;
		}
		
		public function get targetRemoved() : Signal
		{
			return _targetRemoved;
		}
		
		public function AbstractController(targetType : Class 	= null,
										   mode		  : uint	= 1)
		{
			_targetType = targetType || ISceneNode;
			_mode = mode;
		}
		
		public function addTarget(target : ISceneNode) : void
		{
			if (_targetType && !(target is _targetType))
			{
				throw new Error(
					"Controller '" + getQualifiedClassName(this)
					+ " cannot target objects from class '"
					+ getQualifiedClassName(target) + "'."
				);
			}
			
			_targets.push(target);
			
			_targetAdded.execute(this, target);
		}
		
		public function removeTarget(target : ISceneNode) : void
		{
			var index : int = _targets.indexOf(target);
			var numTargets : int = _targets.length - 1;
			
			if (index < 0)
				throw new Error();
			
			_targets[index] = _targets[numTargets];
			_targets.length = numTargets;
			
			_targetRemoved.execute(this, target);
		}
		
		public function getTarget(index : uint) : ISceneNode
		{
			return _targets[index];
		}
		
		public function tick(target : ISceneNode, time : Number) : void
		{
			var update : Boolean	= false;
			
			if (time != _lastTime)
			{
				update = updateOnTime(time);
				_lastTime = time;
			}
			
			if (target != _lastTarget || update)
			{
				updateTarget(target);
				_lastTarget = target;
			}
			
			_ticked.execute(this, target, time);
		}
		
		protected function updateOnTime(time : Number) : Boolean
		{
			// nothing
			
			return false;
		}
		
		protected function updateTarget(target : ISceneNode) : void
		{
			// nothing
		}
	}
}