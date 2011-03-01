package aerys.minko.render
{
	import aerys.minko.Viewport3D;
	import aerys.minko.render.shader.Shader3D;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.TextureBase;
	import flash.geom.Rectangle;
	import flash.utils.ByteArray;

	public interface IRenderer3D
	{
		function get numTriangles()	: uint;
		function get states() 		: RenderState;
		function get viewport() 	: Viewport3D;
		function get drawingTime() 	: int;
		
		function get frameId() : uint;
		
		function setVertexStream(stream	: VertexStream3D,
								 offset	: int	= 0) : void;
		
		function drawTriangles(indexStream 	: IndexStream3D,
							   offset		: uint	= 0,
							   numTriangles	: uint	= 0) : void;
		
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
							  fragmentShader 	: ByteArray) : Shader3D;
		
		function drawToBitmapData(bitmapData : BitmapData) : void;
		
		function setScissorRectangle(rectangle : Rectangle) : void;
	}
}