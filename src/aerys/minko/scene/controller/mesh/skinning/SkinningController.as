package aerys.minko.scene.controller.mesh.skinning
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.EnterFrameController;
	import aerys.minko.scene.controller.IRebindableController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.animation.SkinningMethod;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.display.BitmapData;
	import flash.geom.Matrix3D;
	import flash.utils.Dictionary;
	
	public class SkinningController extends EnterFrameController implements IRebindableController
	{
		private var _skinningHelper		: AbstractSkinningHelper;
		private var _isDirty			: Boolean;
		
		private var _method				: uint;
		private var _bindShape			: Matrix3D;
		private var _skeletonRoot		: Group;
		private var _joints				: Vector.<Group>;
		private var _invBindMatrices	: Vector.<Matrix3D>;
		
		public function get skinningMethod() : uint
		{
			return _method;
		}
		
		public function get bindShape() : Matrix4x4
		{
			var bindShape : Matrix4x4 = new Matrix4x4();
			bindShape.minko_math::_matrix = _bindShape;
			
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
			
			var numJoints : uint = joints.length;
			
			_method			 	= method;
			_skeletonRoot		= skeletonRoot;
			_joints				= joints.slice();
			_bindShape			= bindShape.minko_math::_matrix;
			_invBindMatrices	= new Vector.<Matrix3D>(numJoints, true);
			_isDirty			= false;
			
			for (var jointId : uint = 0; jointId < numJoints; ++jointId)
				_invBindMatrices[jointId] = invBindMatrices[jointId].minko_math::_matrix;
			
			init();
		}
		
		private function init() : void
		{
			if (skinningMethod == SkinningMethod.SOFTWARE_MATRIX)
				_skinningHelper = new SoftwareSkinningHelper(_method, _bindShape, _invBindMatrices);
			else
				_skinningHelper = new ShaderSkinningHelper(_method, _bindShape, _invBindMatrices);
		}
		
		override protected function targetAddedHandler(ctrl		: EnterFrameController,
													   target	: ISceneNode) : void
		{
			super.targetAddedHandler(ctrl, target);
			
			if (numTargets == 1)
				subscribeToJoints();
			
			_isDirty = true;
			_skinningHelper.addMesh(target as Mesh);
		}
		
		override protected function targetRemovedHandler(ctrl	: EnterFrameController,
														 target	: ISceneNode) : void
		{
			super.targetRemovedHandler(ctrl, target);
			
			if (numTargets == 0)
				unsubscribeFromJoints();
			
			_skinningHelper.removeMesh(target as Mesh);
		}
		
		private function jointLocalToWorldChangedHandler(emitter : Matrix4x4) : void
		{
			_isDirty = true;
		}
		
		public function rebindDependencies(nodeMap			: Dictionary, 
										   controllerMap	: Dictionary) : void
		{
			var numJoints	: uint = _joints.length;
			
			unsubscribeFromJoints();
			
			if (_joints.indexOf(_skeletonRoot) == -1 && nodeMap[_skeletonRoot])
				_skeletonRoot = nodeMap[_skeletonRoot];
			
			for (var jointId : uint = 0; jointId < numJoints; ++jointId)
				if (nodeMap[_joints[jointId]])
					_joints[jointId] = nodeMap[_joints[jointId]];
			
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
			if (_isDirty)
			{
				_skinningHelper.update(_skeletonRoot, _joints);
				_isDirty = false;
			}
		}
	}
}
