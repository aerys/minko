package aerys.minko.scene.visitor
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.RenderingData;
	import aerys.minko.scene.node.IScene;
	
	import flash.utils.Dictionary;
	
	/**
	 *  
	 * @author Jean-Marc Le Roux
	 */
	public interface ISceneVisitor
	{
		function get transformData() 		: TransformData;
		function get worldData() 		: Dictionary;
		function get renderingData()	: RenderingData;
		function get numNodes()			: uint;
		function get ancestors()		: Vector.<IScene>;
		
		function visit(scene : IScene) : void;
		
		function processSceneGraph(scene			: IScene, 
								   transformData		: TransformData, 
								   worldData		: Dictionary, 
								   renderingData	: RenderingData,
								   renderer			: IRenderer) : void;
	}
}