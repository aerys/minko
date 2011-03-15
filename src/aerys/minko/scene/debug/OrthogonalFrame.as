package aerys.minko.scene.debug
{
	import aerys.minko.scene.Model3D;
	import aerys.minko.scene.group.Group3D;
	import aerys.minko.scene.group.TransformGroup3D;
	import aerys.minko.scene.material.ColorMaterial3D;
	import aerys.minko.scene.mesh.primitive.CubeMesh3D;
	
	public class OrthogonalFrame extends Group3D
	{
		public function OrthogonalFrame()
		{
			var repere0	: TransformGroup3D = new TransformGroup3D(new Model3D(new CubeMesh3D(), ColorMaterial3D.WHITE));
			var repereX	: TransformGroup3D = new TransformGroup3D(new Model3D(new CubeMesh3D(), ColorMaterial3D.RED));
			var repereY	: TransformGroup3D = new TransformGroup3D(new Model3D(new CubeMesh3D(), ColorMaterial3D.GREEN));
			var repereZ	: TransformGroup3D = new TransformGroup3D(new Model3D(new CubeMesh3D(), ColorMaterial3D.BLUE));
			
			repereX.transform.appendTranslation(1, 0, 0);
			repereY.transform.appendTranslation(0, 1, 0);
			repereZ.transform.appendTranslation(0, 0, 1);
			
			super(repere0, repereX, repereY, repereZ);
		}
	}
}
