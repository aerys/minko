package aerys.minko.scene.mesh
{
	import aerys.minko.ns.minko;
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.query.RenderingQuery;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.scene.AbstractScene3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	
	import flash.display3D.Context3D;
	
	public class Mesh3D extends AbstractScene3D implements IMesh3D
	{
		use namespace minko;
		
		private static var _id		: uint				= 0;
		
		protected var _vertexStream	: VertexStream3D	= null;
		protected var _indexStream	: IndexStream3D		= null;
		
		public function get version() : uint
		{
			return _vertexStream.version + _indexStream.version;
		}
		
		public function Mesh3D(vertexBuffer	: VertexStream3D	= null,
							   indexBuffer	: IndexStream3D 	= null)
		{
			_vertexStream = vertexBuffer;
			_indexStream = indexBuffer || IndexStream3D.dummy(_vertexStream.length, _vertexStream._dynamic);
			
			_name = "Mesh_" + ++_id;
		}
		
		public function get vertexStream() : VertexStream3D
		{
			return _vertexStream;
		}
		
		public function get indexStream() : IndexStream3D
		{
			return _indexStream;
		}
		
		override public function accept(query : IScene3DQuery) : void
		{
			// NOTHING
		}
		
		public function set vertexStream(value : VertexStream3D) : void
		{
			_vertexStream = value;
		}
	}
}