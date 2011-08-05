package aerys.minko.scene.node.group
{
	import aerys.minko.scene.node.Model;
	import aerys.minko.scene.node.mesh.modifier.NormalMeshModifier;
	import aerys.minko.scene.node.mesh.primitive.CubeMesh;
	import aerys.minko.scene.node.texture.ColorTexture;
	
	public class Joint extends TransformGroup
	{
		private var _boneName : String;
		
		public function Joint()
		{
			super();
			
//			var debugCubeMesh : Model = new Model(new NormalMeshModifier(new CubeMesh()), new ColorTexture(0xffFF0000));
//			addChild(debugCubeMesh);
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
