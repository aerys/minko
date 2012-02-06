package aerys.minko.render.shader
{
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.RendererState;
	
	import flash.utils.Dictionary;

	public interface IShader
	{
		function get name() : String;
		function get state() : RendererState;
		
		function createDrawCall() : DrawCall;
		
		function dispose() : void;
	}
}
