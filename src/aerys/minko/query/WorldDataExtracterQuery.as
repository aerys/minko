package aerys.minko.query
{
	import aerys.minko.query.renderdata.transform.TransformManager;
	import aerys.minko.query.renderdata.world.IWorldData;
	import aerys.minko.query.renderdata.world.WorldDataList;
	import aerys.minko.scene.interfaces.IObject3D;
	import aerys.minko.scene.interfaces.IScene3D;
	import aerys.minko.scene.interfaces.IWorldObject;
	import aerys.minko.scene.group.Group3D;
	import aerys.minko.scene.group.IGroup3D;
	
	import flash.net.getClassByAlias;
	import flash.utils.Dictionary;
	
	public class WorldDataExtracterQuery implements IScene3DQuery
	{
		protected var _worldData	: Dictionary;
		protected var _tm			: TransformManager;
		
		public function get worldData() : Dictionary
		{
			return _worldData;
		}
		
		public function WorldDataExtracterQuery()
		{
			_tm	= new TransformManager();
			reset();
		}
		
		public function reset() : void
		{
			_worldData = new Dictionary();
		}
		
		public function query(scene : IScene3D) : void
		{
			var transformObject : IObject3D		= scene as IObject3D;
			
			// push transform
			if (transformObject)
				_tm.world.push().multiply(transformObject.transform);
			
			// Act in accordance with the node type
			if (scene is IWorldObject)
			{
				queryIWorldObject(scene as IWorldObject);
			}
			else if (scene is IGroup3D)
			{
				queryIGroup3D(scene as IGroup3D);
			}
			
			// pop transform
			if (transformObject)
				_tm.world.pop();
		}
	
		protected function queryIWorldObject(worldObject : IWorldObject) : void
		{
			var worldObjectClass		: Class			= Object(worldObject).constructor;
			
			var worldObjectData			: IWorldData	= worldObject.getData(_tm);
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
		
		protected function queryIGroup3D(groupObject : IGroup3D) : void
		{
			for (var childIndex : uint = 0; true; ++childIndex)
			{
				var child : IScene3D = groupObject.getChildAt(childIndex);
				if (child == null)
					break;
				
				query(child);
			}
		}
	}
}