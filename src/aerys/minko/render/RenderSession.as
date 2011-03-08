package aerys.minko.render
{
	import aerys.minko.render.state.RenderState;

	internal final class RenderSession
	{
		private var _renderState	: RenderState		= null;
		private var _offset			: uint				= 0;
		private var _numTriangles	: uint				= 0;
		
		public function get renderState()	: RenderState		{ return _renderState; }
		public function get offset()		: uint				{ return _offset; }
		public function get numTriangles()	: uint				{ return _numTriangles; }
		
		public function set renderState(value : RenderState) : void
		{
			_renderState = value;
		}
		
		public function set offset(value : uint) : void
		{
			_offset = value;
		}
		
		public function set numTriangles(value : uint) : void
		{
			_numTriangles = value;
		}
	}
}