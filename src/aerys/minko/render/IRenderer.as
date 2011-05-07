package aerys.minko.render
{
	import aerys.minko.stage.Viewport;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.state.RenderState;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.TextureBase;
	import flash.utils.ByteArray;

	public interface IRenderer
	{
		function get numTriangles()	: uint;
		function get state() 		: RenderState;
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
		
		function present() : void;
		
		function createTexture(width 		: uint,
							   height 		: uint,
							   format 		: String	= Context3DTextureFormat.BGRA,
							   renderTarget : Boolean 	= false) : TextureBase;
		
		function createShader(vertexShader 		: ByteArray,
							  fragmentShader 	: ByteArray) : Shader;
		
		function drawToBitmapData(bitmapData : BitmapData) : void;
		
	}
}