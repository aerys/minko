package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.scene.action.mesh.MeshAction;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;
	
	public class AbstractMeshModifier extends AbstractScene implements IMeshModifier
	{
		private var _target 			: IMesh				= null;
		private var _indexStream		: IndexStream		= null;
		private var _vertexStreamList	: VertexStreamList	= null;
		
		public function get target() : IMesh
		{
			return _target;
		}
		
		public function get version() : uint
		{
			return _target.version;
		}
				
		public function get vertexStream() : IVertexStream
		{
			return _vertexStreamList || (_target ? _target.vertexStream : null);
		}
		
		public function get indexStream() : IndexStream
		{
			return _indexStream || (_target ? _target.indexStream : null);
		}
		
		public function AbstractMeshModifier(target : IMesh, ...streams)
		{
			super();
			
			_target = target;
			
			initialize(streams);
			
			//actions[0] = new MeshModifierAction();
			actions[0] = MeshAction.meshAction;
		}
	
		private function initialize(streams : Array) : void
		{
			var numStreams : int = streams.length;
			
			_vertexStreamList = target.vertexStream is VertexStreamList
								? (target.vertexStream as VertexStreamList).clone()
								: new VertexStreamList(_target.vertexStream);
			
			for (var i : int = 0; i < numStreams; ++i)
				_vertexStreamList.pushVertexStream(streams[i] as VertexStream);
		}
		
	}
}