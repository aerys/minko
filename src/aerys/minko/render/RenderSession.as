package aerys.minko.render
{
	import aerys.minko.render.state.RenderState;

	internal final class RenderSession
	{
		private var _renderState	: RenderState		= null;
		private var _offsets		: Vector.<uint>		= new Vector.<uint>();
		private var _numTriangles	: Vector.<int>		= new Vector.<int>();
		
		public function get renderState()	: RenderState		{ return _renderState; }
		public function get offsets()		: Vector.<uint>		{ return _offsets; }
		public function get numTriangles()	: Vector.<int>		{ return _numTriangles; }
		
		public function set renderState(value : RenderState) : void
		{
			_renderState = value;
		}
	}
}