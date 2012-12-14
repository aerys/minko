package aerys.minko.scene.controller.mesh
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
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
	public final class MeshVisibilityController extends AbstractController
	{
		use namespace minko_math;
		
		private static const TMP_VECTOR4		: Vector4	= new Vector4();
		
		private static const STATE_UNDEFINED	: uint		= 0;
		private static const STATE_INSIDE		: uint		= 1;
		private static const STATE_OUTSIDE		: uint		= 2;
		
		private var _mesh			    : Mesh;
		private var _state			    : uint;
		private var _lastTest		    : int;
		private var _boundingBox	    : BoundingBox;
		private var _boundingSphere	    : BoundingSphere;
		
        private var _frustumCulling     : uint;
        private var _insideFrustum      : Boolean;
        private var _computedVisibility : Boolean;
		
		public function get frustumCulling() : uint
		{
			return _frustumCulling;
		}
		public function set frustumCulling(value : uint) : void
		{
			_frustumCulling = value;
            testCulling();
		}
		
		public function get insideFrustum() : Boolean
		{
			return _insideFrustum;
		}
        
        public function get computedVisibility() : Boolean
        {
            return frustumCulling == FrustumCulling.DISABLED || _computedVisibility;
        }
		
		public function MeshVisibilityController()
		{
			super(Mesh);
			
			initialize();
		}
		
		private function initialize() : void
		{
			_boundingBox = new BoundingBox(Vector4.ZERO, Vector4.ONE);
			_boundingSphere = new BoundingSphere(Vector4.ZERO, 0.);
			
            _state = STATE_UNDEFINED;
			
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
			
			if (target.root is Scene)
				meshAddedToSceneHandler(target, target.root as Scene);
		}
		
		private function targetRemovedHandler(ctrl		: MeshVisibilityController,
											  target	: Mesh) : void
		{
			_mesh = null;
			
			target.addedToScene.remove(meshAddedToSceneHandler);
			target.removedFromScene.remove(meshRemovedFromSceneHandler);
			
			if (target.root is Scene)
				meshRemovedFromSceneHandler(target, target.root as Scene);
		}
		
		private function meshAddedToSceneHandler(mesh	: Mesh, scene	: Scene) : void
		{
			scene.bindings.addCallback('worldToScreen', worldToScreenChangedHandler);
            
            meshLocalToWorldChangedHandler(mesh.localToWorld);
			mesh.localToWorld.changed.add(meshLocalToWorldChangedHandler);
            mesh.visibilityChanged.add(visiblityChangedHandler);
            mesh.parent.computedVisibilityChanged.add(visiblityChangedHandler);
            mesh.removed.add(meshRemovedHandler);
		}
		
        private function meshRemovedHandler(mesh : Mesh, ancestor : Group) : void
        {
            if (!mesh.parent)
                ancestor.computedVisibilityChanged.remove(visiblityChangedHandler);
        }
        
		private function meshRemovedFromSceneHandler(mesh : Mesh, scene	: Scene) : void
		{
			scene.bindings.removeCallback('worldToScreen', worldToScreenChangedHandler);
            
			mesh.localToWorld.changed.remove(meshLocalToWorldChangedHandler);
            mesh.visibilityChanged.remove(visiblityChangedHandler);
            mesh.removed.remove(meshRemovedHandler);
		}
		
        private function visiblityChangedHandler(node : ISceneNode, visibility : Boolean) : void
        {
            _computedVisibility = _mesh.visible && _mesh.parent.computedVisibility
                && _insideFrustum;
            _mesh.computedVisibilityChanged.execute(_mesh, _computedVisibility);
        }
        
		private function worldToScreenChangedHandler(bindings	    : DataBindings,
                                                     propertyName	: String,
                                                     oldValue		: Matrix4x4,
                                                     newValue		: Matrix4x4) : void
		{
			testCulling();
		}
		
		private function meshLocalToWorldChangedHandler(transform : Matrix4x4) : void
		{
			var geom 	: Geometry 	= _mesh.geometry;
			var culling	: uint		= _frustumCulling;
			
			if (!geom.boundingBox || !geom.boundingSphere || culling == FrustumCulling.DISABLED)
				return ;
			
			if (culling & FrustumCulling.BOX)
				transform.transformRawVectors(geom.boundingBox._vertices, _boundingBox._vertices);
			
			if (culling & FrustumCulling.SPHERE)
			{
				var center 	: Vector4 	= transform.transformVector(geom.boundingSphere.center);
				var scale 	: Vector4 	= transform.deltaTransformVector(Vector4.ONE);
				var radius	: Number	= geom.boundingSphere.radius * Math.max(
					Math.abs(scale.x), Math.abs(scale.y), Math.abs(scale.z)
				);
				
				_boundingSphere.update(center, radius);
			}
			
			testCulling();
		}
		
		private function testCulling() : void
		{
			var culling	: uint	= _frustumCulling;
			
			if (culling != FrustumCulling.DISABLED && _mesh && _mesh.geometry.boundingBox
                && _mesh.visible)
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
					_insideFrustum = true;
                    _computedVisibility = _mesh.parent.computedVisibility && _mesh.visible;
                    _mesh.computedVisibilityChanged.execute(_mesh, computedVisibility);
					_state = STATE_INSIDE;
				}
				else if (!inside && _state != STATE_OUTSIDE)
				{
					_insideFrustum = false;
                    _computedVisibility = false;
                    _mesh.computedVisibilityChanged.execute(_mesh, false);
					_state = STATE_OUTSIDE;
				}
			}
		}
		
		override public function clone() : AbstractController
		{
			return new MeshVisibilityController();;
		}
	}
}
