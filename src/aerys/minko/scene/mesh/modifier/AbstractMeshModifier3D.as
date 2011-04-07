package aerys.minko.scene.mesh.modifier
{
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.scene.AbstractScene3D;
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.scene.mesh.Mesh3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.stream.VertexStream3DList;
	
	public class AbstractMeshModifier3D extends Mesh3D implements IMeshModifier3D
	{
		private var _target 			: IMesh3D				= null;
		
		public function get target() : IMesh3D
		{
			return _target;
		}
		
		override public function get version() : uint
		{
			return super.version + _target.version;
		}
				
		override public function get vertexStreamList() : VertexStream3DList
		{
			return _vertexStreamList || (_target ? _target.vertexStreamList : null);
		}
		
		override public function get indexStream() : IndexStream3D
		{
			return _indexStream || (_target ? _target.indexStream : null);
		}
		
		/*override public function accept(query : IScene3DQuery) : void
		{
			query.query(_target);
		}*/
		
		public function AbstractMeshModifier3D(target : IMesh3D, ...streams)
		{
			super();
			
			_target = target;
			
			initialize(streams);
		}
		
		private function initialize(streams : Array) : void
		{
			var numStreams : int = streams.length;
			
			if (numStreams != 0)
			{
				_vertexStreamList = _target
									? _target.vertexStreamList.clone()
									: new VertexStream3DList();
				
				for (var i : int = 0; i < numStreams; ++i)
					_vertexStreamList.pushVertexStream(streams[i] as VertexStream3D);
			}
		}
		
	}
}