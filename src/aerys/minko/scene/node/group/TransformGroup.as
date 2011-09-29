package aerys.minko.scene.node.group 
{
	import aerys.minko.scene.action.transform.PopTransformAction;
	import aerys.minko.scene.action.transform.PushTransformAction;
	import aerys.minko.scene.node.ITransformableScene;
	import aerys.minko.type.math.Matrix3D;
	
	/**
	 * TransformGroup apply a 3D transform to their children.
	 * 
	 * @author Jean-Marc Le Roux
	 */
	public dynamic class TransformGroup extends Group implements ITransformableScene
	{
		private var _transform	: Matrix3D		= new Matrix3D();
			
		/**
		 * The Matrix3D object defining the transform of the object into world-space.
		 */
		public function get transform() : Matrix3D		{ return _transform; }
		
		public function TransformGroup(...children) 
		{
			super(children);
			
			actions.unshift(PushTransformAction.pushTransformAction);
			actions.push(PopTransformAction.popTransformAction);
		}
	}
}