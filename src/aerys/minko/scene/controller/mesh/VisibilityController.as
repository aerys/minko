package aerys.minko.scene.controller.mesh
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.data.MeshVisibilityDataProvider;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.camera.AbstractCamera;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.enum.FrustumCulling;
	import aerys.minko.type.math.BoundingBox;
	import aerys.minko.type.math.BoundingSphere;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	
	/**
	 * The MeshVisibilityController watches the Mesh and the active Camera of a Scene
	 * to determine whether the object is actually inside the view frustum or not.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class VisibilityController extends AbstractController
	{
		use namespace minko_math;
		
		private static const TMP_VECTOR4		: Vector4	= new Vector4();
		
		private static const STATE_UNDEFINED	: uint		= 0;
		private static const STATE_INSIDE		: uint		= 1;
		private static const STATE_OUTSIDE		: uint		= 2;
		
		private var _mesh			: Mesh;
		private var _state			: uint;
		private var _lastTest		: int;
		private var _boundingBox	: BoundingBox;
		private var _boundingSphere	: BoundingSphere;
		
		private var _visibilityData	: MeshVisibilityDataProvider;
		
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
		
		public function VisibilityController()
		{
			super(Mesh);
			
			initialize();
		}
		
		private function initialize() : void
		{
			_boundingBox = new BoundingBox(Vector4.ZERO, Vector4.ONE);
			_boundingSphere = new BoundingSphere(Vector4.ZERO, 0.);
			
			_visibilityData = new MeshVisibilityDataProvider();
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(ctrl	: VisibilityController,
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
		
		private function targetRemovedHandler(ctrl		: VisibilityController,
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
			scene.bindings.addCallback('worldToScreen', worldToViewChangedHandler);
			mesh.localToWorld.changed.add(meshLocalToWorldChangedHandler);
		}
		
		private function meshRemovedFromSceneHandler(mesh	: Mesh,
													 scene	: Scene) : void
		{
			scene.bindings.removeCallback('worldToScreen', worldToViewChangedHandler);
			mesh.localToWorld.changed.remove(meshLocalToWorldChangedHandler);
		}
		
		private function worldToViewChangedHandler(bindings		: DataBindings,
												   propertyName	: String,
												   oldValue		: Matrix4x4,
												   newValue		: Matrix4x4) : void
		{
			testCulling();
		}
		
		private function meshLocalToWorldChangedHandler(transform : Matrix4x4) : void
		{
			var geom 	: Geometry 	= _mesh.geometry;
			var culling	: uint		= _visibilityData.frustumCulling;
			
			if (!geom.boundingBox || !geom.boundingSphere)
				return ;
			
			if (culling & FrustumCulling.BOX)
				transform.transformRawVectors(
					geom.boundingBox._vertices,
					_boundingBox._vertices
				);
			
			if (culling & FrustumCulling.SPHERE)
			{
				var center 	: Vector4 = transform.transformVector(geom.boundingSphere.center);
				var scale 	: Vector4 = transform.deltaTransformVector(Vector4.ONE, TMP_VECTOR4);
				
				_boundingSphere.update(
					center,
					geom.boundingSphere.radius * Math.max(scale.x, scale.y, scale.z)
				);
			}
			
			testCulling();
		}
		
		private function testCulling() : void
		{
			var culling	: uint	= _visibilityData.frustumCulling;
			
			if (culling != FrustumCulling.DISABLED && _mesh.geometry.boundingBox)
			{
				var camera : AbstractCamera = (_mesh.root as Scene).activeCamera;
				
				if (!camera)
					return ;
				
				_lastTest = camera.cameraData.frustum.testBoundingVolume(
					_boundingSphere,
					_boundingBox,
					null,
					culling,
					_lastTest
				);
				
				var inside : Boolean = _lastTest == -1;
				
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
			var clone : VisibilityController = new VisibilityController();
			
			clone._visibilityData = _visibilityData.clone() as MeshVisibilityDataProvider;
			
			return clone;
		}
	}
}
