package aerys.minko.render
{
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	
	import flash.display3D.IndexBuffer3D;
	import flash.display3D.VertexBuffer3D;

	public final class DrawCall
	{
		private var _vertexBuffer	: VertexStream3D	= null;
		private var _indexBuffer	: IndexStream3D		= null;
		private var _offset			: int				= 0;
		private var _numTriangles	: int				= 0;

		public function get vertexBuffer()	: VertexStream3D	{ return _vertexBuffer; }
		public function get indexBuffer()	: IndexStream3D		{ return _indexBuffer; }
		public function get offset()		: int				{ return _offset; }
		public function get numTriangles()	: int				{ return _numTriangles; }
		
		public function initialize(vb 			: VertexStream3D,
								   ib 			: IndexStream3D,
								   offset 		: int,
								   numTriangles : int) : void
		{
			_vertexBuffer = vb;
			_indexBuffer = ib;
			_offset = offset;
			_numTriangles = numTriangles;
		}
	}
}