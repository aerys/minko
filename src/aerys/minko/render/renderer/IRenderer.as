package aerys.minko.render.renderer
{
	import aerys.minko.render.Viewport;
	
	import flash.display.BitmapData;

	public interface IRenderer
	{
		function get numTriangles()	: uint;
		function get viewport() 	: Viewport;
		function get drawingTime() 	: int;
		function get frameId()		: uint;
		
		function pushState(state : RendererState) : void;
		
		function drawTriangles(offset		: uint	= 0,
							   numTriangles	: int	= -1) : void;
		
		function reset() : void;
		
		function drawToBackBuffer() : void;
		function present() : void;
		function dumpBackbuffer(bitmapData : BitmapData) : void;
		
	}
}