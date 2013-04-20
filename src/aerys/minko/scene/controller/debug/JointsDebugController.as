package aerys.minko.scene.controller.debug
{
	import flash.utils.Dictionary;
	
	import aerys.minko.render.geometry.primitive.CubeGeometry;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.material.basic.BasicMaterial;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.mesh.skinning.SkinningController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.type.enum.DepthTest;
	
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
		private var _jointSize			: Number;
		private var _jointsMaterial		: Material;
		private var _targetToJointsMesh	: Dictionary = new Dictionary();
		
		public function JointsDebugController(jointsSize		: Number	= 0.08,
											  jointsMaterial 	: Material	= null)
		{
			super(Mesh);
			
			initialize(jointsSize, jointsMaterial);
		}
		
		private function initialize(jointSize : Number, jointsMaterial	: Material) : void
		{
			_jointSize = jointSize;
			_jointsMaterial = jointsMaterial;
			if (!_jointsMaterial)
			{
				var jointsBasicMaterial : BasicMaterial = new BasicMaterial();
				
				jointsBasicMaterial.diffuseColor = 0xff0000ff;
				jointsBasicMaterial.depthWriteEnabled = false;
				jointsBasicMaterial.depthTest = DepthTest.ALWAYS;
				_jointsMaterial = jointsBasicMaterial;
			}
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(ctrl	: AbstractController,
											target	: Mesh) : void
		{
			var skinningCtrl : SkinningController = target.getControllersByType(SkinningController)[0]
				as SkinningController;
			
			_targetToJointsMesh[target] = [];
			
			for each (var joint : Group in skinningCtrl.joints)
			addJointsMeshes(joint, target);
		}
		
		private function targetRemovedHandler(ctrl 		: AbstractController,
											  target	: Mesh) : void
		{
			var skinningCtrl : SkinningController = target.getControllersByType(SkinningController)[0]
				as SkinningController;
			
			for each (var jointMesh : Mesh in _targetToJointsMesh[target])
				jointMesh.parent = null;
			
			delete _targetToJointsMesh[target];
		}
		
		private function addJointsMeshes(joint 	: Group,
										 target : Mesh) : void
		{
			if (joint.getChildByName('__joint__'))
				return ;
			
			var numChildren : uint = joint.numChildren;
			
			for (var i : uint = 0; i < numChildren; ++i)
			{
				var child : Group = joint.getChildAt(i) as Group;
				
				if (child != null)
				{
					var jointMesh   : Mesh  = new Mesh(
						CubeGeometry.cubeGeometry, _jointsMaterial, '__joint__'
					);
					
					jointMesh.transform.appendUniformScale(_jointSize);
					joint.addChild(jointMesh);
					
					_targetToJointsMesh[target].push(jointMesh);
				}
			}
		}
	}
}