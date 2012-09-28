package aerys.minko.scene.controller.mesh.skinning
{
	import aerys.minko.Minko;
	import aerys.minko.ns.minko_math;
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.EnterFrameController;
	import aerys.minko.scene.controller.IRebindableController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.animation.SkinningMethod;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.log.DebugLevel;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.display.BitmapData;
	import flash.geom.Matrix3D;
	import flash.utils.Dictionary;
	
	public final class SkinningController extends EnterFrameController implements IRebindableController
	{
		public static const MAX_NUM_INFLUENCES	: uint	= 8;
		public static const MAX_NUM_JOINTS		: uint	= 51;
		
		private var _skinningHelper		: AbstractSkinningHelper;
		private var _isDirty			: Boolean;
		
		private var _method				: uint;
		private var _bindShapeMatrix	: Matrix3D;
		private var _skeletonRoot		: Group;
		private var _joints				: Vector.<Group>;
		private var _invBindMatrices	: Vector.<Matrix3D>;
		
		private var _numVisibleTargets	: uint;
		
		public function get skinningMethod() : uint
		{
			return _method;
		}
		
		public function get bindShape() : Matrix4x4
		{
			var bindShape : Matrix4x4 = new Matrix4x4();
			bindShape.minko_math::_matrix = _bindShapeMatrix;
			
			return bindShape;
		}
		
		public function get skeletonRoot() : Group
		{
			return _skeletonRoot;
		}
		
		public function get joints() : Vector.<Group>
		{
			return _joints;
		}
		
		public function get invBindMatrices() : Vector.<Matrix4x4>
		{
			var numMatrices		: uint					= _invBindMatrices.length;
			var invBindMatrices : Vector.<Matrix4x4>	= new Vector.<Matrix4x4>(numMatrices);
			
			for (var matrixId : uint = 0; matrixId < numMatrices; ++matrixId)
			{
				invBindMatrices[matrixId] = new Matrix4x4();
				invBindMatrices[matrixId].minko_math::_matrix = _invBindMatrices[matrixId];
			}
			
			return invBindMatrices;
		}
		
		public function SkinningController(method			: uint,
										   skeletonRoot		: Group,
										   joints			: Vector.<Group>,
										   bindShape		: Matrix4x4,
										   invBindMatrices	: Vector.<Matrix4x4>)
		{
			super(Mesh);
			
			_method				= method;
			_skeletonRoot		= skeletonRoot;
			_bindShapeMatrix	= bindShape.minko_math::_matrix;
			_isDirty			= false;
			
			initialize(joints, invBindMatrices);
		}
		
		private function initialize(joints			: Vector.<Group>,
									invBindMatrices	: Vector.<Matrix4x4>) : void
		{
			var numJoints : uint = joints.length;
			
			_joints	= joints.slice();
			
			_invBindMatrices	= new Vector.<Matrix3D>(numJoints, true);
			for (var jointId : uint = 0; jointId < numJoints; ++jointId)
				_invBindMatrices[jointId] = invBindMatrices[jointId].minko_math::_matrix;
		}
		
		override protected function targetAddedToSceneHandler(target	: ISceneNode,
															  scene		: Scene) : void
		{
			super.targetAddedToSceneHandler(target, scene);
			
			if (numTargetsInScene == 1)
				subscribeToJoints();
			
			var mesh : Mesh = target as Mesh;
			
			mesh.bindings.addCallback('inFrustum', meshVisibilityChangedHandler);
			
			if (!_skinningHelper)
			{
				var numInfluences 	: uint = AbstractSkinningHelper.getNumInfluences(mesh.geometry.format);
				
				if (_joints.length > MAX_NUM_JOINTS || numInfluences > MAX_NUM_INFLUENCES)
				{
					_method = SkinningMethod.SOFTWARE_MATRIX;
					Minko.log(
						DebugLevel.SKINNING,
						'Too many influences/joints: switching to SkinningMethod.SOFTWARE.'
					);
				}
				
				if (skinningMethod == SkinningMethod.SOFTWARE_MATRIX)
					_skinningHelper = new SoftwareSkinningHelper(
						_method, _bindShapeMatrix, _invBindMatrices
					);
				else
					_skinningHelper = new HardwareSkinningHelper(
						_method, _bindShapeMatrix, _invBindMatrices
					);
			}
			
			_skinningHelper.addMesh(mesh);
			
			_isDirty = true;
		}
		
		override protected function targetRemovedFromSceneHandler(target	: ISceneNode,
																  scene		: Scene) : void
		{
			super.targetRemovedFromSceneHandler(target, scene);
			
			if (numTargets == 0)
				unsubscribeFromJoints();
			
			var mesh : Mesh = target as Mesh;
			
			mesh.bindings.removeCallback('inFrustum', meshVisibilityChangedHandler);
			_skinningHelper.removeMesh(mesh);
		}
		
		private function jointLocalToWorldChangedHandler(emitter : Matrix4x4) : void
		{
			_isDirty = true;
		}
		
		public function rebindDependencies(nodeMap			: Dictionary, 
										   controllerMap	: Dictionary) : void
		{
			var numJoints	: uint = _joints.length;
			
			if (numTargets != 0)
				unsubscribeFromJoints();
			
			if (_joints.indexOf(_skeletonRoot) == -1 && nodeMap[_skeletonRoot])
				_skeletonRoot = nodeMap[_skeletonRoot];
			
			for (var jointId : uint = 0; jointId < numJoints; ++jointId)
				if (nodeMap[_joints[jointId]])
					_joints[jointId] = nodeMap[_joints[jointId]];
			
			if (numTargets != 0)
				subscribeToJoints();
			
			_isDirty = true;
		}
		
		private function subscribeToJoints() : void
		{
			var numJoints	: uint = _joints.length;
			
			for (var jointId : uint = 0; jointId < numJoints; ++jointId)
				_joints[jointId].localToWorld.changed.add(jointLocalToWorldChangedHandler);
			
			if (_joints.indexOf(_skeletonRoot) == -1)
				_skeletonRoot.localToWorld.changed.add(jointLocalToWorldChangedHandler);
		}
		
		private function unsubscribeFromJoints() : void
		{
			var numJoints	: uint = _joints.length;
			
			for (var jointId : uint = 0; jointId < numJoints; ++jointId)
				_joints[jointId].localToWorld.changed.remove(jointLocalToWorldChangedHandler);
			
			if (_joints.indexOf(_skeletonRoot) == -1)
				_skeletonRoot.localToWorld.changed.remove(jointLocalToWorldChangedHandler);
		}
		
		override protected function sceneEnterFrameHandler(scene		: Scene, 
														   viewport		: Viewport, 
														   destination	: BitmapData, 
														   time			: Number) : void
		{
			if (_isDirty && _skinningHelper.numMeshes != 0)
			{
				_skinningHelper.update(_skeletonRoot, _joints);
				_isDirty = false;
			}
		}
		
		override public function clone() : AbstractController
		{
			return new SkinningController(_method, _skeletonRoot, _joints, bindShape, invBindMatrices);
		}
		
		private function meshVisibilityChangedHandler(bindings 	: DataBindings,
												  property	: String,
												  oldValue	: Boolean,
												  newValue	: Boolean) : void
		{
			if (newValue)
				_skinningHelper.addMesh(bindings.owner as Mesh);
			else
				_skinningHelper.removeMesh(bindings.owner as Mesh);
		}
	}
}
