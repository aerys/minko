package aerys.minko.scene.controller.debug
{
	import aerys.minko.render.geometry.primitive.CubeGeometry;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.material.basic.BasicMaterial;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.mesh.skinning.SkinningController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.type.enum.DepthTest;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * Add debug meshes to see the joints/bones of the skeleton of a skinned Mesh.
	 * This controller can target all the meshes with a SkinningController: 
	 * 
	 * <pre>
	 * var debugCtrl : JointsDebugController = new JointsDebugController();
	 * 
	 * for each (var mesh : Mesh in scene.get('//mesh[hasController(SkinningController)]'))
	 *   mesh.addController(debugCtrl);
	 * </pre>
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class JointsDebugController extends AbstractController
	{
		private var _jointsMaterial	: Material;
		private var _bonesMaterial	: Material;
		
		public function JointsDebugController(jointsMaterial	: Material	= null,
											  bonesMaterial		: Material	= null)
		{
			super(Mesh);
			
			initialize(jointsMaterial, bonesMaterial);
		}
		
		private function initialize(jointsMaterial	: Material,
									bonesMaterial	: Material) : void
		{
			_jointsMaterial = jointsMaterial;
			if (!_jointsMaterial)
			{
				var jointsBasicMaterial : BasicMaterial = new BasicMaterial();
				
				jointsBasicMaterial.diffuseColor = 0xff0000ff;
				jointsBasicMaterial.depthWriteEnabled = false;
				jointsBasicMaterial.depthTest = DepthTest.ALWAYS;
				_jointsMaterial = jointsBasicMaterial;
			}
			
			_bonesMaterial = bonesMaterial;
			if (!_bonesMaterial)
			{
				var bonesBasicMaterial : BasicMaterial = new BasicMaterial();
				
				bonesBasicMaterial.diffuseColor = 0x00ff00ff;
				bonesBasicMaterial.depthWriteEnabled = false;
				bonesBasicMaterial.depthTest = DepthTest.ALWAYS;
				_bonesMaterial = bonesBasicMaterial;
			}
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(ctrl	: AbstractController,
											target	: Mesh) : void
		{
			var skinningCtrl : SkinningController = target.getControllersByType(SkinningController)[0]
				as SkinningController;
			
			for each (var joint : Group in skinningCtrl.joints)
				addBonesAndJointsMeshes(joint);
		}
		
		private function targetRemovedHandler(ctrl 		: AbstractController,
											  target	: Mesh) : void
		{
			var skinningCtrl : SkinningController = target.getControllersByType(SkinningController)[0]
				as SkinningController;
			
			for each (var joint : Group in skinningCtrl.joints)
			{
				joint.getChildByName('__bone__').parent = null;
				joint.getChildByName('__joint__').parent = null;
			}
		}
		
		private function addBonesAndJointsMeshes(joint : Group) : void
		{
			if (joint.getChildByName('__bone__') || joint.getChildByName('__joint__'))
				return ;
			
			var numChildren : uint = joint.numChildren;
			
			for (var i : uint = 0; i < numChildren; ++i)
			{
				var child : Group = joint.getChildAt(i) as Group;
				
				if (child != null)
				{
					var nextJointPosition 	: Vector4 	= child.transform.transformVector(
						Vector4.ZERO
					);
					var boneLength 			: Number 	= nextJointPosition.length;
					var boneMesh 			: Mesh 		= new Mesh(
						CubeGeometry.cubeGeometry, _bonesMaterial, '__bone__'
					);
					var jointMesh			: Mesh		= new Mesh(
						CubeGeometry.cubeGeometry, _jointsMaterial, '__joint__'
					);
					
					jointMesh.transform.appendUniformScale(.08);
					joint.addChild(jointMesh);
					
					if (boneLength != 0.)
					{
						boneMesh.transform
							.lookAt(nextJointPosition)
							.prependTranslation(0, 0, boneLength * .5)
							.prependScale(.02, .02, boneLength);
						
						joint.addChild(boneMesh);
					}
				}
			}
		}
	}
}