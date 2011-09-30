package aerys.minko.scene.action.mesh
{
	import aerys.minko.render.effect.Style;
	import aerys.minko.render.effect.animation.AnimationStyle;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.data.StyleData;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.ITransformableScene;
	import aerys.minko.scene.node.group.Group;
	import aerys.minko.scene.node.group.IGroup;
	import aerys.minko.scene.node.group.Joint;
	import aerys.minko.scene.node.group.TransformGroup;
	import aerys.minko.scene.node.mesh.SkinnedMesh;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.type.Factory;
	import aerys.minko.type.math.Matrix3D;
	import aerys.minko.type.math.Vector4;
	
	public final class PushMeshSkinAction implements IAction
	{
		private static const VECTOR4_FACTORY		: Factory	= Factory.getFactory(Vector4);
		private static const TMP_LOCAL_MATRIX		: Matrix3D	= new Matrix3D();
		private static const TMP_SKINNING_MATRIX	: Matrix3D 	= new Matrix3D();
		private static const EMPTY_STYLE			: Style		= new Style();

		private var _boneMatrices	: Vector.<Matrix3D>	= new Vector.<Matrix3D>();
		private var _skinningDQn	: Vector.<Vector4>	= new Vector.<Vector4>();
		private var _skinningDQd	: Vector.<Vector4>	= new Vector.<Vector4>();
		private var _style			: Style				= null;
		
		public function get type() : uint		{ return ActionType.UPDATE_STYLE; }
		
		public function run(scene		: IScene, 
							visitor		: ISceneVisitor, 
							renderer	: IRenderer) : Boolean
		{
			loadSkinningData(scene, visitor.renderingData.styleStack);
			
			return true;
		}
		
		public function loadSkinningData(scene		: IScene,
										 styleStack	: StyleData) : void
		{
			var skinnedMesh			: SkinnedMesh			= SkinnedMesh(scene);
			
			var skeletonRootName	: String				= skinnedMesh.skeletonRootName;
			var skeletonReference	: IGroup				= skinnedMesh.skeletonReference;
			
			var jointNames			: Vector.<String>		= skinnedMesh.jointNames;
			var bindShapeMatrix		: Matrix3D				= skinnedMesh.bindShapeMatrix;
			var invBindMatrices		: Vector.<Matrix3D>		= skinnedMesh.inverseBindMatrices;
			
			var jointCount			: uint					= jointNames.length;
			
			if (jointCount > _boneMatrices.length)
			{
				for (var i : int = _boneMatrices.length; i < jointCount; ++i)
				{
					_boneMatrices[i] = new Matrix3D();
					_skinningDQd[i] = new Vector4();
					_skinningDQn[i] = new Vector4();
				}
			}
			
			// if skeletonRootName is unknown, we search for the bones directly from skeletonReference
			if (skeletonRootName != null)
				findSkeletonRoot(skeletonReference, skeletonRootName, jointNames, bindShapeMatrix, invBindMatrices);
			else
				fillSkinningMatrices(skeletonReference, jointNames, bindShapeMatrix, invBindMatrices);
			
			// write all needed data into the stylestack for futher rendering
//			styleStack.push(EMPTY_STYLE);
			
			if (_style == null)
			{
				_style = new Style();
				_style.set(AnimationStyle.MAX_INFLUENCES, 	skinnedMesh.maxInfluences)
					.set(AnimationStyle.NUM_BONES, 			jointCount)
					.set(AnimationStyle.BONE_QN, 			_skinningDQn)
					.set(AnimationStyle.BONE_QD, 			_skinningDQd)
					.set(AnimationStyle.BIND_SHAPE, 		bindShapeMatrix)
					.set(AnimationStyle.BONE_MATRICES, 		_boneMatrices);
			}
			styleStack.push(_style);
		}
		
		private function findSkeletonRoot(currentNode		: IGroup,
										  skeletonRootName	: String,
										  jointNames		: Vector.<String>,
										  bindShapeMatrix	: Matrix3D,
										  invBindMatrices	: Vector.<Matrix3D>) : void
		{
			// FIXME this patch is because in some collada files, joints are not tagged as such, and must be fixed ASAP in the importer
			if (/*currentNode is Joint && */currentNode.name == skeletonRootName)
			{
				fillSkinningMatrices(currentNode, jointNames, bindShapeMatrix, invBindMatrices);
				return;
			}
			else
			{
				var transformNode : ITransformableScene = currentNode as ITransformableScene;
				
				if (transformNode != null)
					TMP_LOCAL_MATRIX.push().prepend(transformNode.transform);
				
				var numChildren : uint = currentNode.numChildren;
				for (var i : uint = 0; i < numChildren; ++i)
				{
					var child : Group = currentNode.getChildAt(i) as Group;
					if (child != null)
						findSkeletonRoot(child, skeletonRootName, jointNames, bindShapeMatrix, invBindMatrices);
				}
				
				if (transformNode != null)
					TMP_LOCAL_MATRIX.pop();
			}
		}
		
		private function fillSkinningMatrices(currentNode			: IGroup,
											  jointNames			: Vector.<String>,
											  bindShapeMatrix		: Matrix3D,
											  invBindMatrices		: Vector.<Matrix3D>) : void
		{
			if (currentNode is ITransformableScene)
				TMP_LOCAL_MATRIX.push().prepend(ITransformableScene(currentNode).transform);
			
			// FIXME same patch than line 87
			if (currentNode is Joint || currentNode is TransformGroup) 
			{
				// FIXME same as line 87
				var currentJointName	: String	= currentNode is Joint ? Joint(currentNode).boneName : currentNode.name;
				var jointCount			: uint		= jointNames.length;
				
				for (var i : uint = 0; i < jointCount; ++i)
					if (jointNames[i] == currentJointName)
					{
						var matrix : Matrix3D = TMP_SKINNING_MATRIX
							.identity()
							.prepend(TMP_LOCAL_MATRIX)
							.prepend(invBindMatrices[i]);
						
						Matrix3D.copy(matrix, _boneMatrices[i]);
						matrix.toDualQuaternion(_skinningDQn[i], _skinningDQd[i]);
						
						break ;
					}
			}
			
			var numChildren : uint = currentNode.numChildren;
			
			for (i = 0; i < numChildren; ++i)
			{
				var child : Group = currentNode.getChildAt(i) as Group;
				
				if (child != null)
					fillSkinningMatrices(child, jointNames, bindShapeMatrix, invBindMatrices);
			}
			
			if (currentNode is ITransformableScene)
				TMP_LOCAL_MATRIX.pop();
		}
	}
}