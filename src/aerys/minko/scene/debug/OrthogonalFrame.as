package aerys.minko.scene.debug
{
	import aerys.minko.scene.Model3D;
	import aerys.minko.scene.group.Group3D;
	import aerys.minko.scene.group.TransformGroup3D;
	import aerys.minko.scene.material.ColorMaterial3D;
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.scene.mesh.modifier.NormalMesh3D;
	import aerys.minko.scene.mesh.primitive.CubeMesh3D;
	
	public class OrthogonalFrame extends TransformGroup3D
	{
		public function OrthogonalFrame(noBaseEffect : Boolean = false)
		{
			var cubeMesh : IMesh3D = new NormalMesh3D(new CubeMesh3D());
			
			var repere0	: TransformGroup3D = new TransformGroup3D(new Model3D(cubeMesh, ColorMaterial3D.WHITE, noBaseEffect));
			var repereX	: TransformGroup3D = new TransformGroup3D(new Model3D(cubeMesh, ColorMaterial3D.RED, noBaseEffect));
			var repereY	: TransformGroup3D = new TransformGroup3D(new Model3D(cubeMesh, ColorMaterial3D.GREEN, noBaseEffect));
			var repereZ	: TransformGroup3D = new TransformGroup3D(new Model3D(cubeMesh, ColorMaterial3D.BLUE, noBaseEffect));
			
			repereX.transform.appendTranslation(1, 0, 0);
			repereY.transform.appendTranslation(0, 1, 0);
			repereZ.transform.appendTranslation(0, 0, 1);
			
			super(repere0, repereX, repereY, repereZ);
		}
	}
}
