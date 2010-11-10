package aerys.minko.render
{
	import aerys.minko.render.state.RenderStatesManager;
	import aerys.minko.render.transform.TransformManager;
	import aerys.minko.type.stream.IndexStream3D;
	
	import flash.display3D.TextureBase3D;

	public interface IRenderer3D
	{
		function get numTriangles()	: uint;
		function get states() : RenderStatesManager;
		function get transform() : TransformManager;
		function get viewport() : Viewport3D;
		function get textures() : Vector.<TextureBase3D>;
		
		function drawTriangles(myIndexStream 	: IndexStream3D,
							   myOffset			: uint	= 0,
							   myNumTriangles	: uint	= -1) : void;
		
		function clear(color : uint = 0) : void;
		
		function present() : void;
	}
}