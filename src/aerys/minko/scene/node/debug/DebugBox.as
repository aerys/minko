package aerys.minko.scene.node.debug
{
	import aerys.minko.render.state.Blending;
	import aerys.minko.scene.node.Model;
	import aerys.minko.scene.node.texture.BitmapTexture;
	import aerys.minko.scene.node.mesh.primitive.CubeMesh;
	
	import flash.display.CapsStyle;
	import flash.display.LineScaleMode;
	import flash.display.Shape;
	
	public class DebugBox extends Model
	{
		public function DebugBox(color : uint = 0xff0000)
		{
			var shape : Shape = new Shape();
			
			shape.graphics.lineStyle(4, color, 1., false, LineScaleMode.NORMAL, CapsStyle.SQUARE);
			shape.graphics.beginFill(0, 0.);
			shape.graphics.drawRect(2, 2, 252, 252);
			
			var mat : BitmapTexture = BitmapTexture.fromDisplayObject(shape, 256, true);
			
			super(CubeMesh.cubeMesh, mat);
		}
	}
}