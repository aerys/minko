package aerys.minko.scene.visitor.rendering
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.data.CameraData;
	import aerys.minko.scene.visitor.data.IWorldData;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.scene.visitor.data.RenderingData;
	import aerys.minko.scene.visitor.data.ViewportData;
	
	import flash.utils.Dictionary;
	
	public class RenderingVisitor implements ISceneVisitor
	{
		private static const ACTIONS_TYPES	: uint	= ~ActionType.UPDATE_WORLD_DATA;
		
		protected var _renderer			: IRenderer;
		protected var _numNodes			: uint;
		
		protected var _localData		: LocalData;
		protected var _worldData		: Dictionary;
		protected var _renderingData	: RenderingData;
		
		public function get localData()		: LocalData			{ return _localData; }
		public function get worldData()		: Dictionary		{ return _worldData; }
		public function get renderingData()	: RenderingData		{ return _renderingData; }
		
		public function get numNodes() : uint
		{
			return _numNodes;
		}
		
		public function RenderingVisitor()
		{
		}
		
		public function processSceneGraph(scene			: IScene, 
										  localData		: LocalData, 
										  worldData		: Dictionary, 
										  renderingData	: RenderingData,
										  renderer		: IRenderer) : void
		{
			_worldData		= worldData;
			_localData		= localData;
			_renderingData	= renderingData; 
			_renderer		= renderer;
			
			_renderer.clear();
			
			for each (var worldObject : IWorldData in worldData)
				worldObject.setDataProvider(_renderingData.styleStack, _localData, _worldData);
			
			// update our transformManager if there is a camera, or
			// set it to null to render to screenspace otherwise
			var cameraData : CameraData = worldData[CameraData] as CameraData;
			if (cameraData)
			{
				_localData.view			= cameraData.view;
				_localData.projection	= cameraData.projection;
			}
			
			visit(scene);
			
			_renderer.drawToBackBuffer();
			_renderer.present();
		}
		
//		public function reset(defaultEffect : IEffect, color : int = 0) : void
//		{
//			_renderer.clear(((color >> 16) & 0xff) / 255.,
//							((color >> 8) & 0xff) / 255.,
//							(color & 0xff) / 255.);
//			
//			_worldData		= null;
//			_numNodes		= 0;
//			
//			_renderingData.clear(defaultEffect);
//		}
		
		public function visit(scene : IScene) : void
		{
			var actions 	: Vector.<IAction> 	= scene.actions;
			var numActions	: int				= actions.length;
			var	i			: int				= 0;
			var action		: IAction			= null;
			
			for (i = 0; i < numActions; ++i)
				if (((action = actions[i]).type & ACTIONS_TYPES) && !action.prefix(scene, this, _renderer))
					break ;
			
			for (i = 0; i < numActions; ++i)
				if (((action = actions[i]).type & ACTIONS_TYPES) && !action.infix(scene, this, _renderer))
					break ;
			
			for (i = 0; i < numActions; ++i)
				if (((action = actions[i]).type & ACTIONS_TYPES) && !action.postfix(scene, this, _renderer))
					break ;
			
			// update statistical data
			++_numNodes;
		}
	}
}