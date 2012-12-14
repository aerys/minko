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
            
            _numTargetsInScene = new Dictionary(true);
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		protected function targetAddedHandler(ctrl		: EnterFrameController,
											  target	: ISceneNode) : void
		{
			if (target.root is Scene)
				targetAddedToSceneHandler(target, target.root as Scene);
			else
				target.addedToScene.add(targetAddedToSceneHandler);
		}
		
		protected function targetRemovedHandler(ctrl	: EnterFrameController,
												target	: ISceneNode) : void
		{
			if (target.root is Scene)
				target.removedFromScene.remove(targetRemovedFromSceneHandler);
			else
				target.addedToScene.remove(targetAddedToSceneHandler);
		}
		
		protected function targetAddedToSceneHandler(target	: ISceneNode,
													 scene	: Scene) : void
		{
			if (target.addedToScene.hasCallback(targetAddedToSceneHandler))
				target.addedToScene.remove(targetAddedToSceneHandler);
			
			target.removedFromScene.add(targetRemovedFromSceneHandler);
			
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
			target.removedFromScene.remove(targetRemovedFromSceneHandler);
			target.addedToScene.add(targetAddedToSceneHandler);
			
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