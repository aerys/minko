package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;
	
	public class AbstractMeshModifier extends Mesh implements IMeshModifier
	{
		private var _target 			: IMesh				= null;
		
		public function get target() : IMesh
		{
			return _target;
		}
		
		override public function get version() : uint
		{
			return super.version + _target.version;
		}
				
		override public function get vertexStreamList() : VertexStreamList
		{
			return _vertexStreamList || (_target ? _target.vertexStreamList : null);
		}
		
		override public function get indexStream() : IndexStream
		{
			return _indexStream || (_target ? _target.indexStream : null);
		}
		
		public function AbstractMeshModifier(target : IMesh, ...streams)
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
									: new VertexStreamList();
				
				for (var i : int = 0; i < numStreams; ++i)
					_vertexStreamList.pushVertexStream(streams[i] as VertexStream);
			}
		}
		
	}
}