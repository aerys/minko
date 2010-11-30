package aerys.minko.scene
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.IScene3DVisitor;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.render.transform.TransformManager;
	import aerys.minko.render.transform.TransformType;
	import aerys.minko.scene.material.IMaterial3D;
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.type.math.Transform3D;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DProgramType;

	public class DisplayObject3D extends AbstractScene3D implements IScene3D, IObject3D
	{
		use namespace minko;
		
		private var _mesh		: IMesh3D		= null;
		private var _material	: IMaterial3D	= null;
		private var _transform	: Transform3D	= new Transform3D();
		
		public function get transform() 	: Transform3D	{ return _transform; }
		public function get mesh()			: IMesh3D		{ return _mesh; }
		public function get material()		: IMaterial3D	{ return _material; }
		
		public function set mesh(value : IMesh3D) : void
		{
			_mesh = value;
		}
		
		public function set material(value : IMaterial3D) : void
		{
			_material = value;
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
			var renderer	: IRenderer3D		= visitor.renderer;
			var context 	: Context3D 		= renderer.viewport.context;
			var transform 	: TransformManager 	= renderer.transform;
			
			transform.push(TransformType.WORLD);
			transform.world.prepend(_transform);
			
			renderer.states.push(RenderState.ALL);
			
			_mesh && visitor.visit(_mesh);
			_material && visitor.visit(_material);
			
			context.setProgramConstantsFromMatrix(Context3DProgramType.VERTEX,
											  	  0,
											  	  transform.getLocalToScreenMatrix(),
											  	  true);
			
			_mesh && renderer.drawTriangles(_mesh.indexStream);
		
			renderer.states.pop();
			transform.pop();
		}
	}
}