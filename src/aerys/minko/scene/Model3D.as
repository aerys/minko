package aerys.minko.scene
{
	import aerys.minko.effect.Effect3DStyle;
	import aerys.minko.effect.IEffect3D;
	import aerys.minko.effect.IStyled3D;
	import aerys.minko.effect.basic.BasicEffect3D;
	import aerys.minko.effect.basic.BasicStyle3D;
	import aerys.minko.ns.minko;
	import aerys.minko.query.rendering.RenderingQuery;
	import aerys.minko.query.rendering.TransformManager;
	import aerys.minko.scene.material.IMaterial3D;
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.Matrix4x4;

	public class Model3D extends AbstractScene3D implements IScene3D, IObject3D, IStyled3D
	{
		use namespace minko;
		
		private var _mesh		: IMesh3D				= null;
		private var _material	: IMaterial3D			= null;
		private var _transform	: Transform3D			= new Transform3D();
		private var _visible	: Boolean				= true;
		private var _effects	: Vector.<IEffect3D>	= Vector.<IEffect3D>([new BasicEffect3D()]);
		private var _style		: Effect3DStyle			= new Effect3DStyle();
		private var _toScreen	: Matrix4x4				= new Matrix4x4();
		
		public function get transform() : Transform3D			{ return _transform; }
		public function get mesh()		: IMesh3D				{ return _mesh; }
		public function get material()	: IMaterial3D			{ return _material; }
		public function get visible()	: Boolean				{ return _visible; }
		public function get effects()	: Vector.<IEffect3D>	{ return _effects; }
		public function get style()		: Effect3DStyle			{ return _style; }
		
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
		
		public function Model3D(mesh 	 : IMesh3D		= null,
								material : IMaterial3D	= null)
		{
			super();
			
			_mesh = mesh;
			_material = material;
		}
		
		override protected function acceptRenderingQuery(query : RenderingQuery) : void 
		{
			var transform 		: TransformManager 		= query.transform;
			var numEffects		: int					= _effects.length;
			var queryEffects	: Vector.<IEffect3D>	= query.effects;
			var numQueryEffects	: int					= queryEffects.length;
			
			// push world transform
			//transform.push(TransformType.WORLD);
			transform.world.push()
						   .multiply(_transform);
			transform.getLocalToScreen(_toScreen);
			
			_style.set(BasicStyle3D.WORLD_MATRIX, transform.world)
				  .set(BasicStyle3D.VIEW_MATRIX, transform.view)
				  .set(BasicStyle3D.PROJECTION_MATRIX, transform.projection)
				  .set(BasicStyle3D.LOCAL_TO_SCREEN_MATRIX, _toScreen);
			
			// push FXs and style
			query.effects = _effects.concat(queryEffects);
			/*for (var i : int = 0; i < numEffects; ++i)
				queryEffects.unshift(_effects[i]);*/
				//queryEffects[int(numQueryEffects + i)] = _effects[i];	
			//query.style = _style.override(query.style);
			query.style.push(_style);
			
			_material && query.query(_material);
			_mesh && query.query(_mesh);
			
			// pop FXs and style
			//query.style = query.style.override();
			query.style.pop();
			query.effects = queryEffects;
			
			// pop world transform
			transform.world.pop();
		}
	}
}