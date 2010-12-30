package aerys.minko.render.renderer
{
	import aerys.minko.render.Viewport3D;
	import aerys.minko.render.state.RenderStatesManager;
	import aerys.minko.render.transform.TransformManager;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	
	import flash.display3D.textures.Texture;
	import flash.display3D.textures.TextureBase;

	public interface IRenderer3D
	{
		function get numTriangles()	: uint;
		function get states() 		: RenderStatesManager;
		function get transform() 	: TransformManager;
		function get viewport() 	: Viewport3D;
		function get textures() 	: Vector.<TextureBase>;
		function get drawingTime() 	: int;
		function get isReady() 		: Boolean;
		
		function setVertexStream(stream	: VertexStream3D,
								 offset	: int	= 0) : void;
		
		function drawTriangles(indexStream 	: IndexStream3D,
							   offset		: uint	= 0,
							   numTriangles	: uint	= -1) : void;
		
		function clear(color : uint = 0xff000000) : void;
		
		function present() : void;
		
		function createTexture(width 		: uint,
							   height 		: uint,
							   type 		: String,
							   optimized 	: Boolean = false) : Texture;
		
		function beginRenderToTexture(texture 			: TextureBase,
								 	  depthAndStencil 	: Boolean = true) : void;
		
		function endRenderToTexture() : void;
		
		function setMatrix(index 		: int,
						   matrix 		: Matrix4x4,
						   programType	: String,
						   transposed 	: Boolean = true) : void;
		
		function setConstants(firstRegister	: int,
							  data			: Vector.<Number>,
							  programType	: String,
							  numRegisters	: int = -1) : void
		
		function setTexture(index 	: int,
							texture	: TextureBase) : void;
	}
}