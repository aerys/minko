package aerys.minko.scene.controller.debug
{
	import aerys.minko.render.geometry.primitive.CubeGeometry;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.material.basic.BasicMaterial;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.type.enum.DepthTest;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * Add debug meshes to see the joint and the bone described by a Group node.
	 * This controller can be added to every joint of a skeleton to display it:
	 * 
	 * <pre>
	 * var debugCtrl : JointsDebugController = new JointsDebugController();
	 * 
	 * for each (var m : Mesh in result.get('//mesh[hasController(SkinningController)]'))
	 * {
	 *   var skinningCtrl : SkinningController = m.getControllersByType(SkinningController)[0]
	 *     as SkinningController;
	 * 
	 *   for each (var joint : Group in skinningCtrl.joints)
	 *     if (joint.getControllersByType(JointsDebugController).length == 0)
	 *       joint.addController(debugCtrl);
	 * }
	 * </pre> 
	 * 
	 * To remove this controller, you can fetch all the group that have it and remove it from
	 * their controllers:
	 * 
	 * <pre>
	 * for each (var node : ISceneNode in result.get('//group[hasController(JointsDebugController)]'))
	 *   node.removeController(debugCtrl);
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
			super(Group);
			
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
											target	: Group) : void
		{
			var numChildren : uint = target.numChildren;
			
			for (var i : uint = 0; i < numChildren; ++i)
			{
				var child : Group = target.getChildAt(i) as Group;
				
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
					target.addChild(jointMesh);
					
					if (boneLength != 0.)
					{
						boneMesh.transform
							.lookAt(nextJointPosition)
							.prependTranslation(0, 0, boneLength * .5)
							.prependScale(.02, .02, boneLength);
						
						target.addChild(boneMesh);
					}
				}
			}
		}
		
		private function targetRemovedHandler(ctrl 		: AbstractController,
											  target	: Group) : void
		{
			for each (var jointMesh : Mesh in target.get("/mesh[name='__joint__']"))
				jointMesh.parent = null;
			
			for each (var boneMesh : Mesh in target.get("/mesh[name='__bone__']"))
				boneMesh.parent = null;
		}
	}
}