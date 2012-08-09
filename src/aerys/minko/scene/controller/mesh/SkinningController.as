package aerys.minko.scene.controller.mesh
{
	import aerys.minko.ns.minko_animation;
	import aerys.minko.ns.minko_math;
	import aerys.minko.render.Viewport;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	import aerys.minko.scene.SceneIterator;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.EnterFrameController;
	import aerys.minko.scene.controller.IRebindableController;
	import aerys.minko.scene.data.SkinningDataProvider;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.animation.SkinningMethod;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.display.BitmapData;
	import flash.geom.Matrix3D;
	import flash.utils.Dictionary;

	/**
	 * The SkinningController works on meshes, compute all the data required
	 * to process vertex skinning and push it in the mesh data bindings.
	 * 
	 * @author Romain Gilliotte
	 * 
	 */
	public final class SkinningController extends EnterFrameController implements IRebindableController
	{
		use namespace minko_math;
		
		private static const WORLD_SKELETON_MATRIX	: Matrix3D	= new Matrix3D();
		private static const TMP_SKINNING_MATRIX	: Matrix3D 	= new Matrix3D();
		
		private var _skinningData		: SkinningDataProvider	= new SkinningDataProvider();
		
		private var _isDirty			: Boolean				= true;
		
		private var _skinningMethod		: uint					= uint.MAX_VALUE;
		private var _bindShape			: Matrix4x4				= null;
		private var _skeletonRoot		: Group					= null;
		private var _joints				: Vector.<Group>		= null;
		private var _invBindMatrices	: Vector.<Matrix3D>		= null;
		
		private var _matrices			: Vector.<Number>		= new <Number>[];
		private var _dqn				: Vector.<Number>		= new <Number>[];
		private var _dqd				: Vector.<Number>		= new <Number>[];
		
		/**
		 * @private
		 */ 
		public function get bindShape() : Matrix4x4
		{
			return _bindShape;
		}
		
		/**
		 * @private
		 */
		public function get joints() : Vector.<Group>
		{
			return _joints;
		}
		
		public function get skeletonRoot() : Group
		{
			return _skeletonRoot;
		}

		public function get skinningMethod() : uint
		{
			return _skinningMethod;
		}

		public function SkinningController(skinningMethod	: uint,
										   skeletonRoot		: Group,
										   joints			: Vector.<Group>,
										   bindShape		: Matrix4x4,
										   invBindMatrices	: Vector.<Matrix4x4>)
		{
			super();
		
			initialize(skinningMethod, skeletonRoot, joints, bindShape, invBindMatrices);
		}
		
		private function initialize(skinningMethod	: uint,
									skeletonRoot	: Group,
									joints			: Vector.<Group>,
									bindShape		: Matrix4x4,
									invBindMatrices	: Vector.<Matrix4x4>) : void
		{
			var numJoints : uint = joints.length;
			
			_skinningMethod		= skinningMethod;
			_skeletonRoot		= skeletonRoot;
			_bindShape			= bindShape;
			_invBindMatrices	= new Vector.<Matrix3D>(numJoints, true);
			_joints				= joints.slice();
			
			for (var jointId : uint = 0; jointId < numJoints; ++jointId)
			{
				_invBindMatrices[jointId] = invBindMatrices[jointId]._matrix;
				_joints[jointId].localToWorld.changed.add(jointLocalToWorldChangedHandler);
			}
			
			if (_joints.indexOf(skeletonRoot) == -1)
				_skeletonRoot.localToWorld.changed.add(jointLocalToWorldChangedHandler);
			
			_isDirty = true;
			
			// init data provider.
			_skinningData.method	= _skinningMethod;
			_skinningData.numBones	= numJoints;
			_skinningData.bindShape	= _bindShape;
		}
		
		override public function clone() : AbstractController
		{
			return new SkinningController(
				_skinningMethod,
				_skeletonRoot,
				_joints,
				_bindShape,
				getInvBindMatrices()
			);
		}
		
		public function rebindDependencies(nodeMap			: Dictionary, 
										   controllerMap	: Dictionary) : void
		{
			var numJoints : uint = _joints.length;
			
			if (_joints.indexOf(_skeletonRoot) == -1)
			{
				_skeletonRoot.localToWorld.changed.remove(jointLocalToWorldChangedHandler);
				_skeletonRoot = nodeMap[_skeletonRoot];
				_skeletonRoot.localToWorld.changed.add(jointLocalToWorldChangedHandler);
			}
			
			for (var jointId : uint = 0; jointId < numJoints; ++jointId)
			{
				_joints[jointId].localToWorld.changed.remove(jointLocalToWorldChangedHandler);
				_joints[jointId] = nodeMap[_joints[jointId]];
				_joints[jointId].localToWorld.changed.add(jointLocalToWorldChangedHandler);
			}
			
			_isDirty = true;
		}
		
		public function getInvBindMatrices() : Vector.<Matrix4x4>
		{
			var numMatrices		: uint					= _invBindMatrices.length;
			var bindMatrices	: Vector.<Matrix4x4>	= new Vector.<Matrix4x4>(
				numMatrices, true
			);
			
			for (var i : uint = 0; i < numMatrices; ++i)
			{
				var matrix : Matrix4x4 = new Matrix4x4();
				
				matrix._matrix = _invBindMatrices[i].clone();
				bindMatrices[i] = matrix;
			}
			
			return bindMatrices;
		}
		
		override protected function targetAddedHandler(controller	: EnterFrameController, 
													   target		: ISceneNode) : void
		{
			super.targetAddedHandler(controller, target);
			
			var mesh	: Mesh			= target as Mesh;
			var format	: VertexFormat	= mesh.geometry.getVertexStream().format;
			
			_skinningData.maxInfluences = 
				format.hasComponent(VertexComponent.BONE_S) 
				+ 2 * (
					uint(format.hasComponent(VertexComponent.BONE_0_1))
					+ uint(format.hasComponent(VertexComponent.BONE_2_3))
					+ uint(format.hasComponent(VertexComponent.BONE_4_5))
					+ uint(format.hasComponent(VertexComponent.BONE_6_7))
				);
			
			mesh.bindings.addProvider(_skinningData);
		}
		
		override protected function targetRemovedHandler(controller	: EnterFrameController,
														 target		: ISceneNode) : void
		{
			super.targetRemovedHandler(controller, target);
			
			(target as Mesh).bindings.removeProvider(_skinningData);
		}
		
		private function jointLocalToWorldChangedHandler(emitter : Matrix4x4) : void
		{
			_isDirty = true;
		}
		
		override protected function sceneEnterFrameHandler(scene	: Scene,
														   viewport	: Viewport,
														   target	: BitmapData,
														   time		: Number) : void
		{
			if (!_isDirty)
				return ;
			
			switch (_skinningMethod)
			{
				case SkinningMethod.MATRIX:
					writeMatrices();
					_skinningData.matrices = _matrices;
					break;
				
				case SkinningMethod.DUAL_QUATERNION:
					writeMatrices();
					writeDualQuaternions();
					_skinningData.dqN = _dqn;
					_skinningData.dqD = _dqd;
					break;
				
				case SkinningMethod.DUAL_QUATERNION_SCALE:
					throw new Error('This skinning method is yet to be implemented.');
					
				default:
					throw new Error('Invalid skinning method.');
			}
			
			_isDirty = false;
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
				var matrixOffset 		: int 		= quaternionId * 16;
				var quaternionOffset	: int 		= quaternionId * 4;
				
				var m00					: Number 	= _matrices[matrixOffset];
				var m03 				: Number 	= _matrices[int(matrixOffset + 3)];
				var m05					: Number 	= _matrices[int(matrixOffset + 5)];
				var m07					: Number 	= _matrices[int(matrixOffset + 7)];
				var m10					: Number 	= _matrices[int(matrixOffset + 10)];
				var m11					: Number 	= _matrices[int(matrixOffset + 11)];
				
				var mTrace				: Number 	= m00 + m05 + _matrices[int(matrixOffset + 10)];
				var s					: Number	= 0.;
				var nw					: Number	= 0.;
				var nx					: Number	= 0.;
				var ny					: Number	= 0.;
				var nz					: Number	= 0.;
				
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
