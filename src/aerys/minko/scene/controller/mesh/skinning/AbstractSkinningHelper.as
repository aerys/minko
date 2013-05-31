package aerys.minko.scene.controller.mesh.skinning
{
	import aerys.minko.ns.minko_animation;
	import aerys.minko.ns.minko_math;
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.animation.AnimationController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.type.animation.SkinningMethod;
	import aerys.minko.type.animation.timeline.ITimeline;
	import aerys.minko.type.animation.timeline.MatrixTimeline;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.geom.Matrix3D;
	import flash.utils.Dictionary;
	
	internal class AbstractSkinningHelper
	{
		use namespace minko_math;
		use namespace minko_scene;
		
		private static const WORLD_SKELETON_MATRIX	: Matrix3D		= new Matrix3D();
		private static const TMP_SKINNING_MATRIX	: Matrix3D 		= new Matrix3D();
		
		private static var TMP_MATRIX_3D	: Matrix3D = new Matrix3D();
		private static var TMP_MATRIX_4X4	: Matrix4x4 = new Matrix4x4();
		
		protected var _targets			: Vector.<Mesh>		= new Vector.<Mesh>();
		protected var _matrices			: Vector.<Number>	= new <Number>[];
		protected var _dqn				: Vector.<Number>	= new <Number>[];
		protected var _dqd				: Vector.<Number>	= new <Number>[];
		
		protected var _method			: uint;
		protected var _bindShape		: Matrix3D;
		protected var _invBindMatrices	: Vector.<Matrix3D>;
		
		// static skinning
		protected var _isStatic		: Boolean	= false;
		protected var _numFps		: uint		= 0;
		protected var _numJoints	: uint		= 0;
		protected var _numFrames	: uint		= 0;
		protected var _timeStep		: Number	= 0.0;
		
		// precomputed skinning information
		private var _nodeToAnimationController		: Dictionary					= null;
		private var _jointAnimationController		: Vector.<AnimationController>	= null;
		
		private var _jointSkinMatricesByFrame		: Vector.<Number>				= null;
		private var _jointDualQuaternionsDByFrame	: Vector.<Number>				= null;
		private var _jointDualQuaternionsNByFrame	: Vector.<Number>				= null;
		
		public function get numMeshes() : uint
		{
			return _targets.length;
		}
		
		public function AbstractSkinningHelper(method			: uint,
											   bindShape		: Matrix3D,
											   invBindMatrices	: Vector.<Matrix3D>,
											   flattenSkinning	: Boolean			= false,
											   numFps			: uint				= 0,
											   skeletonRoot		: Group				= null,
											   joints			: Vector.<Group>	= null)
		{
			_method				= method;
			_bindShape			= bindShape;
			_invBindMatrices	= invBindMatrices;
			
			if (flattenSkinning)
				initializeStaticSkinning(numFps, skeletonRoot, joints);
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
		
		public function update(skeletonRoot			: Group,
							   joints				: Vector.<Group>,
							   forceStaticUpdate	: Boolean,
							   numFps				: uint) : void
		{
			if (forceStaticUpdate)
				initializeStaticSkinning(numFps, skeletonRoot, joints);
			
			updateTargetSkinning(skeletonRoot, joints);			
		}
		
		protected function updateTargetSkinning(skeletonRoot	: Group,
												joints			: Vector.<Group>) : void
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
		
		public function clearStaticSkinning() : void
		{
			_isStatic	= false;
			_numFrames	= 0;
			_numJoints	= 0;
			_timeStep	= 0.0;
			
			// unlock the update of the transform matrices of all the animated nodes
			// encountered between a joint and the skeleton root
			for (var node:Object in _nodeToAnimationController)
			{
				var controller	: AnimationController	= _nodeToAnimationController[node];
				controller.unlockTimelineUpdate("transform");
			}
			
			_nodeToAnimationController		= null;
			_jointAnimationController		= null;
			
			_jointSkinMatricesByFrame		= null;
			_jointDualQuaternionsDByFrame	= null;
			_jointDualQuaternionsNByFrame	= null;
		}
		
		protected function initializeStaticSkinning(numFps			: uint,
													skeletonRoot	: Group,
													joints			: Vector.<Group>) : void
			
		{
			_isStatic	= true;
			_numFps		= numFps;
			
			initializeJointSkinMatrices(skeletonRoot, joints);
			
			if (_method == SkinningMethod.HARDWARE_DUAL_QUATERNION)
			{
				var dualQuaternions	: Array		= computeDualQuaternionsFromMatrices(_jointSkinMatricesByFrame);
				_jointDualQuaternionsDByFrame	= dualQuaternions[0];
				_jointDualQuaternionsNByFrame	= dualQuaternions[1];
			}			
		}
		
		private function initializeJointSkinMatrices(skeletonRoot	: Group,
													 joints			: Vector.<Group>) : void
		{
			// for each scene node between a joint and the skeleton root,
			// record the animation controller that controls its transform matrix.
			recordTransformAnimationControllers(skeletonRoot, joints);
			
			// lock the update of the transform matrices of all the animated nodes
			// encountered between a joint and the skeleton root
			for (var node:Object in _nodeToAnimationController)
			{
				_nodeToAnimationController[node].lockTimelineUpdate("transform");
			}
			
			// given the specified frame rate, resample all timelines
			// and interpolate matrices
			var nodeToTransformMatrices	: Dictionary	= resampleTransformTimelines();
			
			// compute the vector of skin matrices for each joint and frame
			_jointSkinMatricesByFrame = flattenJointSkinMatrices(
				skeletonRoot,
				joints, 
				nodeToTransformMatrices, 
				_bindShape,
				_invBindMatrices
			);
		}
		
		static private function getTransformAnimationController(node : ISceneNode) : AnimationController
		{
			// return the animation controller that updates the specified node's transform matrix (null otherwise), 
			var controllers 	: Vector.<AbstractController> 	= node.getControllersByType(AnimationController);	
			var numControllers	: uint 							= controllers.length;
			for (var ctrlId:uint = 0; ctrlId < numControllers; ++ctrlId)
			{
				var controller	: AnimationController	= AnimationController(controllers[ctrlId]); 
				var timeline	: MatrixTimeline		= getTransformMatrixTimeline(controller);
				if (timeline)
					return controller;
			}
			
			return null;
		}
		
		static private function getTransformMatrixTimeline(controller : AnimationController) : MatrixTimeline
		{
			var numTimelines : uint = controller.numTimelines;
			for (var timelineId:uint = 0; timelineId < numTimelines; ++timelineId)
			{
				var timeline : ITimeline = controller.getTimeline(timelineId);
				if (timeline.propertyPath == "transform")
					return MatrixTimeline(timeline);
			}
			return null;
		}
		
		private function recordTransformAnimationControllers(skeletonRoot	: Group,
															 joints			: Vector.<Group>) : void
		{
			_numJoints = joints.length;
			
			// associate for each encountered node and joint, the animation controller
			// that controls their transform matrix.
			// also return the common duration of all animations.
			
			// associate for each node between a joint and the skeleton root 
			// the animation controller that upadtes its transform matrix timeline.
			_nodeToAnimationController = new Dictionary();
			
			// record for each joint, the nearest transform animation controller
			// within the node hierarchy
			_jointAnimationController = new Vector.<AnimationController>(_numJoints, true);
			
			var totalTime	: int	= -1;
			for (var jointId:uint = 0; jointId < _numJoints; ++jointId)
			{
				_jointAnimationController[jointId] = null;
				
				var currentNode	: ISceneNode = joints[jointId];
				do
				{
					var controller	: AnimationController = getTransformAnimationController(currentNode);
					if (controller != null)
					{
						_nodeToAnimationController[currentNode] = controller;
						
						if (_jointAnimationController[jointId] == null)
							_jointAnimationController[jointId] = controller;
						
						if (totalTime >= 0 && totalTime != controller.totalTime)
							throw new Error("Failed to flatten skinning animation. All transform animation must have the same duration.");
						else
							totalTime = controller.totalTime;
					}
					currentNode = currentNode.parent;
				} 
				while (currentNode != skeletonRoot);
			}
			
			_numFrames	= uint(totalTime * 1e-3 * _numFps); // total time is in milliseconds
			_numFrames	= _numFrames < 2 ? 2 : _numFrames; 
			
			_timeStep	= totalTime / (_numFrames - 1);
		}
		
		private function resampleTransformTimelines() : Dictionary
		{	
			// resample all nodes' transform matrix animiations according to
			// the framerate specified in the constructor if any.
			// return a dictionary mapping an animated scene node to the
			// series of its transform matrices along the complete animation.
			
			var nodeToTransformMatrices	: Dictionary = new Dictionary;
			
			for (var key:Object in _nodeToAnimationController)
			{
				var node		: ISceneNode 			= ISceneNode(key);
				var controller	: AnimationController	= _nodeToAnimationController[node];				
				var timeline	: MatrixTimeline 		= getTransformMatrixTimeline(controller);
				var timeTable	: Vector.<uint>			= timeline.minko_animation::timeTable;
				var matrices	: Vector.<Matrix4x4>	= timeline.minko_animation::matrices;
				var numTimes	: uint					= timeTable.length;

				var interpMatrices	: Vector.<Number> 	= new Vector.<Number>(_numFrames << 4, true); // one 4x4 matrix per frame
				
				var timeIndex	: uint	 	= 0
				var time		: Number	= 0.0;	
				for (var frameId:uint = 0; frameId < _numFrames; ++frameId)
				{					
					if (time < timeTable[0])
						TMP_MATRIX_4X4 = matrices[0].clone();
						
					else if (time > timeTable[numTimes-1])
						TMP_MATRIX_4X4 = matrices[numTimes-1].clone();
						
					else
					{
						while(timeIndex < numTimes-1  && 
							time >= timeTable[timeIndex])
						{
							++timeIndex;
						}
						
						var t0	: Number	= timeTable[timeIndex > 0 ? timeIndex-1 : 0];
						var t1	: Number	= timeTable[timeIndex];
						var ratio	: Number =  t0 < t1
							? (time-t0)/(t1-t0)
							: 0.0;
						
						if (time < t0 || time > t1)
							throw new Error("Time parameter out-of-bounds.");
						ratio = ratio<0.0 ? 0.0 : ratio;
						ratio = ratio>1.0 ? 1.0 : ratio;
						
						TMP_MATRIX_4X4.interpolateBetween(
							matrices[timeIndex > 0 ? timeIndex-1 : 0],
							matrices[timeIndex],
							ratio,
							false,
							false
						);
					}
					
					TMP_MATRIX_3D.copyFrom(TMP_MATRIX_4X4.minko_math::_matrix);
					TMP_MATRIX_3D.copyRawDataTo(
						interpMatrices,
						frameId << 4,
						false
					);
					
					time += _timeStep;
				}
				nodeToTransformMatrices[node] = interpMatrices;
			}
			return nodeToTransformMatrices;
		}
		
		private function flattenJointSkinMatrices(skeletonRoot	: Group,
												  joints		: Vector.<Group>,
												  nodeToTransformMatrices	: Dictionary, 
												  bindShapeMatrix	: Matrix3D,
												  invBindMatrices	: Vector.<Matrix3D>) : Vector.<Number>
		{
			// given a node to transform matrix series mapping, "flatten" the 
			// skinning animation of each joint by appending all the transform matrices
			// encountered while going up the node hierarchy from the joint
			// to the skeleton root.
			// return a vector of matrices indexed by the joint id and the 
			// animation frame id.
			
			var TMP_MATRIX	: Matrix3D	= new Matrix3D();
			
			var numMatrices					: uint				= _numFrames * _numJoints;
			var jointSkinMatricesByFrame	: Vector.<Number> 	= new Vector.<Number>(numMatrices << 4, true);
			
			var jointLocalToWorld			: Matrix3D			= new Matrix3D();
			for (var jointId:uint = 0; jointId<_numJoints; ++jointId)
			{				
				for (var frameId:uint = 0; frameId<_numFrames; ++frameId)
				{
					var currentNode		: ISceneNode = joints[jointId];
					
					// goes up the hierarchy in order to accumulate the transform matrices
					// from the considered joint to the skeleton's root	
					jointLocalToWorld.identity();
					do {
						var frameTransforms	: Vector.<Number>	= nodeToTransformMatrices[currentNode];
						if (frameTransforms == null)
						{
							// the node is not animated, just use its transform matrix directly
							TMP_MATRIX.copyFrom(currentNode.transform.minko_math::_matrix);
						}
						else
						{
							// the node is animated, just pick the transform matrix corresponding
							// to the considered frame
							TMP_MATRIX.copyRawDataFrom(
								frameTransforms, 
								frameId << 4, 
								false
							);
						}
						jointLocalToWorld.append(TMP_MATRIX);
						
						currentNode = currentNode.parent;
					} while(currentNode != skeletonRoot);
					
					// precompute the final skinning matrix as:
					//    joint world transform * inverse (joint bind pose) * bind shape
					
					TMP_MATRIX.copyFrom(bindShapeMatrix);
					TMP_MATRIX.append(invBindMatrices[jointId]);
					TMP_MATRIX.append(jointLocalToWorld);
					
					TMP_MATRIX.copyRawDataTo(
						jointSkinMatricesByFrame,
						(jointId + _numJoints * frameId) << 4,
						true
					);
				}	
			}
			return jointSkinMatricesByFrame;
		}

		private function getJointCurrentFrame(jointId	: uint)	: uint
		{
			var jointController		: AnimationController = _jointAnimationController[jointId];
			var jointCurrentTime	: int 	= jointController.currentTime;
			var jointTotalTime		: int 	= jointController.totalTime;
			var frameId				: int	= (_numFrames-1) * (jointCurrentTime / jointTotalTime);
			
			// int((_numFrames-1.0) * (jointCurrentTime / jointTotalTime));			
			//			var frameId				: int 	= jointTotalTime > 0
			//				? int((_numFrames-1.0) * (jointCurrentTime / jointTotalTime))
			//				: 0;
			//			frameId = frameId<0 			? 0 			: frameId;
			//			frameId = frameId>=_numFrames 	? _numFrames-1 	: frameId;
			
			return frameId;
		}
		
		protected final function writeMatrices(skeletonRoot	: Group,
											   joints		: Vector.<Group>) : void
		{
			if (_isStatic)
				writePrecomputedMatrices();
			else
				computeMatrices(skeletonRoot, joints);
		}
		
		private function writePrecomputedMatrices()	: void
		{
			for (var jointId:uint = 0; jointId<_numJoints; ++jointId)
			{
				var frameId		: uint = getJointCurrentFrame(jointId);
				var matrixIndex	: uint = jointId + _numJoints * frameId; 
				
				TMP_MATRIX_3D.copyRawDataFrom(_jointSkinMatricesByFrame, matrixIndex << 4);
				TMP_MATRIX_3D.copyRawDataTo(_matrices, jointId << 4, false); // already transposed 
			}
		}
		
		private function computeMatrices(skeletonRoot	: Group,
										 joints			: Vector.<Group>) : void
		{
			var numJoints : int	= joints.length;
			
			WORLD_SKELETON_MATRIX.copyFrom(
				skeletonRoot.getLocalToWorldTransformUnsafe()._matrix
			);
			WORLD_SKELETON_MATRIX.invert();
			
			for (var jointIndex : int = 0; jointIndex < numJoints; ++jointIndex)
			{
				var joint			: Group 	= joints[jointIndex];
				var invBindMatrix 	: Matrix3D	= _invBindMatrices[jointIndex];
				
				TMP_SKINNING_MATRIX.copyFrom(_bindShape);
				TMP_SKINNING_MATRIX.append(invBindMatrix);
				TMP_SKINNING_MATRIX.append(joint.getLocalToWorldTransformUnsafe()._matrix);
				TMP_SKINNING_MATRIX.append(WORLD_SKELETON_MATRIX);
				TMP_SKINNING_MATRIX.copyRawDataTo(_matrices, jointIndex << 4, true);
			}
		}
		
		protected final function writeDualQuaternions(skeletonRoot	: Group,
													  joints		: Vector.<Group>) : void
		{
			if (_isStatic)
				writePrecomputedDualQuaternions();
			else
				computeDualQuaternions(skeletonRoot, joints);
		}
		
		private function writePrecomputedDualQuaternions()	: void
		{		
			for (var jointId:uint = 0; jointId<_numJoints; ++jointId)
			{
				var frameId			: uint = getJointCurrentFrame(jointId);
				var quaternionIndex	: uint = (jointId + _numJoints * frameId) << 2; 
				
				_dqd.splice(
					jointId << 2, 
					4, 
					_jointDualQuaternionsDByFrame[quaternionIndex], 
					_jointDualQuaternionsDByFrame[uint(quaternionIndex+1)],
					_jointDualQuaternionsDByFrame[uint(quaternionIndex+2)],
					_jointDualQuaternionsDByFrame[uint(quaternionIndex+3)]
				);
				
				_dqn.splice(
					jointId << 2, 
					4, 
					_jointDualQuaternionsNByFrame[quaternionIndex], 
					_jointDualQuaternionsNByFrame[uint(quaternionIndex+1)],
					_jointDualQuaternionsNByFrame[uint(quaternionIndex+2)],
					_jointDualQuaternionsNByFrame[uint(quaternionIndex+3)]
				);				
			}
		}
		
		private function computeDualQuaternions(skeletonRoot	: Group,
												joints			: Vector.<Group>) : void
		{
			computeMatrices(skeletonRoot, joints);
			
			var dualQuaternions	: Array = computeDualQuaternionsFromMatrices(_matrices);
			_dqd = dualQuaternions[0];
			_dqn = dualQuaternions[1];
		}
		
		private function computeDualQuaternionsFromMatrices(matrices	: Vector.<Number>) : Array
		{
			var numQuaternions	: uint = matrices.length >> 4;
			
			var dqD : Vector.<Number>	= new Vector.<Number>(numQuaternions << 2, true);
			var dqN : Vector.<Number>	= new Vector.<Number>(numQuaternions << 2, true);
			
			for (var quaternionId : uint = 0; quaternionId < numQuaternions; ++quaternionId)
			{
				var matrixOffset 		: int 		= quaternionId << 4;
				var quaternionOffset	: int 		= quaternionId << 2;
				
				var m00					: Number 	= matrices[matrixOffset];
				var m03 				: Number 	= matrices[uint(matrixOffset + 3)];
				var m05					: Number 	= matrices[uint(matrixOffset + 5)];
				var m07					: Number 	= matrices[uint(matrixOffset + 7)];
				var m10					: Number 	= matrices[uint(matrixOffset + 10)];
				var m11					: Number 	= matrices[uint(matrixOffset + 11)];
				
				var mTrace				: Number 	= m00 + m05 + matrices[uint(matrixOffset + 10)];
				var s					: Number	= 0.0;
				var nw					: Number	= 0.0;
				var nx					: Number	= 0.0;
				var ny					: Number	= 0.0;
				var nz					: Number	= 0.0;
				
				if (mTrace > 0)
				{
					s = 2.0 * Math.sqrt(mTrace + 1.0);
					nw = 0.25 * s;
					nx = (matrices[uint(matrixOffset + 9)] - matrices[uint(matrixOffset + 6)]) / s;
					ny = (matrices[uint(matrixOffset + 2)] - matrices[uint(matrixOffset + 8)]) / s;
					nz = (matrices[uint(matrixOffset + 4)] - matrices[uint(matrixOffset + 1)]) / s;
				}
				else if (m00 > m05 && m00 > m10)
				{
					s = 2.0 * Math.sqrt(1.0 + m00 - m05 - m10);
					
					nw = (matrices[uint(matrixOffset + 9)] - matrices[uint(matrixOffset + 6)]) / s
					nx = 0.25 * s;
					ny = (matrices[uint(matrixOffset + 1)] + matrices[uint(matrixOffset + 4)]) / s;
					nz = (matrices[uint(matrixOffset + 2)] + matrices[uint(matrixOffset + 8)]) / s;
				}
				else if (m05 > m10)
				{
					s = 2.0 * Math.sqrt(1.0 + m05 - m00 - m10);
					
					nw = (matrices[uint(matrixOffset + 2)] - matrices[uint(matrixOffset + 8)]) / s;
					nx = (matrices[uint(matrixOffset + 1)] + matrices[uint(matrixOffset + 4)]) / s;
					ny = 0.25 * s;
					nz = (matrices[uint(matrixOffset + 6)] + matrices[uint(matrixOffset + 9)]) / s;
				}
				else
				{
					s = 2.0 * Math.sqrt(1.0 + m10 - m00 - m05);
					
					nw = (matrices[uint(matrixOffset + 4)] - matrices[uint(matrixOffset + 1)]) / s;
					nx = (matrices[uint(matrixOffset + 2)] + matrices[uint(matrixOffset + 8)]) / s;
					ny = (matrices[uint(matrixOffset + 6)] + matrices[uint(matrixOffset + 9)]) / s;
					nz = 0.25 * s;
				}
				
				dqD[quaternionOffset]			=  0.5 * ( m03 * nw + m07 * nz - m11 * ny);
				dqD[uint(quaternionOffset + 1)]	=  0.5 * (-m03 * nz + m07 * nw + m11 * nx);
				dqD[uint(quaternionOffset + 2)]	=  0.5 * ( m03 * ny - m07 * nx + m11 * nw);
				dqD[uint(quaternionOffset + 3)]	= -0.5 * ( m03 * nx + m07 * ny + m11 * nz);
				
				dqN[quaternionOffset]			= nx;
				dqN[uint(quaternionOffset + 1)]	= ny;
				dqN[uint(quaternionOffset + 2)]	= nz;
				dqN[uint(quaternionOffset + 3)]	= nw;
			}
			return [dqD, dqN];
		}
	}
}