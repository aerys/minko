package aerys.minko.scene.controller
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	
	import flash.display.BitmapData;
	import flash.utils.Dictionary;

	/**
	 * EnterFrameController are controllers triggered whenever the Scene.enterFrame
	 * signal is executed.
	 * 
	 * The best way to 
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
                targetAddedToSceneHandler(target, target.scene);
            
            target.addedToScene.add(targetAddedToSceneHandler);
            target.removedFromScene.add(targetRemovedFromSceneHandler);
		}
		
		protected function targetRemovedHandler(ctrl	: EnterFrameController,
												target	: ISceneNode) : void
		{
            if (target.scene)
                targetRemovedFromSceneHandler(target, target.scene);
            
            target.addedToScene.remove(targetAddedToSceneHandler);
            target.removedFromScene.remove(targetRemovedFromSceneHandler);
		}
		
		protected function targetAddedToSceneHandler(target	: ISceneNode,
													 scene	: Scene) : void
		{
            if (!_numTargetsInScene.hasOwnProperty(scene))
            {
                _numTargetsInScene[scene] = 1;
                scene.enterFrame.add(sceneEnterFrameHandler);
            }
            else
                _numTargetsInScene[scene]++;
		}
		
		protected function targetRemovedFromSceneHandler(target	: ISceneNode,
														 scene	: Scene) : void
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