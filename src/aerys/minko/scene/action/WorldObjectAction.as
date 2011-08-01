package aerys.minko.scene.action
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.IWorldObject;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.data.IWorldData;
	import aerys.minko.scene.data.WorldDataList;
	
	import flash.utils.Dictionary;
	
	public class WorldObjectAction implements IAction
	{
		private static const TYPE	: uint		= ActionType.UPDATE_WORLD_DATA;
		
		private static var _instance	: WorldObjectAction	= null;
		
		public static function get worldObjectAction() : WorldObjectAction
		{
			return _instance || (_instance = new WorldObjectAction());
		}
		
		public function get type() : uint		{ return TYPE; }
		
		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var worldObject		: IWorldObject	= scene as IWorldObject;
			var worldData		: Dictionary	= visitor.worldData;
			var worldObjectData	: IWorldData	= worldObject.getData(visitor.localData);
			
			if (worldObjectData == null)
				return false;
			
			var worldObjectDataClass	: Class	= Object(worldObjectData).constructor;
			
			if (worldObject.isSingle)
			{
				worldData[worldObjectDataClass] = worldObjectData;
			}
			else
			{
				if (worldData[worldObjectDataClass] == undefined)
					worldData[worldObjectDataClass] = new WorldDataList();
				
				worldData[worldObjectDataClass].push(worldObjectData);
			}
			
			return true;
		}
	}
}