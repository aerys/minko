package aerys.minko.scene.visitor
{
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.scene.visitor.data.RenderingData;
	import aerys.minko.scene.visitor.data.StyleStack;
	
	import flash.utils.Dictionary;
	

	/**
	 * 
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public interface ISceneVisitor
	{
		function get localData() 		: LocalData;
		function get worldData() 		: Dictionary;
		function get renderingData()	: RenderingData;
		
		function visit(scene : IScene) : void;
	}
}