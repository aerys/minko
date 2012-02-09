package aerys.minko.scene.controller
{
	import aerys.minko.scene.node.Group;

	public class AbstractController
	{
		private var _lastTime	: Number	= 0.0;
		private var _lastTarget	: Group		= null;
		
		public function tick(target : Group, time : Number) : void
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
		}
		
		protected function updateOnTime(time : Number) : Boolean
		{
			// nothing
			
			return false;
		}
		
		protected function updateTarget(target : Group) : void
		{
			// nothing
		}
	}
}