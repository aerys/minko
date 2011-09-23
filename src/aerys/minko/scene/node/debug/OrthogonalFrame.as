package aerys.minko.scene.node.debug
{
	import aerys.minko.scene.node.Model;
	import aerys.minko.scene.node.group.Group;
	import aerys.minko.scene.node.group.TransformGroup;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.scene.node.mesh.modifier.ColorMeshModifier;
	import aerys.minko.scene.node.mesh.modifier.NormalMeshModifier;
	import aerys.minko.scene.node.mesh.primitive.CubeMesh;
	import aerys.minko.scene.node.texture.BitmapTexture;
	
	import flash.display.BitmapData;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	
	public class OrthogonalFrame extends Group
	{
		public function OrthogonalFrame()
		{
			var cubeMesh : IMesh = new NormalMeshModifier(new CubeMesh());
			
			var repere0	: Model = new Model(new ColorMeshModifier(cubeMesh, Vector.<uint>([0xffffff])));
			var repereX	: Model = new Model(cubeMesh);
			var repereY	: Model = new Model(cubeMesh);
			var repereZ	: Model = new Model(cubeMesh);
			
			var text : TextField	= new TextField();
			var tmp : BitmapData 	= new BitmapData(32, 32, false, 0xffff0000);
			
			text.width = 32;
			text.height = 32;
			text.autoSize = TextFieldAutoSize.CENTER;
			
			text.text = "x";
			tmp.draw(text);
			repereX.textures.addChild(new BitmapTexture(tmp));
			
			tmp.fillRect(tmp.rect, 0xff00ff00);
			text.text = "y";
			tmp.draw(text);
			repereY.textures.addChild(new BitmapTexture(tmp));
			
			tmp.fillRect(tmp.rect, 0xff0000ff);
			text.text = "z";
			tmp.draw(text);
			repereZ.textures.addChild(new BitmapTexture(tmp));
			
			repereX.transform.appendTranslation(1, 0, 0);
			repereY.transform.appendTranslation(0, 1, 0);
			repereZ.transform.appendTranslation(0, 0, 1);
			
			super(repere0, repereX, repereY, repereZ);
		}
	}
}
