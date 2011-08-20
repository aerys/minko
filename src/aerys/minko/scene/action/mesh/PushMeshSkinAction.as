package aerys.minko.scene.action.mesh
{
	import aerys.minko.render.effect.Style;
	import aerys.minko.render.effect.skinning.SkinningStyle;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.ITransformableScene;
	import aerys.minko.scene.node.group.Group;
	import aerys.minko.scene.node.group.IGroup;
	import aerys.minko.scene.node.group.Joint;
	import aerys.minko.scene.node.group.TransformGroup;
	import aerys.minko.scene.node.mesh.SkinnedMesh;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public class PushMeshSkinAction implements IAction
	{
		private static const TMP_LOCAL_MATRIX		: Matrix4x4	= new Matrix4x4();
		private static const TMP_SKINNING_MATRIX	: Matrix4x4 = new Matrix4x4();
		private static const EMPTY_STYLE			: Style		= new Style();
		
		private static var _instance : PushMeshSkinAction	= null;
		
		public static function get pushMeshSkinAction() : PushMeshSkinAction
		{
			return _instance || (_instance = new PushMeshSkinAction());
		}
		
		public function get type() : uint		{ return ActionType.UPDATE_STYLE; }
		
		public function run(scene		: IScene, 
							visitor		: ISceneVisitor, 
							renderer	: IRenderer) : Boolean
		{
			var skinnedMesh			: SkinnedMesh			= SkinnedMesh(scene);
			
			var skeletonRootName	: String				= skinnedMesh.skeletonRootName;
			var skeletonReference	: IGroup				= skinnedMesh.skeletonReference;
			
			var jointNames			: Vector.<String>		= skinnedMesh.jointNames;
			var bindShapeMatrix		: Matrix4x4				= skinnedMesh.bindShapeMatrix;
			var invBindMatrices		: Vector.<Matrix4x4>	= skinnedMesh.inverseBindMatrices;
			
			var jointCount			: uint					= jointNames.length;
			
			// Create a vector to store each quaternion, and another one for matrices
			var skinningDQn			: Vector.<Vector4>		= new Vector.<Vector4>(jointCount);
			var skinningDQd			: Vector.<Vector4>		= new Vector.<Vector4>(jointCount);
			var boneMatrices		: Vector.<Matrix4x4>	= new Vector.<Matrix4x4>(jointCount);
			
			// if skeletonRootName is unknown, we search for the bones directly from skeletonReference
			if (skeletonRootName != null)
				findSkeletonRoot(skeletonReference, skeletonRootName, jointNames, bindShapeMatrix, invBindMatrices, skinningDQn, skinningDQd, boneMatrices);
			else
				fillSkinningMatrices(skeletonReference, jointNames, bindShapeMatrix, invBindMatrices, skinningDQn, skinningDQd, boneMatrices);
			
			// write all needed data into the stylestack for futher rendering
			visitor.renderingData.styleStack.push(EMPTY_STYLE);
			visitor.renderingData.styleStack.set(SkinningStyle.MAX_INFLUENCES, skinnedMesh.maxInfluences);
			visitor.renderingData.styleStack.set(SkinningStyle.NUM_BONES, jointCount);
			visitor.renderingData.styleStack.set(SkinningStyle.BONE_QN, skinningDQn);
			visitor.renderingData.styleStack.set(SkinningStyle.BONE_QD, skinningDQd);
			visitor.renderingData.styleStack.set(SkinningStyle.BIND_SHAPE, bindShapeMatrix);
			visitor.renderingData.styleStack.set(SkinningStyle.BONE_MATRICES, boneMatrices);
			
			return true;
		}
		
		private function findSkeletonRoot(currentNode		: IGroup,
										  skeletonRootName	: String,
										  jointNames		: Vector.<String>,
										  bindShapeMatrix	: Matrix4x4,
										  invBindMatrices	: Vector.<Matrix4x4>,
										  skinningDQn		: Vector.<Vector4>,
										  skinningDQd		: Vector.<Vector4>,
										  boneMatrices		: Vector.<Matrix4x4>) : void
		{
			// FIXME this patch is because in some collada files, joints are not tagged as such, and must be fixed ASAP in the importer
			if (/*currentNode is Joint && */currentNode.name == skeletonRootName)
			{
				fillSkinningMatrices(currentNode, jointNames, bindShapeMatrix, invBindMatrices, skinningDQn, skinningDQd, boneMatrices);
				return;
			}
			else
			{
				var transformNode : ITransformableScene = currentNode as ITransformableScene;
				
				if (transformNode != null)
					TMP_LOCAL_MATRIX.push().multiply(transformNode.transform);
				
				var numChildren : uint = currentNode.numChildren;
				for (var i : uint = 0; i < numChildren; ++i)
				{
					var child : Group = currentNode.getChildAt(i) as Group;
					if (child != null)
						findSkeletonRoot(child, skeletonRootName, jointNames, bindShapeMatrix, invBindMatrices, skinningDQn, skinningDQd, boneMatrices);
				}
				
				if (transformNode != null)
					TMP_LOCAL_MATRIX.pop();
			}
		}
		
		private function fillSkinningMatrices(currentNode			: IGroup,
											  jointNames			: Vector.<String>,
											  bindShapeMatrix		: Matrix4x4,
											  invBindMatrices		: Vector.<Matrix4x4>,
											  skinningDQn			: Vector.<Vector4>,
											  skinningDQd			: Vector.<Vector4>,
											  boneMatrices			: Vector.<Matrix4x4>) : void
		{
			if (currentNode is ITransformableScene)
				TMP_LOCAL_MATRIX.push().multiply(ITransformableScene(currentNode).transform);
			
			// FIXME same patch than line 87
			if (currentNode is Joint || currentNode is TransformGroup) 
			{
				// FIXME same as line 87
				var currentJointName	: String	= currentNode is Joint ? Joint(currentNode).boneName : currentNode.name;
				var jointCount			: uint		= jointNames.length;
				
				for (var i : uint = 0; i < jointCount; ++i)
					if (jointNames[i] == currentJointName)
					{
						var matrix : Matrix4x4 = TMP_SKINNING_MATRIX
							.identity()
							.multiply(TMP_LOCAL_MATRIX)
							.multiply(invBindMatrices[i]);
						
						boneMatrices[i] = Matrix4x4.copy(matrix);
						skinningDQn[i]	= new Vector4();
						skinningDQd[i]	= new Vector4();
						matrix.toDualQuaternion(skinningDQn[i], skinningDQd[i]);
						break;
					}
			}
			
			var numChildren : uint = currentNode.numChildren;
			
			for (i = 0; i < numChildren; ++i)
			{
				var child : Group = currentNode.getChildAt(i) as Group;
				if (child != null)
					fillSkinningMatrices(child, jointNames, bindShapeMatrix, invBindMatrices, skinningDQn, skinningDQd, boneMatrices);
			}
			
			if (currentNode is ITransformableScene)
				TMP_LOCAL_MATRIX.pop();
		}
	}
}