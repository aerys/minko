package aerys.minko.scene.graph.mesh
{
	import aerys.minko.effect.basic.BasicStyle;
	import aerys.minko.ns.minko;
	import aerys.minko.scene.graph.AbstractScene;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStreamList;
	
	public class Mesh extends AbstractScene implements IMesh
	{
		use namespace minko;
		
		private static var _id : uint = 0;
		
		protected var _vertexStreamList	: VertexStreamList	= null;
		protected var _indexStream		: IndexStream		= null;
		
		public function get version() : uint
		{
			return _vertexStreamList.version + _indexStream.version;
		}
		
		public function get vertexStreamList() : VertexStreamList
		{
			return _vertexStreamList;
		}
		
		public function set vertexStreamList(value : VertexStreamList) : void
		{
			_vertexStreamList = value;
		}

		public function get indexStream() : IndexStream
		{
			return _indexStream;
		}
		
		public function set indexStream(value : IndexStream) : void
		{
			_indexStream = value;
		}

		public function Mesh(vertexStreamList	: VertexStreamList	= null,
							 indexStream		: IndexStream 		= null)
		{
			super();
			
			_vertexStreamList = vertexStreamList;
			_indexStream = indexStream;
			
			if (!_indexStream && _vertexStreamList)
				_indexStream = new IndexStream(null, vertexStreamList.length, vertexStreamList.dynamic);
		}
		
//		override protected function visitedByRenderingVisitor(query : RenderingVisitor) : void
//		{
//			var transform : TransformManager = query.transform;
//			
//			query.style.set(BasicStyle.WORLD_MATRIX, 			transform.world)
//				  	   .set(BasicStyle.VIEW_MATRIX, 			transform.view)
//				  	   .set(BasicStyle.PROJECTION_MATRIX,		transform.projection)
//				  	   .set(BasicStyle.LOCAL_TO_SCREEN_MATRIX, 	transform.getLocalToScreen());
//			
//			query.draw(vertexStreamList, indexStream);
//		}
		
	}
}