package aerys.minko.scene.debug
{
	import aerys.minko.render.state.Blending;
	import aerys.minko.scene.Model3D;
	import aerys.minko.scene.material.NativeMaterial3D;
	import aerys.minko.scene.mesh.primitive.CubeMesh3D;
	
	import flash.display.CapsStyle;
	import flash.display.Shape;
	
	public class DebugBox3D extends Model3D
	{
		public function DebugBox3D(color : uint = 0xff0000)
		{
			var shape : Shape = new Shape();
			
			shape.graphics.lineStyle(4, color, 1., false, "normal", CapsStyle.SQUARE);
			shape.graphics.beginFill(0, 0.);
			shape.graphics.drawRect(2, 2, 252, 252);
			
			var mat : NativeMaterial3D = NativeMaterial3D.fromDisplayObject(shape, 256, true);
			
			mat.blending = Blending.ADDITIVE;
				
			super(CubeMesh3D.cubeMesh, mat);
		}
		
		/*override public function accept(query : IScene3DQuery) : void
		{
			if (query is RenderingQuery)
			{
				var renderer	: IRenderer3D			= (query as RenderingQuery).renderer;
				var states		: RenderStatesManager	= renderer.states;
	
				states.push(RenderState.TRIANGLE_CULLING);
				states.triangleCulling = TriangleCulling.DISABLED;
				
				super.accept(query);
				
				states.pop();
			}
		}*/
	}
}