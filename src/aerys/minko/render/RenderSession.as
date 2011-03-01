package aerys.minko.render
{
	import aerys.minko.render.state.RenderState;

	internal final class RenderSession
	{
		private var _renderState	: RenderState		= new RenderState();
		private var _drawCalls		: Vector.<DrawCall>	= new Vector.<DrawCall>();
		private var _next			: RenderSession		= null;
		
		public function get drawCalls()		: Vector.<DrawCall>	{ return _drawCalls; }
		public function get renderState()	: RenderState		{ return _renderState; }
		public function get next()			: RenderSession		{ return _next; }
		
		public function set next(value : RenderSession) : void
		{
			_next = value;
		}
		
		public function clear() : void
		{
			_drawCalls.length = 0;
			_next = null;
		}
	}
}