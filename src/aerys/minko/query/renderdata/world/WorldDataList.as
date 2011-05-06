package aerys.minko.query.renderdata.world
{
	import aerys.common.Factory;
	import aerys.minko.query.renderdata.style.StyleStack3D;
	import aerys.minko.query.renderdata.transform.TransformData;

	public class WorldDataList implements IWorldData
	{
		protected var _objects		: Vector.<IWorldData>;
		
		public function get length() : uint
		{
			return _objects.length;
		}
		
		public function set length(v : uint) : void
		{
			if (v != _objects.length)
			{
				_objects.length = v;
			}
		}
		
		public function WorldDataList(styleName : String = null)
		{
			_objects = new Vector.<IWorldData>();
		}
		
		public function setLocalDataProvider(styleStack		: StyleStack3D, 
											 transformData	: TransformData) : void
		{
			for each (var dataObject : IWorldData in _objects)
				dataObject.setLocalDataProvider(styleStack, transformData);
		}
		
		public function invalidate() : void
		{
			for each (var dataObject : IWorldData in _objects)
				dataObject.invalidate();
		}
		
		public function reset() : void
		{
			for each (var dataObject : IWorldData in _objects)
				dataObject.reset();
		}
		
		public function getItem(i : uint) : IWorldData
		{
			return _objects[i];
		}
		
		public function setItem(i : uint, value : IWorldData) : void
		{
			_objects[i] = value;
		}
		
		public function push(v : IWorldData) : void
		{
			_objects.push(v);
		}
		
		public function pop() : IWorldData
		{
			return _objects.pop();
		}
		
		public function clone() : WorldDataList
		{
			var newStyleObjList : WorldDataList = Factory.getFactory(WorldDataList).create(true);
			newStyleObjList._objects	= _objects.concat();
			return newStyleObjList;
		}
		
	}
}