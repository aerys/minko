package aerys.minko.scene.controller
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	
	import flash.display.BitmapData;

	public class EnterFrameController extends AbstractController
	{
		private var _targetsInScene	: uint	= 0;
		
		public function EnterFrameController(targetType	: Class = null)
		{
			super(targetType);
			
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
			
			++_targetsInScene;
			
			if (_targetsInScene == 1)
				scene.enterFrame.add(sceneEnterFrameHandler);
		}
		
		protected function targetRemovedFromSceneHandler(target	: ISceneNode,
														 scene	: Scene) : void
		{
			--_targetsInScene;
			
			target.removedFromScene.remove(targetRemovedFromSceneHandler);
			target.addedToScene.add(targetAddedToSceneHandler);
			
			if (_targetsInScene == 0)
				scene.enterFrame.remove(sceneEnterFrameHandler);
		}
		
		protected function sceneEnterFrameHandler(scene			: Scene,
												  viewport		: Viewport,
												  destination	: BitmapData,
												  time			: Number) : void
		{
			throw new Error(
				"The method EnterFrameController.sceneEnterFrameHandler must be overriden."
			);
		}
	}
}