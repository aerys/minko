package aerys.minko.scene.graph.debug
{
	import aerys.minko.scene.graph.IScene;
	import aerys.minko.scene.graph.Model;
	import aerys.minko.scene.graph.group.Group;
	import aerys.minko.scene.graph.group.TransformGroup;
	import aerys.minko.scene.graph.texture.BitmapTexture;
	import aerys.minko.scene.graph.texture.ColorTexture;
	import aerys.minko.scene.graph.mesh.IMesh;
	import aerys.minko.scene.graph.mesh.modifier.NormalMesh;
	import aerys.minko.scene.graph.mesh.primitive.CubeMesh;
	
	import flash.display.BitmapData;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	
	public class OrthogonalFrame extends TransformGroup
	{
		public function OrthogonalFrame(noBaseEffect : Boolean = false)
		{
			var cubeMesh : IMesh = new NormalMesh(new CubeMesh());
			
			var repere0	: Model = new Model(cubeMesh, ColorTexture.WHITE, noBaseEffect);
			var repereX	: Model = new Model(cubeMesh, null, noBaseEffect);
			var repereY	: Model = new Model(cubeMesh, null, noBaseEffect);
			var repereZ	: Model = new Model(cubeMesh, null, noBaseEffect);
			
			var text : TextField	= new TextField();
			var tmp : BitmapData 	= new BitmapData(32, 32, false, 0xffff0000);
			
			text.width = 32;
			text.height = 32;
			text.autoSize = TextFieldAutoSize.CENTER;
			
			text.text = "x";
			tmp.draw(text);
			repereX.material = new BitmapTexture(tmp);
			
			tmp.fillRect(tmp.rect, 0xff00ff00);
			text.text = "y";
			tmp.draw(text);
			repereY.material = new BitmapTexture(tmp);
			
			tmp.fillRect(tmp.rect, 0xff0000ff);
			text.text = "z";
			tmp.draw(text);
			repereZ.material = new BitmapTexture(tmp);
			
			repereX.transform.appendTranslation(1, 0, 0);
			repereY.transform.appendTranslation(0, 1, 0);
			repereZ.transform.appendTranslation(0, 0, 1);
			
			super(repere0, repereX, repereY, repereZ);
		}
	}
}
