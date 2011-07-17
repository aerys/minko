package aerys.minko.scene.visitor
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.data.LocalData;
	import aerys.minko.scene.data.RenderingData;
	import aerys.minko.scene.data.StyleStack;
	
	import flash.utils.Dictionary;
	
	/**
	 *  
	 * @author Jean-Marc Le Roux
	 */
	public interface ISceneVisitor
	{
		function get localData() 		: LocalData;
		function get worldData() 		: Dictionary;
		function get renderingData()	: RenderingData;
		function get numNodes()			: uint;
		function get ancestors()		: Vector.<IScene>;
		
		function visit(scene : IScene) : void;
		
		function processSceneGraph(scene			: IScene, 
								   localData		: LocalData, 
								   worldData		: Dictionary, 
								   renderingData	: RenderingData,
								   renderer			: IRenderer) : void;
	}
}