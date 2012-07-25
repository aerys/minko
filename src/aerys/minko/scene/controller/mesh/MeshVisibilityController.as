package aerys.minko.scene.controller.mesh
{
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.data.MeshVisibilityDataProvider;
	import aerys.minko.scene.node.Camera;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.type.Signal;
	import aerys.minko.type.bounding.FrustumCulling;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.binding.IDataProvider;
	import aerys.minko.type.math.Frustum;
	import aerys.minko.type.math.Matrix4x4;
	
	import avmplus.USE_ITRAITS;
	
	/**
	 * The MeshVisibilityController watches the Mesh and the active Camera of a Scene
	 * to determine whether the object is actually inside the view frustum or not.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class MeshVisibilityController extends AbstractController
	{
		private static const STATE_UNDEFINED	: uint	= 0;
		private static const STATE_INSIDE		: uint	= 1;
		private static const STATE_OUTSIDE		: uint	= 2;
		
		private var _mesh			: Mesh							= null;
		private var _state			: uint							= 0;
		
		private var _visibilityData	: MeshVisibilityDataProvider	= new MeshVisibilityDataProvider();
		
		public function get visible() : Boolean
		{
			return _visibilityData.visible;
		}
		public function set visible(value : Boolean) : void
		{
			_visibilityData.visible = value;
		}
		
		public function get frustumCulling() : uint
		{
			return _visibilityData.frustumCulling;
		}
		public function set frustumCulling(value : uint) : void
		{
			_visibilityData.frustumCulling = value;
		}
		
		public function get insideFrustum() : Boolean
		{
			return _visibilityData.inFrustum;
		}
		
		public function MeshVisibilityController()
		{
			super(Mesh);
			
			initialize();
		}
		
		private function initialize() : void
		{
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(ctrl	: MeshVisibilityController,
											target	: Mesh) : void
		{
			if (_mesh != null)
				throw new Error();

			_mesh = target;
			
			target.addedToScene.add(meshAddedToSceneHandler);
			target.removedFromScene.add(meshRemovedFromSceneHandler);
			target.bindings.addProvider(_visibilityData);
			
			if (target.root is Scene)
				meshAddedToSceneHandler(target, target.root as Scene);
		}
		
		private function targetRemovedHandler(ctrl		: MeshVisibilityController,
											  target	: Mesh) : void
		{
			_mesh = null;
			
			target.addedToScene.remove(meshAddedToSceneHandler);
			target.removedFromScene.remove(meshRemovedFromSceneHandler);
			target.bindings.removeProvider(_visibilityData);
			
			if (target.root is Scene)
				meshRemovedFromSceneHandler(target, target.root as Scene);
		}
		
		private function meshAddedToSceneHandler(mesh	: Mesh,
												 scene	: Scene) : void
		{
			scene.bindings.addCallback('worldToView', worldToViewChangedHandler);
			mesh.localToWorld.changed.add(meshLocalToWorldChangedHandler);
		}
		
		private function meshRemovedFromSceneHandler(mesh	: Mesh,
													 scene	: Scene) : void
		{
			mesh.localToWorld.changed.remove(meshLocalToWorldChangedHandler);
		}
		
		private function worldToViewChangedHandler(bindings		: DataBindings,
												   propertyName	: String,
												   oldValue		: Matrix4x4,
												   newValue		: Matrix4x4) : void
		{
			testCulling();
		}
		
		private function meshLocalToWorldChangedHandler(transform	: Matrix4x4,
														property	: String) : void
		{
			testCulling();
		}
		
		private function testCulling() : void
		{
			var culling	: uint	= _visibilityData.frustumCulling;
			
			if (culling != FrustumCulling.DISABLED)
			{
				var camera : Camera = (_mesh.root as Scene).activeCamera;
				
				if (!camera)
					return ;
				
				culling = camera.cameraData.frustum.testBoundingVolume(
					_mesh.geometry,
					_mesh.localToWorld,
					culling
				);
				
				var inside : Boolean = culling != Frustum.OUTSIDE;
				
				if (inside && _state != STATE_INSIDE)
				{
					_visibilityData.inFrustum = true;
					_state = STATE_INSIDE;
				}
				else if (!inside && _state != STATE_OUTSIDE)
				{
					_visibilityData.inFrustum = false;
					_state = STATE_OUTSIDE;
				}
			}
		}
		
		override public function clone() : AbstractController
		{
			var clone : MeshVisibilityController = new MeshVisibilityController();
			
			clone._visibilityData = _visibilityData.clone() as MeshVisibilityDataProvider;
			
			return clone;
		}
	}
}