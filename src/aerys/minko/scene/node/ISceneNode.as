package aerys.minko.scene.node
{
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.clone.CloneOptions;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	/**
	 * The ISceneNode interface describes the properties of every
	 * node in a 3D scene.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public interface ISceneNode
	{
		function get x() : Number;
		function get y() : Number;
		function get z() : Number;
		function get rotationX() : Number;
		function get rotationY() : Number;
		function get rotationZ() : Number;
		function get scaleX() : Number;
		function get scaleY() : Number;
		function get scaleZ() : Number;
		
		function get userData() : DataProvider;
		
		function set x(value : Number) : void;
		function set y(value : Number) : void;
		function set z(value : Number) : void;
		function set rotationX(value : Number) : void;
		function set rotationY(value : Number) : void;
		function set rotationZ(value : Number) : void;
		function set scaleX(value : Number) : void;
		function set scaleY(value : Number) : void;
		function set scaleZ(value : Number) : void;
		
        function get scene() : Scene;
        
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
		 * Recursively clone the scene node.
		 * 
		 * @param cloneOptions
		 * @return 
		 */		
		function clone(cloneOptions : CloneOptions = null) : ISceneNode;
		
		/**
		 * The name of the scene node. 
		 * @return 
		 * 
		 */
		function get name() : String;
		function set name(value : String) : void;
		
		function get transform() : Matrix4x4;
		
		function get localToWorldTransformChanged() : Signal;
		
		/**
		 * Return a copy of the current local to world transform of the scene node. 
		 * @param output
		 * @return 
		 * 
		 */
		function getLocalToWorldTransform(forceUpdate 	: Boolean 	= false,
										  output 		: Matrix4x4 = null) : Matrix4x4;
		
		/**
		 * Return a copy of the current world to local transform of the scene node. 
		 * @param output
		 * @return 
		 * 
		 */
		function getWorldToLocalTransform(forceUpdate 	: Boolean 	= false,
										  output 		: Matrix4x4 = null) : Matrix4x4;
		
		/**
		 * Transform a local space vector into world space.
		 * 
		 * @param vector
		 * @param output
		 * @return 
		 * 
		 */
		function localToWorld(inputVector						: Vector4,
							  outputVector						: Vector4 	= null,
                              skipTranslation                   : Boolean   = false,
							  forceLocalToWorldTransformUpdate	: Boolean	= false) : Vector4;
		
        function worldToLocal(inputVector						: Vector4,
                              outputVector						: Vector4 	= null,
                              skipTranslation                   : Boolean   = false,
                              forceWorldToLocalTransformUpdate	: Boolean	= false) : Vector4;
        
		/**
		 * The signal executed when the node (or one of its ancestors) is added to a parent scene
         * node. Callbacks functions must accept the following arguments:
		 * <ul>
		 * <li>node : ISceneNode, the scene node itself</li>
		 * <li>ancestor : Group, the ancestor the scene node has been added to</li>
		 * </ul>
		 * @return 
		 * 
		 */
		function get added() : Signal;
		
		/**
		 * The signal executed when the node (or one of its ancestors) is removed from a parent
         * scene node. Callbacks functions must accept the following arguments:
		 * <ul>
		 * <li>node : ISceneNode, the scene node itself</li>
		 * <li>ancestor : Group, the ancestor the scene node has been removed from</li>
		 * </ul>
		 * @return 
		 * 
		 */
		function get removed() : Signal;
		
		/**
		 * Get one of the scene node controllers by its index. 
		 * @param index
		 * @return 
		 * 
		 */
		function getController(index : uint) : AbstractController;
        
        function hasController(controller : AbstractController) : Boolean;
        
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