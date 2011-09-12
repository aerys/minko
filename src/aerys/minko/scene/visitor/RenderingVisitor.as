package aerys.minko.scene.visitor
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.RenderingData;
	import aerys.minko.scene.node.IScene;
	
	import flash.utils.Dictionary;
	
	public class RenderingVisitor implements ISceneVisitor
	{
		private static const ACTIONS_TYPES	: uint	= ~ActionType.UPDATE_WORLD_DATA;
		
		protected var _renderer			: IRenderer;
		protected var _numNodes			: uint;
		
		protected var _transformData		: TransformData;
		protected var _worldData		: Dictionary;
		protected var _renderingData	: RenderingData;
		
		protected var _parent			: IScene			= null;
		protected var _current			: IScene			= null;
		protected var _ancestors		: Vector.<IScene>	= new Vector.<IScene>();
		
		public function get transformData()		: TransformData			{ return _transformData; }
		public function get worldData()		: Dictionary		{ return _worldData; }
		public function get renderingData()	: RenderingData		{ return _renderingData; }
		public function get ancestors()		: Vector.<IScene>	{ return _ancestors; }
		public function get parent()		: IScene			{ return _parent; }
		
		public function get numNodes() 		: uint				{ return _numNodes;	}
		
		public function RenderingVisitor()
		{
		}
		
		public function processSceneGraph(scene			: IScene, 
										  transformData	: TransformData, 
										  worldData		: Dictionary, 
										  renderingData	: RenderingData,
										  renderer		: IRenderer) : void
		{
			_worldData		= worldData;
			_transformData	= transformData;
			_renderingData	= renderingData; 
			_renderer		= renderer;
			
			_renderer.clear();
			visit(scene);
			
			_renderer.drawToBackBuffer();
		}
		
		public function visit(scene : IScene) : void
		{
			var actions 	: Vector.<IAction> 	= scene.actions;
			var numActions	: int				= actions.length;
			var action		: IAction			= null;
			
//			_ancestors.push(_parent);
//			_parent = _current;
//			_current = scene;
			
			for (var i : int = 0; i < numActions; ++i)
				if (((action = actions[i]).type & ACTIONS_TYPES) && !action.run(scene, this, _renderer))
					break ;
			
			// update statistical data
			++_numNodes;
			
//			_current = _parent;
//			_parent = _ancestors.pop();
		}
	}
}