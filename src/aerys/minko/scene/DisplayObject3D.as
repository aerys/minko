package aerys.minko.scene
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.renderer.IRenderer3D;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.render.transform.TransformManager;
	import aerys.minko.render.transform.TransformType;
	import aerys.minko.render.visitor.IScene3DVisitor;
	import aerys.minko.scene.material.IMaterial3D;
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.transform.Transform3D;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DProgramType;

	public class DisplayObject3D extends AbstractScene3D implements IScene3D, IObject3D
	{
		use namespace minko;
		
		private var _mesh		: IMesh3D		= null;
		private var _material	: IMaterial3D	= null;
		private var _transform	: Transform3D	= new Transform3D();
		private var _visible	: Boolean		= true;
		
		public function get transform() 	: Transform3D	{ return _transform; }
		public function get mesh()			: IMesh3D		{ return _mesh; }
		public function get material()		: IMaterial3D	{ return _material; }
		public function get visible()		: Boolean		{ return _visible; }
		
		public function set mesh(value : IMesh3D) : void
		{
			_mesh = value;
		}
		
		public function set material(value : IMaterial3D) : void
		{
			_material = value;
		}
		
		public function set visible(value : Boolean) : void
		{
			_visible = value;
		}
		
		public function DisplayObject3D(mesh 	 : IMesh3D		= null,
										material : IMaterial3D	= null)
		{
			super();
			
			_mesh = mesh;
			_material = material;
		}
		
		override public function visited(visitor : IScene3DVisitor) : void
		{
			var renderer	: IRenderer3D	= visitor.renderer;
			
			if (renderer.isReady && _visible)
			{
				var transform 	: TransformManager 	= renderer.transform;
				
				transform.push(TransformType.WORLD);
				transform.world.multiply(_transform);
				
				renderer.states.push(RenderState.ALL);
				
				_mesh && visitor.visit(_mesh);
				_material && visitor.visit(_material);
				
				renderer.setMatrix(0,
								   transform.getLocalToScreenMatrix(),
								   Context3DProgramType.VERTEX,
								   true);
				
				_mesh && renderer.drawTriangles(_mesh.indexStream);
			
				renderer.states.pop();
				transform.pop();
			}
		}
	}
}