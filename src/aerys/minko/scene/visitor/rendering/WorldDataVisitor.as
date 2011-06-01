package aerys.minko.scene.visitor.rendering
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.ITransformable;
	import aerys.minko.scene.node.IWorldObject;
	import aerys.minko.scene.node.group.IGroup;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.data.IWorldData;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.scene.visitor.data.RenderingData;
	import aerys.minko.scene.visitor.data.WorldDataList;
	
	import flash.utils.Dictionary;
	
	public class WorldDataVisitor implements ISceneVisitor
	{
		private static const ACTIONS_TYPES	: uint	= ActionType.UPDATE_WORLD_DATA
													  | ActionType.UPDATE_LOCAL_DATA
													  | ActionType.RECURSE;
		
		protected var _worldData	: Dictionary	= null;
		protected var _localData	: LocalData		= new LocalData();
		
		public function get localData()		: LocalData		{ return _localData; }
		public function get worldData() 	: Dictionary	{ return _worldData; }
		public function get renderingData()	: RenderingData	{ return null; }
		
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
			
			visit(scene);
		}
		
		public function visit(scene : IScene) : void
		{
			var actions 	: Vector.<IAction> 	= scene.actions;
			var numActions	: int				= actions.length;
			var	i			: int				= 0;
			var action		: IAction			= null;
			
			for (i = 0; i < numActions; ++i)
				if (((action = actions[i]).type & ACTIONS_TYPES) && !action.prefix(scene, this, null))
					break ;
			
			for (i = 0; i < numActions; ++i)
				if (((action = actions[i]).type & ACTIONS_TYPES) && !action.infix(scene, this, null))
					break ;
			
			for (i = 0; i < numActions; ++i)
				if (((action = actions[i]).type & ACTIONS_TYPES) && !action.postfix(scene, this, null))
					break ;
		}
	}
}