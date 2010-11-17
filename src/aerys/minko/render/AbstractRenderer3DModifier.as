package aerys.minko.render
{
	import aerys.minko.render.state.RenderStatesManager;
	import aerys.minko.render.transform.TransformManager;
	import aerys.minko.type.stream.IndexStream3D;
	
	import flash.display3D.TextureBase3D;
	
	public class AbstractRenderer3DModifier implements IRenderer3DModifier
	{
		private var _target			: IRenderer3D	= null;
		
		public function AbstractRenderer3DModifier(myTarget : IRenderer3D)
		{
			_target = myTarget;
		}
		
		public function get transform()		: TransformManager			{ return _target.transform; }
		public function get states()		: RenderStatesManager		{ return _target.states; }
		public function get viewport()		: Viewport3D				{ return _target.viewport; }
		public function get target()		: IRenderer3D				{ return _target; }
		public function get numTriangles()	: uint						{ return _target.numTriangles; }
		public function get textures()		: Vector.<TextureBase3D>	{ return _target.textures; }
		public function get drawingTime()	: int						{ return _target.drawingTime; }
		
		public function set target(value : IRenderer3D) : void
		{
			_target = value;
		}
		
		public function drawTriangles(myIndexStream 	: IndexStream3D,
									  myOffset			: uint = 0,
									  myNumTriangles	: uint = -1) : void
		{
			_target.drawTriangles(myIndexStream, myOffset, myNumTriangles);
		}
		
		public function clear(color : uint = 0) : void
		{
			_target.clear(color);
		}
		
		public function present() : void
		{
			_target.present();
		}
	}
}