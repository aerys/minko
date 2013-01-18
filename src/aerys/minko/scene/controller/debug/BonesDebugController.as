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
    
    public final class BonesDebugController extends AbstractController
    {
        private var _material   : Material;
        
        public function BonesDebugController(bonesMaterial : Material = null)
        {
            super(Mesh);
            
            initialize(bonesMaterial);
        }
        
        private function initialize(bonesMaterial : Material) : void
        {
            _material = bonesMaterial;
            if (!_material)
            {
                var bonesBasicMaterial : BasicMaterial = new BasicMaterial();
                
                bonesBasicMaterial.diffuseColor = 0x00ff00ff;
                bonesBasicMaterial.depthWriteEnabled = false;
                bonesBasicMaterial.depthTest = DepthTest.ALWAYS;
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
            
            for each (var joint : Group in skinningCtrl.joints)
                addBonesMeshes(joint);
        }
        
        private function targetRemovedHandler(ctrl 		: AbstractController,
                                              target	: Mesh) : void
        {
            var skinningCtrl : SkinningController = target.getControllersByType(SkinningController)[0]
                as SkinningController;
            
            for each (var joint : Group in skinningCtrl.joints)
                joint.getChildByName('__bone__').parent = null;
        }
        
        private function addBonesMeshes(joint : Group) : void
        {
            if (joint.getChildByName('__bone__'))
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
                    
                    
                    if (boneLength != 0.)
                    {
                        var boneMesh : Mesh = new Mesh(
                            CubeGeometry.cubeGeometry, _material, '__bone__'
                        );
                        
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