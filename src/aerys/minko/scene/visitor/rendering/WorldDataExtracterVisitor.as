package aerys.minko.scene.visitor.rendering
{
	import aerys.minko.scene.graph.IScene;
	import aerys.minko.scene.graph.ITransformable;
	import aerys.minko.scene.graph.IWorldObject;
	import aerys.minko.scene.graph.group.IGroup;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.data.IWorldData;
	import aerys.minko.scene.visitor.data.TransformManager;
	import aerys.minko.scene.visitor.data.WorldDataList;
	
	import flash.display.InteractiveObject;
	import flash.utils.Dictionary;
	
	public class WorldDataExtracterVisitor implements ISceneVisitor
	{
		protected var _worldData	: Dictionary;
		protected var _tm			: TransformManager;
		
		public function get worldData() : Dictionary
		{
			return _worldData;
		}
		
		public function WorldDataExtracterVisitor()
		{
			_tm	= new TransformManager();
			reset();
		}
		
		public function reset() : void
		{
			_worldData = new Dictionary();
		}
		
		public function visit(scene : IScene) : void
		{
			var transformObject : ITransformable = scene as ITransformable;
			
			// push transform
			if (transformObject)
				_tm.world.push().multiply(transformObject.transform);
			
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
				_tm.world.pop();
		}
	
		protected function queryIWorldObject(worldObject : IWorldObject) : void
		{
			var worldObjectData			: IWorldData	= worldObject.getData(_tm);
			
			if (!worldObjectData)
				return ;
			
			var worldObjectClass		: Class			= Object(worldObject).constructor;
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