package aerys.minko.render
{
	import aerys.minko.Viewport3D;
	import aerys.minko.effect.Effect3DStyle;
	import aerys.minko.render.shader.Shader3D;
	import aerys.minko.render.state.RenderStatesManager;
	import aerys.minko.transform.TransformManager;
	import aerys.minko.type.math.Matrix4x4;
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
		function get states() 		: RenderStatesManager;
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
		
		function beginRenderToTexture(texture 			: TextureBase,
									  depthAndStencil	: Boolean 	= false,
									  antiAliasing		: int		= 0,
									  surface			: int		= 0) : void;
		
		function endRenderToTexture() : void;
		
		function setMatrix(index 		: int,
						   programType	: String,
						   matrix 		: Matrix4x4,
						   transposed 	: Boolean = true) : void;
		
		function setConstants(firstRegister	: int,
							  programType	: String,
							  ...data) : void
		
		function setTexture(index 	: int,
							texture	: TextureBase) : void;
		
		function drawToBitmapData(bitmapData : BitmapData) : void;
		
		function setScissorRectangle(rectangle : Rectangle) : void;
	}
}