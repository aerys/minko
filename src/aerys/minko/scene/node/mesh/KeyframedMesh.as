package aerys.minko.scene.node.mesh
{
	import aerys.minko.render.effect.Style;
	import aerys.minko.scene.action.mesh.KeyframedMeshAction;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.scene.node.IStylableScene;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	
	public class KeyframedMesh extends AbstractScene implements IMesh, IStylableScene
	{
		private var _style					: Style;
		
		private var _frameId				: Number;
		
		private var _indexStream 			: IndexStream;
		private var _vertexStreams			: Vector.<IVertexStream>;
		private var _adjacentVertexStreams	: Vector.<IVertexStream>;
		
		public function get style()					: Style						{ return _style; }
		public function get styleEnabled()			: Boolean					{ return true; }
		
		public function get version()				: uint						{ return 0; }
		public function get indexStream()			: IndexStream				{ return _indexStream; }
		
		public function get vertexStreams()			: Vector.<IVertexStream>	{ return _vertexStreams; }
		
		public function get vertexStream()			: IVertexStream
		{
			return _vertexStreams[Math.floor(_frameId)];
		}
		
		public function get adjacentVertexStreams()	: Vector.<IVertexStream>
		{
			_adjacentVertexStreams[0]	= _vertexStreams[Math.floor(_frameId)];
			_adjacentVertexStreams[1]	= _vertexStreams[Math.ceil(_frameId)];
			return _adjacentVertexStreams;
		}
		
		public function get frameId() : Number
		{
			return _frameId;
		}

		public function set frameId(v : Number) : void
		{
			_frameId = v;
		}

		public function KeyframedMesh(vertexStreams : Vector.<IVertexStream>,
									  indexStream	: IndexStream = null)
		{
			super();
			
			_adjacentVertexStreams	= new Vector.<IVertexStream>(2, true);
			_indexStream			= indexStream;
			_vertexStreams			= vertexStreams;
			_frameId				= 0;
			
			actions[0]				= KeyframedMeshAction.keyframedMeshAction;
			
			if (!_indexStream)
				_indexStream = new IndexStream(null, _vertexStreams[0].length, _vertexStreams[0].dynamic);
		}
		
	}
}