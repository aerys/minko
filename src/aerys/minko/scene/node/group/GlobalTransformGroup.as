package aerys.minko.scene.node.group
{
	import aerys.minko.scene.action.group.GlobalTransformAction;
	
	import flash.events.Event;

	/**
	 * GlobalTransformGroup scene nodes provides access to global world transform matrix
	 * computed during last scene traversal.
	 * 
	 * The global world transform matrix is available in the 'transform' property.
	 * Each time the node is traversed, the world matrix as it is is copied into the
	 * 'transform' property. Thus, <strong>modifying the 'transform' property will have
	 * no effect</strong></li>.
	 * 
	 * In order to be able to compute a valid global world transform matrix,
	 * GlobalTransformGroup nodes require that there is only one and single path from them
	 * to the the scene root. Therefore, <strong>GlobalTransformGroup nodes can have only
	 * one parent</strong>.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class GlobalTransformGroup extends TransformGroup
	{
		public function GlobalTransformGroup(...children)
		{
			super(children);
		
			actions.unshift(GlobalTransformAction.globalTransformAction);
			addEventListener(Event.ADDED, addedHandler);
		}
		
		private function addedHandler(event : Event) : void
		{
			if (parents.length > 1)
				throw new Error('GlobalTransformGroup can have only one parent.');
		}
	}
}