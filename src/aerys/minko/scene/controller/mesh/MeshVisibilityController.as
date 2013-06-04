package aerys.minko.scene.controller.mesh
{
	import flash.display.BitmapData;
	
	import aerys.minko.ns.minko_math;
	import aerys.minko.render.Viewport;
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.camera.AbstractCamera;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
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
        private static const TMP_VECTOR4_2		: Vector4	= new Vector4();
		
		private static const STATE_UNDEFINED	: uint		= 0;
		private static const STATE_INSIDE		: uint		= 1;
		private static const STATE_OUTSIDE		: uint		= 2;
		
		private var _mesh			    : Mesh;
		private var _state			    : uint;
		private var _lastTest		    : int;
		private var _boundingBox	    : BoundingBox;
		private var _boundingSphere	    : BoundingSphere;
		
		private var _data				: DataProvider;
		
        private var _frustumCulling     : uint;
        private var _insideFrustum      : Boolean;
		
		public function get frustumCulling() : uint
		{
			return _frustumCulling;
		}
		public function set frustumCulling(value : uint) : void
		{
			if (value != _frustumCulling)
			{
				_frustumCulling = value;
				if (value == FrustumCulling.DISABLED)
				{
					_data.computedVisibility = true;
					if (_mesh.localToWorldTransformChanged.hasCallback(meshLocalToWorldChangedHandler))
						_mesh.localToWorldTransformChanged.remove(meshLocalToWorldChangedHandler);
				}
				else if (_mesh && _mesh.scene)
				{
					_mesh.localToWorldTransformChanged.add(meshLocalToWorldChangedHandler);
	            	testCulling();
				}
			}
		}
		
		public function get insideFrustum() : Boolean
		{
			return _insideFrustum;
		}
        
        public function get computedVisibility() : Boolean
        {
            return _data.computedVisibility;
        }
		
		public function MeshVisibilityController()
		{
			super(Mesh);
			
			initialize();
		}
		
		private function initialize() : void
		{
			_data = new DataProvider(
				{ computedVisibility : true },
				'MeshVisibilityDataProvider',
				DataProviderUsage.EXCLUSIVE
			);
			
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
			
			target.added.add(addedHandler);
			target.removed.add(removedHandler);
			
			if (target.root is Scene)
				addedHandler(target, target.root as Scene);
			
			if (_frustumCulling)
				testCulling();
		}
		
		private function targetRemovedHandler(ctrl		: MeshVisibilityController,
											  target	: Mesh) : void
		{
			_mesh = null;
			
			target.added.remove(addedHandler);
			target.removed.remove(removedHandler);
			
			if (target.root is Scene)
				removedHandler(target, target.root as Scene);
		}
		
		private function addedHandler(mesh : Mesh, ancestor : Group) : void
		{
			var scene : Scene = ancestor.scene;
			
			if (!scene)
				return ;
			
			scene.bindings.addCallback('worldToScreen', worldToScreenChangedHandler);

            if (_frustumCulling)
                scene.renderingBegin.add(sceneRenderingBeginHandler);
            
            _mesh.computedVisibilityChanged.add(computedVisiblityChangedHandler);
            _mesh.bindings.addProvider(_data);
		}
        
        private function sceneRenderingBeginHandler(scene			: Scene,
                                                    viewport		: Viewport,
                                                    destination	    : BitmapData,
                                                    time			: Number) : void
        {
            scene.renderingBegin.remove(sceneRenderingBeginHandler);
            
            meshLocalToWorldChangedHandler(_mesh, _mesh.getLocalToWorldTransform());
            _mesh.localToWorldTransformChanged.add(meshLocalToWorldChangedHandler);
        }
        
		private function removedHandler(mesh : Mesh, ancestor : Group) : void
		{
			var scene : Scene = ancestor.scene;
			
			if (!scene)
				return ;

            if (scene.renderingBegin.hasCallback(sceneRenderingBeginHandler))
                scene.renderingBegin.remove(sceneRenderingBeginHandler);
            
			scene.bindings.removeCallback('worldToScreen', worldToScreenChangedHandler);
            
			if (_frustumCulling
                && mesh.localToWorldTransformChanged.hasCallback(meshLocalToWorldChangedHandler))
				mesh.localToWorldTransformChanged.remove(meshLocalToWorldChangedHandler);

				mesh.computedVisibilityChanged.remove(computedVisiblityChangedHandler);
            	mesh.bindings.removeProvider(_data);
		}
		
        private function computedVisiblityChangedHandler(node               : ISceneNode,
                                                         computedVisibility : Boolean) : void
        {
            _data.computedVisibility = computedVisibility;
        }
        
		private function worldToScreenChangedHandler(bindings	    : DataBindings,
													 bindingName	: String,
													 oldValue		: Matrix4x4,
                                                     value			: Matrix4x4) : void
		{
			testCulling();
		}
		
		private function meshLocalToWorldChangedHandler(mesh : Mesh, transform : Matrix4x4) : void
		{
			var geom 	: Geometry 	= _mesh.geometry;
			var culling	: uint		= _frustumCulling;
			
			if (!geom || !geom.boundingBox || !geom.boundingSphere || culling == FrustumCulling.DISABLED)
				return ;
			
			if (culling & FrustumCulling.BOX)
				transform.transformRawVectors(geom.boundingBox._vertices, _boundingBox._vertices);
			
			if (culling & FrustumCulling.SPHERE)
			{
				var center 	: Vector4 	= transform.transformVector(geom.boundingSphere.center, TMP_VECTOR4);
				var scale 	: Vector4 	= transform.deltaTransformVector(Vector4.ONE, TMP_VECTOR4_2);
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
			
			if (_mesh && _mesh.geometry && _mesh.geometry.boundingBox && _mesh.visible && _mesh.root is Scene)
			{
				var camera : AbstractCamera = (_mesh.root as Scene).activeCamera;
				
				if (!camera)
					return ;
				
				_lastTest = camera.frustum.testBoundingVolume(
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
                    _data.computedVisibility = _mesh.parent.computedVisibility && _mesh.visible;
                    _mesh.computedVisibilityChanged.execute(_mesh, computedVisibility);
					_state = STATE_INSIDE;
				}
				else if (!inside && _state != STATE_OUTSIDE)
				{
					_insideFrustum = false;
                    _data.computedVisibility = false;
                    _mesh.computedVisibilityChanged.execute(_mesh, false);
					_state = STATE_OUTSIDE;
				}
			}
		}
		
		override public function clone() : AbstractController
		{
			return new MeshVisibilityController();
		}
	}
}
