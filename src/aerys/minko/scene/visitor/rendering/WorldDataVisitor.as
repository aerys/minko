package aerys.minko.scene.visitor.rendering
{
	import aerys.minko.render.renderer.IRenderer;
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
		protected var _worldData	: Dictionary	= null;
		protected var _localData	: LocalData		= new LocalData();
		
		public function get localData()		: LocalData		{ return _localData; }
		public function get worldData() 	: Dictionary	{ return _worldData; }
		public function get renderingData()	: RenderingData	{ return null; }
		public function get renderer()		: IRenderer		{ return null; }
		
		public function WorldDataVisitor()
		{
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
				_localData.world.pop();
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