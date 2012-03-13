package aerys.minko.scene.controller.mesh
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.Signal;
	import aerys.minko.type.animation.SkinningMethod;
	import aerys.minko.type.data.DataProvider;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.geom.Matrix3D;
	import aerys.minko.scene.controller.AbstractController;

	/**
	 * The SkinningController works on meshes, compute all the data required
	 * to process vertex skinning and push it in the mesh data bindings.
	 * 
	 * @author Romain Gilliotte
	 * 
	 */
	public class SkinningController extends AbstractController
	{
		use namespace minko_math;
		
		private static const WORLD_SKELETON_MATRIX	: Matrix3D	= new Matrix3D();
		private static const TMP_SKINNING_MATRIX	: Matrix3D 	= new Matrix3D();
		
		private var _skinningData		: DataProvider			= new DataProvider();
		
		private var _isDirty			: Boolean				= true;
		
		private var _skinningMethod		: uint					= uint.MAX_VALUE;
		private var _bindShape			: Matrix4x4				= null;
		private var _skeletonRoot		: Group					= null;
		private var _joints				: Vector.<Group>		= null;
		private var _invBindMatrices	: Vector.<Matrix3D>		= null;
		
		// TEMP! Quick fix for mk file
		private var _invBindMatricesTmp : Vector.<Matrix4x4> 	= null;
		
		private var _matrices			: Vector.<Number>		= new Vector.<Number>();
		private var _dqn				: Vector.<Number>		= new Vector.<Number>();
		private var _dqd				: Vector.<Number>		= new Vector.<Number>();
		
		public function get joints():Vector.<Group>
		{
			return _joints;
		}

		public function get skeletonRoot():Group
		{
			return _skeletonRoot;
		}

		public function get invBindMatricesTmp():Vector.<Matrix4x4>
		{
			return _invBindMatricesTmp;
		}

		public function get bindShape():Matrix4x4
		{
			return _bindShape;
		}

		public function get skinningMethod():uint
		{
			return _skinningMethod;
		}

		public function get skinningData() : DataProvider
		{
			return _skinningData;
		}
		
		public function SkinningController(skinningMethod	: uint,
										   skeletonRoot		: Group,
										   joints 			: Vector.<Group>,
										   bindShape		: Matrix4x4,
										   invBindMatrices	: Vector.<Matrix4x4>)
		{
			super();
			
			var numJoints : uint = joints.length;
			
			_skinningMethod		= skinningMethod;
			_skeletonRoot		= skeletonRoot;
			_invBindMatricesTmp = invBindMatrices;
			_joints				= joints;
			_bindShape			= bindShape;
			_invBindMatrices	= new Vector.<Matrix3D>(numJoints, true);
			
			for (var jointId : uint = 0; jointId < numJoints; ++jointId)
				_invBindMatrices[jointId] = invBindMatrices[jointId]._matrix;
			
			// init data provider.
			_skinningData.skinningMethod		= _skinningMethod;
			_skinningData.skinningNumBones		= _joints.length;
			_skinningData.skinningBindShape		= _bindShape;
			
			// subscribe to skinned meshes (in order to register our data provider).
			targetAdded.add(targetAddedHandler);
			
			// subscribe to root and bone transform changes.
			skeletonRoot.localToWorld.changed.add(jointLocalToWorldChangedHandler);
			for each (var joint : Group in _joints)
				joint.localToWorld.changed.add(jointLocalToWorldChangedHandler)
		}
		
		private function targetAddedHandler(controller	: SkinningController, 
											mesh		: Mesh) : void
		{
			var format			: VertexFormat	= mesh.geometry.getVertexStream().format;
			var maxInfluences	: uint			= 
				uint(format.hasComponent(VertexComponent.BONE0))
				+ uint(format.hasComponent(VertexComponent.BONE1))
				+ uint(format.hasComponent(VertexComponent.BONE2))
				+ uint(format.hasComponent(VertexComponent.BONE3))
				+ uint(format.hasComponent(VertexComponent.BONE4))
				+ uint(format.hasComponent(VertexComponent.BONE5))
				+ uint(format.hasComponent(VertexComponent.BONE6))
				+ uint(format.hasComponent(VertexComponent.BONE7));
			
			mesh.bindings.add(_skinningData);
			mesh.bindings.setProperty('skinningMaxInfluences', maxInfluences);
		}
		
		private function targetRemovedHandler(controlller	: SkinningController,
											  mesh			: Mesh) : void
		{
			mesh.bindings.remove(_skinningData);
			mesh.bindings.removeProperty('skinningMaxInfluences');
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
					_skinningData.skinningMatrices = _matrices;
					break;
				
				case SkinningMethod.DUAL_QUATERNION:
					writeMatrices();
					writeDualQuaternions();
					_skinningData.skinningDQn = _dqn;
					_skinningData.skinningDQd = _dqd;
					break;
				
				case SkinningMethod.DUAL_QUATERNION_SCALE:
					throw new Error('This skinning method is yet to be implemented.');
					
				default:
					throw new Error('Invalid skinning method.');
			}
			
			_isDirty = false;
			
			return false;
		}
		
		private function writeMatrices() : void
		{
			var numJoints : int	= _joints.length;
			
			WORLD_SKELETON_MATRIX.copyFrom(_skeletonRoot.localToWorld._matrix);
			WORLD_SKELETON_MATRIX.invert();
			
			for (var jointIndex : int = 0; jointIndex < numJoints; ++jointIndex)
			{
				var joint			: Group 	= _joints[jointIndex];
				var invBindMatrix 	: Matrix3D	= _invBindMatrices[jointIndex];
				
				TMP_SKINNING_MATRIX.copyFrom(joint.localToWorld._matrix);
				TMP_SKINNING_MATRIX.append(WORLD_SKELETON_MATRIX);
				TMP_SKINNING_MATRIX.prepend(invBindMatrix);
				TMP_SKINNING_MATRIX.copyRawDataTo(_matrices, jointIndex * 16, true);
			}
		}
		
		private function writeDualQuaternions() : void
		{
			var numQuaternions : int = _matrices.length / 16;
			
			_dqd.length = _dqn.length = numQuaternions * 4;
			
			for (var quaternionId : int = 0; quaternionId < numQuaternions; ++quaternionId)
			{
				var matrixOffset 		: int = quaternionId * 16;
				var quaternionOffset	: int = quaternionId * 4;
				
				var m00		: Number = _matrices[matrixOffset];
				var m03 	: Number = _matrices[int(matrixOffset + 3)];
				var m05		: Number = _matrices[int(matrixOffset + 5)];
				var m07		: Number = _matrices[int(matrixOffset + 7)];
				var m10		: Number = _matrices[int(matrixOffset + 10)];
				var m11		: Number = _matrices[int(matrixOffset + 11)];
				
				var mTrace	: Number = m00 + m05 + _matrices[int(matrixOffset + 10)];
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
				else if (m00 > m05 && m00 > m10)
				{
					s = 2.0 * Math.sqrt(1.0 + m00 - m05 - m10);
					
					nw = (_matrices[int(matrixOffset + 9)] - _matrices[int(matrixOffset + 6)]) / s
					nx = 0.25 * s;
					ny = (_matrices[int(matrixOffset + 1)] + _matrices[int(matrixOffset + 4)]) / s;
					nz = (_matrices[int(matrixOffset + 2)] + _matrices[int(matrixOffset + 8)]) / s;
				}
				else if (m05 > m10)
				{
					s = 2.0 * Math.sqrt(1.0 + m05 - m00 - m10);
					
					nw = (_matrices[int(matrixOffset + 2)] - _matrices[int(matrixOffset + 8)]) / s;
					nx = (_matrices[int(matrixOffset + 1)] + _matrices[int(matrixOffset + 4)]) / s;
					ny = 0.25 * s;
					nz = (_matrices[int(matrixOffset + 6)] + _matrices[int(matrixOffset + 9)]) / s;
				}
				else
				{
					s = 2.0 * Math.sqrt(1.0 + m10 - m00 - m05);
					
					nw = (_matrices[int(matrixOffset + 4)] - _matrices[int(matrixOffset + 1)]) / s;
					nx = (_matrices[int(matrixOffset + 2)] + _matrices[int(matrixOffset + 8)]) / s;
					ny = (_matrices[int(matrixOffset + 6)] + _matrices[int(matrixOffset + 9)]) / s;
					nz = 0.25 * s;
				}
				
				
				_dqd[quaternionOffset]			=  0.5 * ( m03 * nw + m07 * nz - m11 * ny);
				_dqd[int(quaternionOffset + 1)]	=  0.5 * (-m03 * nz + m07 * nw + m11 * nx);
				_dqd[int(quaternionOffset + 2)]	=  0.5 * ( m03 * ny - m07 * nx + m11 * nw);
				_dqd[int(quaternionOffset + 3)]	= -0.5 * ( m03 * nx + m07 * ny + m11 * nz);
				
				_dqn[quaternionOffset]			= nx;
				_dqn[int(quaternionOffset + 1)]	= ny;
				_dqn[int(quaternionOffset + 2)]	= nz;
				_dqn[int(quaternionOffset + 3)]	= nw;
			}
		}
	}
}
