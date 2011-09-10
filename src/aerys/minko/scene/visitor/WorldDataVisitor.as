package aerys.minko.scene.visitor
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.data.CameraData;
	import aerys.minko.scene.data.IWorldData;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.RenderingData;
	import aerys.minko.scene.node.IScene;
	
	import flash.utils.Dictionary;
	
	public final class WorldDataVisitor implements ISceneVisitor
	{
		private static const ACTIONS_TYPES	: uint	= ActionType.UPDATE_WORLD_DATA
													  | ActionType.UPDATE_LOCAL_DATA
													  | ActionType.RECURSE;
		
		protected var _worldData		: Dictionary	= null;
		protected var _transformData		: TransformData		= null;
		protected var _numNodes			: uint			= 0;
		
		public function get numNodes()		: uint				{ return _numNodes; }
		public function get transformData()		: TransformData			{ return _transformData; }
		public function get worldData() 	: Dictionary		{ return _worldData; }
		public function get renderingData()	: RenderingData		{ return null; }
		public function get ancestors()		: Vector.<IScene>	{ return null; }
		
		public function processSceneGraph(scene			: IScene,
										  transformData		: TransformData,
										  worldData		: Dictionary,
										  renderingData	: RenderingData,
										  renderer		: IRenderer) : void
		{
			_worldData = worldData;
			_transformData = transformData;
			_numNodes = 0;
			visit(scene);
			
			for each (var worldObject : IWorldData in worldData)
				worldObject.setDataProvider(renderingData.styleStack, transformData, worldData);
		}
		
		public function visit(scene : IScene) : void
		{
			var actions 	: Vector.<IAction> 	= scene.actions;
			var numActions	: int				= actions.length;
			var action		: IAction			= null;
			
			for (var i : int = 0; i < numActions; ++i)
				if (((action = actions[i]).type & ACTIONS_TYPES) && !action.run(scene, this, null))
					break ;
			
			++_numNodes;
		}
	}
}