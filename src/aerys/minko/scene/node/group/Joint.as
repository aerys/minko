package aerys.minko.scene.node.group
{
	import aerys.minko.scene.node.Model;
	import aerys.minko.scene.node.mesh.modifier.NormalMeshModifier;
	import aerys.minko.scene.node.mesh.primitive.CubeMesh;
	
	public class Joint extends TransformGroup
	{
		private var _boneName : String;
		
		public function Joint()
		{
			super();
			
			//var debugCubeMesh : Model = new Model(new NormalMeshModifier(new CubeMesh()));
			//debugCubeMesh.transform.appendScale(0.05, 0.05, 0.05);
			//addChild(debugCubeMesh);
		}

		public function get boneName() : String
		{
			return _boneName;
		}

		public function set boneName(value : String):void
		{
			_boneName = value;
		}
	}
}
