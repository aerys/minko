package aerys.minko.scene.node.debug
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.scene.node.Model;
	import aerys.minko.scene.node.mesh.primitive.CubeMesh;
	import aerys.minko.scene.node.texture.BitmapTexture;

	import flash.display.CapsStyle;
	import flash.display.LineScaleMode;
	import flash.display.Shape;

	public class DebugBox extends Model
	{
		public function DebugBox(argb : uint = 0xffffffff)
		{
			var color : int = argb & 0xffffff;
			var alpha : Number = ((argb >> 24) & 0xff) / 255.;
			var shape : Shape = new Shape();

			shape.graphics.lineStyle(4, color, alpha, false, LineScaleMode.NONE, CapsStyle.NONE);
			shape.graphics.beginFill(0, 0.);
			shape.graphics.drawRect(2, 2, 252, 252);

			var mat : BitmapTexture = BitmapTexture.fromDisplayObject(shape, 256, true);

			super(CubeMesh.cubeMesh, mat);

			style.set(BasicStyle.BLENDING, Blending.ADDITIVE);
		}
	}
}