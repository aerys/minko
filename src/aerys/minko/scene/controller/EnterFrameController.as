package aerys.minko.scene.controller
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	
	import flash.display.BitmapData;
	import flash.utils.Dictionary;

	/**
	 * EnterFrameController are controllers triggered whenever the Scene.enterFrame
	 * signal is executed.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class EnterFrameController extends AbstractController
	{
		private var _numTargetsInScene	: Dictionary;

        protected function getNumTargetsInScene(scene : Scene) : uint
        {
            return _numTargetsInScene[scene];
        }
        
		public function EnterFrameController(targetType	: Class = null)
		{
			super(targetType);
            
            initialize();
		}
		
        private function initialize() : void
        {
            _numTargetsInScene = new Dictionary(true);
            
            targetAdded.add(targetAddedHandler);
            targetRemoved.add(targetRemovedHandler);
        }
        
		protected function targetAddedHandler(ctrl		: EnterFrameController,
											  target	: ISceneNode) : void
		{
            if (target.scene)
                targetAddedToScene(target, target.scene);
            
            target.added.add(addedHandler);
            target.removed.add(removedHandler);
		}
		
		protected function targetRemovedHandler(ctrl	: EnterFrameController,
												target	: ISceneNode) : void
		{
            if (target.scene)
                targetRemovedFromScene(target, target.scene);
            
            target.added.remove(addedHandler);
            target.removed.remove(removedHandler);
		}
		
		private function addedHandler(target	: ISceneNode,
									  ancestor	: Group) : void
		{
			var scene : Scene = ancestor.scene;
			
			if (!scene)
				return ;
			
			targetAddedToScene(target, scene);
		}
		
		protected function targetAddedToScene(target : ISceneNode, scene : Scene) : void
		{
            if (!_numTargetsInScene[scene])
            {
                _numTargetsInScene[scene] = 1;
                scene.enterFrame.add(sceneEnterFrameHandler);
            }
            else
                _numTargetsInScene[scene]++;
		}
		
		private function removedHandler(target		: ISceneNode,
										ancestor	: Group) : void
		{
			var scene : Scene = ancestor.scene;
			
			if (!scene)
				return ;
			
			targetRemovedFromScene(target, scene);
		}
		
		protected function targetRemovedFromScene(target : ISceneNode, scene : Scene) : void
		{
            _numTargetsInScene[scene]--;
            if (_numTargetsInScene[scene] == 0)
            {
                delete _numTargetsInScene[scene];
                scene.enterFrame.remove(sceneEnterFrameHandler);
            }
		}
        
		protected function sceneEnterFrameHandler(scene			: Scene,
												  viewport		: Viewport,
												  destination	: BitmapData,
												  time			: Number) : void
		{
			throw new Error(
				'The method EnterFrameController.sceneEnterFrameHandler must be overriden.'
			);
		}
	}
}