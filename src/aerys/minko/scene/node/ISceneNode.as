package aerys.minko.scene.node
{
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.type.Signal;
	import aerys.minko.type.math.Matrix4x4;

	/**
	 * The ISceneNode interface describes the properties of every
	 * node in a 3D scene.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public interface ISceneNode
	{
		/**
		 * The root of the scene or the node itself if it has no parent.
		 * 
		 * @return 
		 * 
		 */
		function get root() : ISceneNode;
		/**
		 * The parent of the node or null if the node has no parent. 
		 * @return 
		 * 
		 */
		function get parent() : Group;
		function set parent(value : Group) : void;
		
		/**
		 * Clone the scene node, and its children if relevant. Geometries and textures are not cloned.
		 * 
		 * @return 
		 * 
		 */		
		function clone(cloneControllers : Boolean = false) : ISceneNode;
		
		/**
		 * The name of the scene node. 
		 * @return 
		 * 
		 */
		function get name() : String;
		function set name(value : String) : void;
		
		function get transform() : Matrix4x4;
		function get localToWorld() : Matrix4x4;
		function get worldToLocal() : Matrix4x4;
		
		/**
		 * The signal executed when the node is added to a parent scene node. 
		 * Callbacks functions must accept the following arguments:
		 * <ul>
		 * <li>node : ISceneNode, the scene node itself</li>
		 * <li>parent : Group, the new parent of the scene node</li>
		 * </ul>
		 * @return 
		 * 
		 */
		function get added() : Signal;
		
		function get addedToScene() : Signal;
		
		/**
		 * The signal executed when the node is removed from a parent scene node.
		 * Callbacks functions must accept the following arguments:
		 * <ul>
		 * <li>node : ISceneNode, the scene node itself</li>
		 * <li>parent : Group, the former parent of the scene node</li>
		 * </ul>
		 * @return 
		 * 
		 */
		function get removed() : Signal;
		
		function get removedFromScene() : Signal;
		
		/**
		 * Get one of the scene node controllers by its index. 
		 * @param index
		 * @return 
		 * 
		 */
		function getController(index : uint) : AbstractController;
		
		function getControllersByType(type			: Class,
									  controllers	: Vector.<AbstractController> = null) : Vector.<AbstractController>;
		/**
		 * Add a controller to the scene node. 
		 * @param controller
		 * 
		 */
		function addController(controller : AbstractController) : ISceneNode;
		/**
		 * Remove a controller from the scene node. 
		 * @param controller
		 * 
		 */
		function removeController(controller : AbstractController) : ISceneNode;
		
		/**
		 * Remove all controllers from the scene node. 
		 * @return 
		 * 
		 */
		function removeAllControllers() : ISceneNode;
		
		/**
		 * The number of controllers targeting the scene node. 
		 * @return 
		 * 
		 */
		function get numControllers() : uint;
		/**
		 * The signal executed when a controller is added to the scene node. 
		 * Callbacks functions must accept the following arguments:
		 * <ul>
		 * <li>node : ISceneNode, the scene node itself</li>
		 * <li>controller : AbstractController, the new controller added to the scene node</li>
		 * </ul>
		 * @return 
		 * 
		 */
		function get controllerAdded() : Signal;
		/**
		 * The signal executed when a controller is removed to the scene node.
		 * Callbacks functions must accept the following arguments:
		 * <ul>
		 * <li>node : ISceneNode, the scene node itself</li>
		 * <li>controller : AbstractController, the controller removed from the scene node</li>
		 * </ul>
		 * @return 
		 * 
		 */
		function get controllerRemoved() : Signal;
	}
}