package aerys.minko.scene.graph
{
	import aerys.common.IVersionnable;
	import aerys.minko.effect.StyleStack;
	import aerys.minko.effect.IEffect;
	import aerys.minko.effect.IStyled;
	import aerys.minko.effect.Style;
	import aerys.minko.effect.basic.BasicEffect;
	import aerys.minko.effect.basic.BasicStyle;
	import aerys.minko.ns.minko;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	import aerys.minko.scene.visitor.rendering.TransformManager;
	import aerys.minko.scene.graph.texture.ITexture;
	import aerys.minko.scene.graph.mesh.IMesh;
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.Matrix4x4;

	/**
	 * Model3D objects are visible scene objects. They contain references to:
	 * <ul>
	 * <li>an IMesh3D object (geometry)</li>
	 * <li>an IMaterial3D object (texture data)</li>
	 * <li>a Style3D object (rendering parameters)</li>
	 * <li>and a Transform3D object (position, rotation and scale)</li>
	 * </ul>
	 * 
	 * A Model3D object can be seen as a combination of a TransformGroup3D and an EffectGroup3D
	 * that can only contain two children: a mesh and a material.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class Model extends AbstractScene implements IScene, ITransformable, IStyled, IVersionnable
	{
		use namespace minko;
		
		private var _version	: uint				= 0;
		
		private var _mesh		: IMesh				= null;
		private var _material	: ITexture			= null;
		private var _transform	: Transform3D		= new Transform3D();
		private var _visible	: Boolean			= true;
		private var _effects	: Vector.<IEffect>	= new Vector.<IEffect>();
		private var _style		: Style				= new Style();
		private var _toScreen	: Matrix4x4			= new Matrix4x4();
		
		public function get version() : uint
		{
			return _version
				   + _transform.version
				   + _style.version
				   + (_mesh ? _mesh.version : 0)
				   + (_material ? _material.version : 0);
		}
		
		/**
		 * The Transform3D object (position, rotation and scale) used
		 * to render the object.
		 *  
		 * @return 
		 * 
		 */
		public function get transform() : Transform3D			{ return _transform; }
		/**
		 * The IMesh3D object (geometry) used to render the object.
		 *  
		 * @return 
		 * 
		 */
		public function get mesh()		: IMesh				{ return _mesh; }
		/**
		 * The IMaterial3D object (texture data) used to render the
		 * object.
		 * 
		 * @return 
		 * 
		 */
		public function get material()	: ITexture			{ return _material; }
		/**
		 * Indicates whether the object is visible or not. Invisible
		 * objects are not traversed during scene rendering.
		 *  
		 * @return 
		 * 
		 */
		public function get visible()	: Boolean			{ return _visible; }
		/**
		 * The IEffect3D objects used to render the object during
		 * scene rendering.
		 *  
		 * @return 
		 * 
		 */
		public function get effects()	: Vector.<IEffect>	{ return _effects; }
		/**
		 * The Style3D object used to parametrize rendering.
		 * 
		 * @return 
		 * 
		 */
		public function get style()		: Style				{ return _style; }
		
		public function set mesh(value : IMesh) : void
		{
			_mesh = value;
		}
		
		public function set material(value : ITexture) : void
		{
			_material = value;
		}
		
		public function set visible(value : Boolean) : void
		{
			_visible = value;
		}
		
		public function Model(mesh 			: IMesh		= null,
							  material		: ITexture	= null)
		{
			super();
			
			_mesh = mesh;
			_material = material;
		}
		
		override protected function visitedByRenderingVisitor(query : RenderingVisitor) : void 
		{
			if (!_visible)
				return ;
			
			var transform 		: TransformManager 		= query.transform;
			var numEffects		: int					= _effects.length;
			var queryEffects	: Vector.<IEffect>		= query.effects;
			var numQueryEffects	: int					= queryEffects.length;
			
			// push world transform
			transform.world.push().multiply(_transform);
			
			/*transform.getLocalToScreen(_toScreen);
			_style.set(BasicStyle3D.WORLD_MATRIX, 			transform.world)
				  .set(BasicStyle3D.VIEW_MATRIX, 			transform.view)
				  .set(BasicStyle3D.PROJECTION_MATRIX, 		transform.projection)
				  .set(BasicStyle3D.LOCAL_TO_SCREEN_MATRIX, _toScreen);*/
			
			// push FXs and style
			for (var i : int = 0; i < numEffects; ++i)
				queryEffects.push(_effects[i]);
			query.style.push(_style);
			
			_material && query.visit(_material);
			_mesh && query.visit(_mesh);
			
			// pop FXs and style
			query.style.pop();
			query.effects.length = numQueryEffects;
			
			// pop world transform
			transform.world.pop();
		}
	}
}