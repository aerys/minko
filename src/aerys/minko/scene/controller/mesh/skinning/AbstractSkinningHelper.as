package aerys.minko.scene.controller.mesh.skinning
{
	import aerys.minko.Minko;
	import aerys.minko.ns.minko_math;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.type.animation.SkinningMethod;
	import aerys.minko.type.log.DebugLevel;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;

	internal class AbstractSkinningHelper
	{
		use namespace minko_math;
		
		private static const WORLD_SKELETON_MATRIX	: Matrix3D		= new Matrix3D();
		private static const TMP_SKINNING_MATRIX	: Matrix3D 		= new Matrix3D();
		
		protected var _targets			: Vector.<Mesh>		= new Vector.<Mesh>();
		protected var _matrices			: Vector.<Number>	= new <Number>[];
		protected var _dqn				: Vector.<Number>	= new <Number>[];
		protected var _dqd				: Vector.<Number>	= new <Number>[];
		
		protected var _method			: uint;
		protected var _bindShape		: Matrix3D;
		protected var _invBindMatrices	: Vector.<Matrix3D>;
		
		public function get numMeshes() : uint
		{
			return _targets.length;
		}
		
		public function AbstractSkinningHelper(method			: uint,
											   bindShape		: Matrix3D,
											   invBindMatrices	: Vector.<Matrix3D>)
		{
			_method				= method;
			_bindShape			= bindShape;
			_invBindMatrices	= invBindMatrices;
		}
		
		public function addMesh(mesh : Mesh) : void
		{
			_targets.push(mesh);
		}
		
		public function removeMesh(mesh : Mesh) : void
		{
			_targets.splice(_targets.indexOf(mesh), 1);
		}
		
		public function removeAllMeshes() : void
		{
			while (_targets.length != 0)
				removeMesh(_targets[0]);
		}
		
		public function update(skeletonRoot	: Group,
							   joints		: Vector.<Group>) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected final function getInfluenceStrides(format : VertexFormat, out : Vector.<uint>) : void
		{
			var numInfluences			: uint = getNumInfluences(format);
			var roundedNumInfluences	: uint = (numInfluences >> 1) << 1;
			
			if (out.length != numInfluences)
				out.length = numInfluences;
			
			for (var influenceId : uint = 0; influenceId < roundedNumInfluences; ++influenceId)
				out[influenceId] = 
					format.getBytesOffsetForComponent(VertexComponent.BONES[influenceId >> 1]) 
					+ ((influenceId & 0x1) << 3);
			
			if (numInfluences % 2 != 0)
				out[numInfluences - 1] = 
					format.getBytesOffsetForComponent(VertexComponent.BONE_S);
		}
		
		public static function getNumInfluences(format : VertexFormat) : uint
		{
			return uint(format.hasComponent(VertexComponent.BONE_S))
				+ 2 * (
					uint(format.hasComponent(VertexComponent.BONE_0_1))
					+ uint(format.hasComponent(VertexComponent.BONE_2_3))
					+ uint(format.hasComponent(VertexComponent.BONE_4_5))
					+ uint(format.hasComponent(VertexComponent.BONE_6_7))
					+ uint(format.hasComponent(VertexComponent.BONE_8_9))
					+ uint(format.hasComponent(VertexComponent.BONE_10_11))
				);
		}
		
		protected final function writeMatrices(skeletonRoot	: Group,
											   joints		: Vector.<Group>) : void
		{
			var numJoints : int	= joints.length;
			
			WORLD_SKELETON_MATRIX.copyFrom(skeletonRoot.localToWorld._matrix);
			WORLD_SKELETON_MATRIX.invert();
			
			for (var jointIndex : int = 0; jointIndex < numJoints; ++jointIndex)
			{
				var joint			: Group 	= joints[jointIndex];
				var invBindMatrix 	: Matrix3D	= _invBindMatrices[jointIndex];
				
				TMP_SKINNING_MATRIX.copyFrom(_bindShape);
				TMP_SKINNING_MATRIX.append(invBindMatrix);
				TMP_SKINNING_MATRIX.append(joint.localToWorld._matrix);
				TMP_SKINNING_MATRIX.append(WORLD_SKELETON_MATRIX);
				TMP_SKINNING_MATRIX.copyRawDataTo(_matrices, jointIndex * 16, true);
			}
		}
		
		protected final function writeDualQuaternions() : void
		{
			var numQuaternions : int = _matrices.length / 16;
			
			_dqd.length = _dqn.length = numQuaternions * 4;
			
			for (var quaternionId : int = 0; quaternionId < numQuaternions; ++quaternionId)
			{
				var matrixOffset 		: int 		= quaternionId * 16;
				var quaternionOffset	: int 		= quaternionId * 4;
				
				var m00					: Number 	= _matrices[matrixOffset];
				var m03 				: Number 	= _matrices[uint(matrixOffset + 3)];
				var m05					: Number 	= _matrices[uint(matrixOffset + 5)];
				var m07					: Number 	= _matrices[uint(matrixOffset + 7)];
				var m10					: Number 	= _matrices[uint(matrixOffset + 10)];
				var m11					: Number 	= _matrices[uint(matrixOffset + 11)];
				
				var mTrace				: Number 	= m00 + m05 + _matrices[uint(matrixOffset + 10)];
				var s					: Number	= 0.;
				var nw					: Number	= 0.;
				var nx					: Number	= 0.;
				var ny					: Number	= 0.;
				var nz					: Number	= 0.;
				
				if (mTrace > 0)
				{
					s = 2.0 * Math.sqrt(mTrace + 1.0);
					nw = 0.25 * s;
					nx = (_matrices[uint(matrixOffset + 9)] - _matrices[uint(matrixOffset + 6)]) / s;
					ny = (_matrices[uint(matrixOffset + 2)] - _matrices[uint(matrixOffset + 8)]) / s;
					nz = (_matrices[uint(matrixOffset + 4)] - _matrices[uint(matrixOffset + 1)]) / s;
				}
				else if (m00 > m05 && m00 > m10)
				{
					s = 2.0 * Math.sqrt(1.0 + m00 - m05 - m10);
					
					nw = (_matrices[uint(matrixOffset + 9)] - _matrices[uint(matrixOffset + 6)]) / s
					nx = 0.25 * s;
					ny = (_matrices[uint(matrixOffset + 1)] + _matrices[uint(matrixOffset + 4)]) / s;
					nz = (_matrices[uint(matrixOffset + 2)] + _matrices[uint(matrixOffset + 8)]) / s;
				}
				else if (m05 > m10)
				{
					s = 2.0 * Math.sqrt(1.0 + m05 - m00 - m10);
					
					nw = (_matrices[uint(matrixOffset + 2)] - _matrices[uint(matrixOffset + 8)]) / s;
					nx = (_matrices[uint(matrixOffset + 1)] + _matrices[uint(matrixOffset + 4)]) / s;
					ny = 0.25 * s;
					nz = (_matrices[uint(matrixOffset + 6)] + _matrices[uint(matrixOffset + 9)]) / s;
				}
				else
				{
					s = 2.0 * Math.sqrt(1.0 + m10 - m00 - m05);
					
					nw = (_matrices[uint(matrixOffset + 4)] - _matrices[uint(matrixOffset + 1)]) / s;
					nx = (_matrices[uint(matrixOffset + 2)] + _matrices[uint(matrixOffset + 8)]) / s;
					ny = (_matrices[uint(matrixOffset + 6)] + _matrices[uint(matrixOffset + 9)]) / s;
					nz = 0.25 * s;
				}
				
				_dqd[quaternionOffset]				=  0.5 * ( m03 * nw + m07 * nz - m11 * ny);
				_dqd[uint(quaternionOffset + 1)]	=  0.5 * (-m03 * nz + m07 * nw + m11 * nx);
				_dqd[uint(quaternionOffset + 2)]	=  0.5 * ( m03 * ny - m07 * nx + m11 * nw);
				_dqd[uint(quaternionOffset + 3)]	= -0.5 * ( m03 * nx + m07 * ny + m11 * nz);
				
				_dqn[quaternionOffset]				= nx;
				_dqn[uint(quaternionOffset + 1)]	= ny;
				_dqn[uint(quaternionOffset + 2)]	= nz;
				_dqn[uint(quaternionOffset + 3)]	= nw;
			}
		}
	}
}