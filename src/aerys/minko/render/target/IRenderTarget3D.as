package aerys.minko.render.target
{
	import aerys.minko.render.IRenderer3D;
	
	import flash.display3D.textures.TextureBase;

	public interface IRenderTarget3D
	{
		function get size()		: uint;
		function get texture() 	: TextureBase;
		
		function prepare(renderer : IRenderer3D) : void;
	}
}