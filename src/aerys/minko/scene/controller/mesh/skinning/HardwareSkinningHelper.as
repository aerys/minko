package aerys.minko.scene.controller.mesh.skinning
{
	import aerys.minko.Minko;
	import aerys.minko.scene.data.SkinningDataProvider;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.type.animation.SkinningMethod;
	import aerys.minko.type.log.DebugLevel;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;
	import flash.utils.Dictionary;

	internal final class HardwareSkinningHelper extends AbstractSkinningHelper
	{
		private var _meshToProvider	: Dictionary	= new Dictionary();
		
		public function HardwareSkinningHelper(method			: uint,
											 bindShape			: Matrix3D,
											 invBindMatrices	: Vector.<Matrix3D>)
		{
			super(method, bindShape, invBindMatrices);
			
			if ((Minko.debugLevel & DebugLevel.SKINNING) != 0 && method & SkinningMethod.HARDWARE_DUAL_QUATERNION)
			{
				var scale : Vector3D;
				
				scale = bindShape.decompose()[2];
				if (scale.x != 1 || scale.y != 1 || scale.z != 1)
					Minko.log(
						DebugLevel.SKINNING,
						'Cannot use Dual Quaternion skinning when the bindingShape have a scale. '
						+ 'Please chose another SkinningMethod.'
					);
				
				for each (var invBindMatrix : Matrix3D in _invBindMatrices)
					if (scale.x != 1 || scale.y != 1 || scale.z != 1)
					{
						Minko.log(
							DebugLevel.SKINNING,
							'Cannot use Dual Quaternion skinning when at least one inverse bind'
							+ 'matrix have a scale. Please chose another SkinningMethod.'
						);
						break;
					}
			}
		}
		
		override public function addMesh(mesh:Mesh):void
		{
			super.addMesh(mesh);
			
			var provider : SkinningDataProvider = new SkinningDataProvider();
			
			provider.method			= _method;
			provider.numBones		= _invBindMatrices.length;
			provider.maxInfluences	= getNumInfluences(mesh.geometry.getVertexStream().format);
			provider.matrices		= _matrices;
			provider.dqN			= _dqn;
			provider.dqD			= _dqd;
			
			mesh.bindings.addProvider(provider);
			_meshToProvider[mesh] = provider;
		}
		
		override public function removeMesh(mesh : Mesh):void
		{
			super.removeMesh(mesh);
			
			mesh.bindings.removeProvider(_meshToProvider[mesh]);
			delete _meshToProvider[mesh];
		}
		
		override public function update(skeletonRoot	: Group, 
										joints			: Vector.<Group>) : void 
		{
			var numTargets	: uint = _targets.length;
			var targetId	: uint;
			
			switch (_method)
			{
				case SkinningMethod.HARDWARE_MATRIX:
					writeMatrices(skeletonRoot, joints);
					
					for (targetId = 0; targetId < numTargets; ++targetId)
						_meshToProvider[_targets[targetId]].matrices = _matrices;
					
					break;
				
				case SkinningMethod.HARDWARE_DUAL_QUATERNION:
					writeMatrices(skeletonRoot, joints);
					writeDualQuaternions();
					
					for (targetId = 0; targetId < numTargets; ++targetId)
					{
						var provider : SkinningDataProvider	= _meshToProvider[_targets[targetId]];
						
						provider.dqN = _dqn;
						provider.dqD = _dqd;
					}
					break;
				
				case SkinningMethod.HARDWARE_DUAL_QUATERNION_SCALE:
					throw new Error('This skinning method is yet to be implemented.');
					
				default:
					throw new Error('Invalid skinning method.');
			}
		}
	}
}
