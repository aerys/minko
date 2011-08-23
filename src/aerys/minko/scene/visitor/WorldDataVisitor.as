package aerys.minko.scene.visitor
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.data.CameraData;
	import aerys.minko.scene.data.IWorldData;
	import aerys.minko.scene.data.LocalData;
	import aerys.minko.scene.data.RenderingData;
	import aerys.minko.scene.node.IScene;
	
	import flash.utils.Dictionary;
	
	public class WorldDataVisitor implements ISceneVisitor
	{
		private static const ACTIONS_TYPES	: uint	= ActionType.UPDATE_WORLD_DATA
													  | ActionType.UPDATE_LOCAL_DATA
													  | ActionType.RECURSE;
		
		protected var _worldData		: Dictionary	= null;
		protected var _localData		: LocalData		= null;
		protected var _numNodes			: uint			= 0;
		
		public function get numNodes()		: uint				{ return _numNodes; }
		public function get localData()		: LocalData			{ return _localData; }
		public function get worldData() 	: Dictionary		{ return _worldData; }
		public function get renderingData()	: RenderingData		{ return null; }
		public function get ancestors()		: Vector.<IScene>	{ return null; }
		
		public function WorldDataVisitor()
		{
		}
		
		public function processSceneGraph(scene			: IScene,
										  localData		: LocalData,
										  worldData		: Dictionary,
										  renderingData	: RenderingData,
										  renderer		: IRenderer) : void
		{
			_worldData = worldData;
			_localData = localData;
			_numNodes = 0;
			visit(scene);
			
			for each (var worldObject : IWorldData in worldData)
				worldObject.setDataProvider(renderingData.styleStack, localData, worldData);
			
			// update our transformManager if there is a camera, or
			// set it to null to render to screenspace otherwise
			var cameraData : CameraData = worldData[CameraData] as CameraData;
			
			if (cameraData)
			{
				localData.view			= cameraData.view;
				localData.projection	= cameraData.projection;
			}
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