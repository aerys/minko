package aerys.minko.scene.debug
{
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.IScene3DVisitor;
	import aerys.minko.render.Viewport3D;
	import aerys.minko.render.state.BlendingDestination;
	import aerys.minko.render.state.BlendingSource;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.render.state.RenderStatesManager;
	import aerys.minko.scene.DisplayObject3D;
	import aerys.minko.scene.material.BitmapMaterial3D;
	import aerys.minko.scene.mesh.primitive.CubeMesh;
	
	import flash.display.CapsStyle;
	import flash.display.Shape;
	import flash.display3D.Context3DBlendMode;
	import flash.display3D.Context3DTriangleFace;
	
	public class DebugBox3D extends DisplayObject3D
	{
		public function DebugBox3D(myColor : uint = 0xff0000)
		{
			var shape : Shape = new Shape();
			
			shape.graphics.lineStyle(4, myColor, 1., false, "normal", CapsStyle.SQUARE);
			shape.graphics.beginFill(0, 0.);
			shape.graphics.drawRect(2, 2, 252, 252);
			
			super(CubeMesh.cubeMesh,
				  BitmapMaterial3D.fromDisplayObject(shape, 256, true));
		}
		
		override public function visited(myVisitor : IScene3DVisitor) : void
		{
			var renderer 		: IRenderer3D			= myVisitor.renderer;
			var viewport		: Viewport3D			= renderer.viewport;
			var renderStates	: RenderStatesManager	= renderer.states;
			
			viewport.context.setCulling(Context3DTriangleFace.NONE);
			
			renderStates.push(RenderState.BLENDING);
			renderStates.blending = BlendingSource.SOURCE_ALPHA
									| BlendingDestination.ONE_MINUS_SOURCE_ALPHA;
			
			super.visited(myVisitor);
			
			viewport.context.setCulling(Context3DTriangleFace.FRONT);
			
			renderStates.pop();
		}
	}
}