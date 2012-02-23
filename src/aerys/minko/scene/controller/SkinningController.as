package aerys.minko.scene.controller
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.Signal;
	import aerys.minko.type.animation.SkinningMethod;
	import aerys.minko.type.data.DataProvider;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.geom.Matrix3D;

	public class SkinningController extends AbstractController
	{
		use namespace minko_math;
		
		private static const WORLD_SKELETON_MATRIX	: Matrix3D	= new Matrix3D();
		private static const TMP_SKINNING_MATRIX	: Matrix3D 	= new Matrix3D();
		
		private var _skinningData		: DataProvider			= new DataProvider();
		
		private var _isDirty			: Boolean				= true;
		
		private var _skinningMethod		: uint					= uint.MAX_VALUE;
		private var _bindShape			: Matrix4x4				= new Matrix4x4();
		private var _skeletonRoot		: Group					= new Group();
		private var _joints				: Vector.<Group>		= null;
		private var _invBindMatrices	: Vector.<Matrix4x4>	= null;
		
		private var _matrices			: Vector.<Number>		= new Vector.<Number>();
		private var _dqn				: Vector.<Number>		= new Vector.<Number>();
		private var _dqd				: Vector.<Number>		= new Vector.<Number>();
		
		public function get skinningData() : DataProvider
		{
			return _skinningData;
		}
		
		public function SkinningController(skinningMethod	: uint,
										   skeletonRoot		: Group,
										   joints 			: Vector.<Group>,
										   invBindMatrices	: Vector.<Matrix4x4>)
		{
			super(Mesh);
			
			_skinningMethod		= skinningMethod;
			_skeletonRoot		= skeletonRoot;
			_joints				= joints;
			_invBindMatrices	= invBindMatrices;
			
			initialize();
		}
		
		private function initialize() : void
		{
			_skinningData.numBones = _joints.length;
			
			targetAdded.add(targetAddedHandler);
			
			for each (var joint : Group in _joints)
				joint.localToWorld.changed.add(jointLocalToWorldChangedHandler)
		}
		
		private function targetAddedHandler(controller	: SkinningController, 
											mesh		: Mesh) : void
		{
			mesh.effectBindings.add(_skinningData);
		}
		
		private function targetRemovedHandler(controlller	: SkinningController,
											  mesh			: Mesh) : void
		{
			mesh.effectBindings.remove(_skinningData);
		}
		
		private function jointLocalToWorldChangedHandler(emitter		: Matrix4x4, 
														 propertyName	: String) : void
		{
			_isDirty = true;
		}
		
		override protected function updateOnTime(time : Number) : Boolean
		{
			if (!_isDirty)
				return false;
			
			switch (_skinningMethod)
			{
				case SkinningMethod.MATRIX:
					writeMatrices();
					_skinningData.skinningMatrices = _skinningData;
					break;
				
				case SkinningMethod.DUAL_QUATERNION:
					writeMatrices();
					writeDualQuaternions();
					_skinningData.dqn = _dqn;
					_skinningData.dqd = _dqd;
					break;
				
				case SkinningMethod.DUAL_QUATERNION_SCALE:
					throw new Error('This skinning method yet to be implemented.');
					
				default:
					throw new Error('Invalid skinning method.');
			}
			
			_isDirty = false;
			
			return false;
		}
		
		private function writeMatrices() : void
		{
			var numJoints	: int	= _joints.length;
			
			WORLD_SKELETON_MATRIX.copyFrom(_skeletonRoot.localToWorld._matrix);
			
			for (var jointIndex : int = 0; jointIndex < numJoints; ++jointIndex)
			{
				var joint			: Group 	= _joints[jointIndex];
				var invBindMatrix 	: Matrix3D 	= Matrix4x4(_invBindMatrices[jointIndex])._matrix;
				
				TMP_SKINNING_MATRIX.copyFrom(joint.localToWorld._matrix);
				TMP_SKINNING_MATRIX.prepend(WORLD_SKELETON_MATRIX);
				TMP_SKINNING_MATRIX.prepend(invBindMatrix);
				TMP_SKINNING_MATRIX.copyRawDataTo(_matrices, jointIndex * 16, true);
			}
		}
		
		private function writeDualQuaternions() : void
		{
			var numQuaternions : int = _matrices.length >> 2;
			
			_dqd.length = numQuaternions;
			_dqn.length = numQuaternions;
			
			for (var quaternionId : int = 0; quaternionId < numQuaternions; ++quaternionId)
			{
				var matrixOffset 		: int = quaternionId << 4;
				var quaternionOffset 	: int = quaternionId << 2;
				
				var mTrace	: Number = _matrices[int(matrixOffset + 0)] + _matrices[int(matrixOffset + 5)] + _matrices[int(matrixOffset + 10)];
				var s		: Number;
				var nw		: Number;
				var nx		: Number;
				var ny		: Number;
				var nz		: Number;
				
				if (mTrace > 0)
				{
					s = 2.0 * Math.sqrt(mTrace + 1.0);
					nw = 0.25 * s;
					nx = (_matrices[int(matrixOffset + 9)] - _matrices[int(matrixOffset + 6)]) / s;
					ny = (_matrices[int(matrixOffset + 2)] - _matrices[int(matrixOffset + 8)]) / s;
					nz = (_matrices[int(matrixOffset + 4)] - _matrices[int(matrixOffset + 1)]) / s;
				}
				else if (_matrices[int(matrixOffset + 0)] > _matrices[int(matrixOffset + 5)] && _matrices[int(matrixOffset + 0)] > _matrices[int(matrixOffset + 10)])
				{
					s = 2.0 * Math.sqrt(1.0 + _matrices[int(matrixOffset + 0)] - _matrices[int(matrixOffset + 5)] - _matrices[int(matrixOffset + 10)]);
					
					nw = (_matrices[int(matrixOffset + 9)] - _matrices[int(matrixOffset + 6)]) / s
					nx = 0.25 * s;
					ny = (_matrices[int(matrixOffset + 1)] + _matrices[int(matrixOffset + 4)]) / s;
					nz = (_matrices[int(matrixOffset + 2)] + _matrices[int(matrixOffset + 8)]) / s;
				}
				else if (_matrices[int(matrixOffset + 5)] > _matrices[int(matrixOffset + 10)])
				{
					s = 2.0 * Math.sqrt(1.0 + _matrices[int(matrixOffset + 5)] - _matrices[int(matrixOffset + 0)] - _matrices[int(matrixOffset + 10)]);
					
					nw = (_matrices[int(matrixOffset + 2)] - _matrices[int(matrixOffset + 8)]) / s;
					nx = (_matrices[int(matrixOffset + 1)] + _matrices[int(matrixOffset + 4)]) / s;
					ny = 0.25 * s;
					nz = (_matrices[int(matrixOffset + 6)] + _matrices[int(matrixOffset + 9)]) / s;
				}
				else
				{
					s = 2.0 * Math.sqrt(1.0 + _matrices[int(matrixOffset + 10)] - _matrices[int(matrixOffset + 0)] - _matrices[int(matrixOffset + 5)]);
					
					nw = (_matrices[int(matrixOffset + 4)] - _matrices[int(matrixOffset + 1)]) / s;
					nx = (_matrices[int(matrixOffset + 2)] + _matrices[int(matrixOffset + 8)]) / s;
					ny = (_matrices[int(matrixOffset + 6)] + _matrices[int(matrixOffset + 9)]) / s;
					nz = 0.25 * s;
				}
				
				_dqd[quaternionOffset] = 	0.5 * (_matrices[int(matrixOffset + 3)] * nw + _matrices[int(matrixOffset + 7)] * nz - _matrices[int(matrixOffset + 11)] * ny);
				_dqd[int(quaternionOffset + 1)] = 	0.5 * (- _matrices[int(matrixOffset + 3)] * nz + _matrices[int(matrixOffset + 7)] * nw + _matrices[int(matrixOffset + 11)] * nx);
				_dqd[int(quaternionOffset + 2)] = 	0.5 * (	 _matrices[int(matrixOffset + 3)] * ny - _matrices[int(matrixOffset + 7)] * nx + _matrices[int(matrixOffset + 11)] * nw);
				_dqd[int(quaternionOffset + 3)] = -	0.5 * (	 _matrices[int(matrixOffset + 3)] * nx + _matrices[int(matrixOffset + 7)] * ny + _matrices[int(matrixOffset + 11)] * nz);
				
				_dqn[quaternionOffset] = nx;
				_dqn[int(quaternionOffset + 1)] = ny;
				_dqn[int(quaternionOffset + 2)] = nz;
				_dqn[int(quaternionOffset + 3)] = nw;
			}
		}
	}
}
