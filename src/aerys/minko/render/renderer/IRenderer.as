package aerys.minko.render.renderer
{
	import aerys.minko.render.Viewport;
	import aerys.minko.render.renderer.state.RendererState;
	
	import flash.display.BitmapData;

	public interface IRenderer
	{
		function get numTriangles()	: uint;
		function get state() 		: RendererState;
		function get viewport() 	: Viewport;
		function get drawingTime() 	: int;
		function get frameId()		: uint;
		
		function begin() : void;
		function end() : void;
		
		function drawTriangles(offset		: uint	= 0,
							   numTriangles	: int	= -1) : void;
		
		function clear(red 		: Number	= 0.,
					   green 	: Number	= 0.,
					   blue 	: Number	= 0.,
					   alpha 	: Number	= 0.,
					   depth 	: Number	= 1.,
					   stencil	: uint		= 0,
					   mask		: uint		= 0xffffffff) : void;
		
		function drawToBackBuffer() : void;
		
		function present() : void;
		
		function dumpBackbuffer(bitmapData : BitmapData) : void;
		
	}
}