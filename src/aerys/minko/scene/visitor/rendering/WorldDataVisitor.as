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
		public function get renderer()		: IRenderer		{ return null; }
		
		public function WorldDataVisitor()
		{
		}
		
		public function processSceneGraph(scene		: IScene, 
										  worldData	: Dictionary = null) : Dictionary
		{
			_worldData = worldData || new Dictionary();
			visit(scene);
			return _worldData;
		}
		
		public function visit(scene : IScene) : void
		{
			/*var transformObject : ITransformable = scene as ITransformable;
			
			// push transform
			if (transformObject)
				_localData.world.push().multiply(transformObject.transform);
			
			// Act in accordance with the node type
			if (scene is IWorldObject)
			{
				queryIWorldObject(scene as IWorldObject);
			}
			else if (scene is IGroup)
			{
				queryIGroup(scene as IGroup);
			}
			
			// pop transform
			if (transformObject)
				_localData.world.pop();*/
						
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
		
		protected function queryIWorldObject(worldObject : IWorldObject) : void
		{
			var worldObjectData	: IWorldData	= worldObject.getData(_localData);
						if (worldObjectData == null)
				return;
			
			var worldObjectDataClass	: Class			= Object(worldObjectData).constructor;
			
			if (worldObject.isSingle)
			{
				_worldData[worldObjectDataClass] = worldObjectData;
			}
			else
			{
				if (_worldData[worldObjectDataClass] == undefined)
					_worldData[worldObjectDataClass] = new WorldDataList();
				
				_worldData[worldObjectDataClass].push(worldObjectData);
			}
		}
		
		protected function queryIGroup(groupObject : IGroup) : void
		{
			for (var childIndex : uint = 0; true; ++childIndex)
			{
				var child : IScene = groupObject.getChildAt(childIndex);
				
				if (child == null)
					break ;
				
				visit(child);
			}
		}
	}
}