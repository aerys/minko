package aerys.minko.scene.node
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectTarget;
	import aerys.minko.scene.action.EffectAction;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.action.IActionTarget;
	import aerys.minko.scene.action.ModelAction;
	import aerys.minko.scene.action.StyleAction;
	import aerys.minko.scene.action.TransformAction;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.scene.node.texture.ITexture;
	import aerys.minko.scene.visitor.data.Style;
	import aerys.minko.type.IVersionnable;
	import aerys.minko.type.math.Transform3D;

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
	public class Model extends AbstractScene implements IScene, ITransformable, IStyled, IVersionnable, IEffectTarget, IActionTarget
	{
		use namespace minko;
		
		private var _version	: uint				= 0;
		
		private var _mesh		: IMesh				= null;
		private var _texture	: ITexture			= null;
		
		private var _transform	: Transform3D		= new Transform3D();
		private var _visible	: Boolean			= true;
		
		private var _effects	: Vector.<IEffect>	= new Vector.<IEffect>();
		private var _style		: Style				= new Style();
		
		public function get version() : uint
		{
			return _version
				   + _transform.version
				   + _style.version
				   + (_mesh ? _mesh.version : 0)
				   + (_texture ? _texture.version : 0);
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
		public function get texture()	: ITexture			{ return _texture; }
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
		
		public function set texture(value : ITexture) : void
		{
			_texture = value;
		}
		
		public function set visible(value : Boolean) : void
		{
			_visible = value;
		}
		
		public function Model(mesh 			: IMesh		= null,
							  texture		: ITexture	= null)
		{
			super();
			
			actions.push(StyleAction.styleAction,
						 new EffectAction(),
						 TransformAction.transformAction,
						 ModelAction.modelAction);
			
			_mesh = mesh;
			_texture = texture;
		}
	}
}