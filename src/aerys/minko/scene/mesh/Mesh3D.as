package aerys.minko.scene.mesh
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.IScene3DVisitor;
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
		
		public function Mesh3D(myVertexBuffer	: VertexStream3D	= null,
							   myIndexBuffer	: IndexStream3D 	= null)
		{
			_vertexStream = myVertexBuffer;
			_indexStream = myIndexBuffer || IndexStream3D.dummy(_vertexStream.length);
			
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
		
		override public function visited(visitor : IScene3DVisitor) : void
		{
			var renderer : IRenderer3D = visitor.renderer;
			var context : Context3D = renderer.viewport.context;
			
			_indexStream.prepare(context);
			_vertexStream.prepareContext(context,
										 renderer.states.vertexFormat);
		}
		
		public function set vertexStream(value : VertexStream3D) : void
		{
			_vertexStream = value;
		}
	}
}