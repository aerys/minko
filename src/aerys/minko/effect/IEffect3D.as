package aerys.minko.effect
{
	import aerys.minko.render.IRenderer3D;

	public interface IEffect3D extends IStyled3D
	{
		function getTechnique(name : String) : IEffect3DTechnique;
		
		function set currentTechnique(value : IEffect3DTechnique) : void;
		function get currentTechnique() : IEffect3DTechnique;
		
		function begin(renderer : IRenderer3D, style : IEffect3DStyle) : void;
		function end(renderer : IRenderer3D, style : IEffect3DStyle) : void;
	}
}