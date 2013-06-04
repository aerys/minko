package aerys.minko.scene.controller.debug
{
	import flash.utils.Dictionary;
	
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.Effect;
	import aerys.minko.render.geometry.primitive.LineGeometry;
	import aerys.minko.render.material.line.LineMaterial;
	import aerys.minko.render.material.line.LineShader;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.mesh.skinning.SkinningController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.type.enum.DepthTest;
	import aerys.minko.type.enum.TriangleCulling;
	import aerys.minko.type.math.Vector4;
	
	public final class BonesDebugController extends AbstractController
	{
		private var _bonesThickness		: Number;
		private var _material   		: LineMaterial;
		private var _targetToBonesMesh	: Dictionary = new Dictionary();
		private var _numExtraDescendant	: uint 		 = 0;
		
		public function BonesDebugController(bonesThickness		: Number		= 0.02,
											 bonesMaterial 		: LineMaterial 	= null,
											 numExtraDescendant	: uint 			= 0)
		{
			super(Mesh);
			
			_numExtraDescendant = numExtraDescendant;
			initialize(bonesThickness, bonesMaterial);
		}
		
		private function initialize(bonesThickness : Number, bonesMaterial : LineMaterial) : void
		{
			_bonesThickness = bonesThickness;
			_material = bonesMaterial;
			
			if (!_material)
			{
				var bonesBasicMaterial : LineMaterial = new LineMaterial();
				
				bonesBasicMaterial.effect = new Effect(new LineShader(null, -Number.MAX_VALUE));
				bonesBasicMaterial.diffuseColor = 0xffe400ff;
				bonesBasicMaterial.setProperty('depthWriteEnabled', false);
				bonesBasicMaterial.setProperty('depthTest', DepthTest.ALWAYS);
				bonesBasicMaterial.setProperty('triangleCulling', TriangleCulling.NONE);
				_material = bonesBasicMaterial;
			}
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(ctrl	: AbstractController,
											target	: Mesh) : void
		{
			var skinningCtrl : SkinningController = target.getControllersByType(SkinningController)[0]
				as SkinningController;
			
			_targetToBonesMesh[target] = [];
			
			for each (var joint : Group in skinningCtrl.joints)
				addBonesMeshes(joint, target);
		}
		
		private function targetRemovedHandler(ctrl 		: AbstractController,
											  target	: Mesh) : void
		{
			var skinningCtrl : SkinningController = target.getControllersByType(SkinningController)[0]
				as SkinningController;
			
			for each (var boneMesh : Mesh in _targetToBonesMesh[target])
				boneMesh.parent = null;
			
			delete _targetToBonesMesh[target];
		}
		
		private function addBonesMeshes(joint 	: Group,
										target 	: Mesh) : void
		{
			if (joint.getChildByName('__bone__'))
				return ;
			
			var numChildren : uint = joint.numChildren;
			
			for (var i : uint = 0; i < numChildren; ++i)
			{
				var child : ISceneNode = joint.getChildAt(i);
				
				if (child != null)
				{
					for (var extraDescendantCount : uint = 0; extraDescendantCount < _numExtraDescendant && child is Group; ++extraDescendantCount)
						child = Group(child).minko_scene::_children[0] as ISceneNode;
					
					var nextJointPosition 	: Vector4 	= child.transform.transformVector(
						Vector4.ZERO
					);
					var boneLength 			: Number 	= nextJointPosition.length;
					
					
					if (boneLength != 0.)
					{
						var lineGeometry : LineGeometry = new LineGeometry();
						var boneMesh : Mesh = new Mesh(
							lineGeometry, _material, '__bone__'
						);
						
//						boneMesh.transform
//							.lookAt(nextJointPosition)
//							.prependTranslation(0, 0, boneLength * .5)
//							.prependScale(_bonesThickness, _bonesThickness, boneLength);
//						
						lineGeometry.moveTo(0, 0, 0).lineTo(nextJointPosition.x, nextJointPosition.y, nextJointPosition.z);
						joint.addChild(boneMesh);
						_targetToBonesMesh[target].push(boneMesh);
					}
				}
			}
		}
	}
}