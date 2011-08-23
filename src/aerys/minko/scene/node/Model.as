package aerys.minko.scene.node
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectTarget;
	import aerys.minko.render.effect.Style;
	import aerys.minko.scene.action.ModelAction;
	import aerys.minko.scene.action.effect.PopEffectAction;
	import aerys.minko.scene.action.effect.PushEffectAction;
	import aerys.minko.scene.action.style.PopStyleAction;
	import aerys.minko.scene.action.style.PushStyleAction;
	import aerys.minko.scene.action.transform.PopTransformAction;
	import aerys.minko.scene.action.transform.PushTransformAction;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.scene.node.texture.ITexture;
	import aerys.minko.type.IVersionable;
	import aerys.minko.type.math.Transform3D;
	
	import avmplus.getQualifiedClassName;
	
	import flash.net.getClassByAlias;

	/**
	 * Model objects are visible scene objects. They contain references to:
	 * <ul>
	 * <li>an IMesh object (geometry)</li>
	 * <li>an ITexture object (texture data)</li>
	 * <li>a Style object (cascading rendering style parameters)</li>
	 * <li>and a Transform3D object (position, rotation and scale)</li>
	 * </ul>
	 * 
	 * <p>
	 * A Model object can be seen as a combination of a TransformGroup,
	 * a StyleGroup and an EffectGroup that can only contain two children:
	 * a texture and a mesh.
	 * </p>
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class Model extends AbstractScene implements ISearchableScene, ITransformableScene, IStylableScene, IVersionable, IEffectTarget
	{
		use namespace minko;
		
		private var _version		: uint				= 0;
		
		private var _mesh			: IMesh				= null;
		private var _texture		: ITexture			= null;
		
		private var _transform		: Transform3D		= new Transform3D();
		private var _visible		: Boolean			= true;
		
		private var _effect			: IEffect			= null;
		private var _style			: Style				= new Style();
		private var _styleEnabled	: Boolean			= true;
		
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
		public function get effect()	: IEffect	{ return _effect; }
		/**
		 * The Style3D object used to parametrize rendering.
		 * 
		 * @return 
		 * 
		 */
		public function get style()		: Style				{ return _style; }
		
		public function get styleEnabled() : Boolean		{ return _styleEnabled; }
		
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
		
		public function set effect(value : IEffect) : void
		{
			_effect = value;
		}
		
		public function set style(value : Style) : void
		{
			_style = value;
		}
		
		public function set styleEnabled(value : Boolean) : void
		{
			_styleEnabled = value;
		}
		
		public function Model(mesh 			: IMesh		= null,
							  texture		: ITexture	= null)
		{
			super();
			
			actions.push(
				PushStyleAction.pushStyleAction,
				new PushEffectAction(),
				PushTransformAction.pushTransformAction,
				ModelAction.modelAction,
				PopTransformAction.popTransformAction,
				new PopEffectAction(),
				PopStyleAction.popStyleAction
			);
			
			_mesh = mesh;
			_texture = texture;
		}
		
		public function getDescendantByName(name : String) : IScene
		{
			var result : IScene = null;
			
			if (_texture)
			{
				if (_texture.name == name)
					result = _texture;
				else if (_texture is ISearchableScene)
					result = (_texture as ISearchableScene).getDescendantByName(name);
			}
			
			if (!result && _mesh)
			{
				if (_mesh.name == name)
					result = _mesh;
				else if (_mesh is ISearchableScene)
					result = (_mesh as ISearchableScene).getDescendantByName(name);
			}
			
			return result;
		}
		
		public function getDescendantsByType(type : Class, descendants : Vector.<IScene> = null) : Vector.<IScene>
		{
			descendants ||= new Vector.<IScene>();
			
			if (_texture)
			{
				if (_texture is type)
					descendants.push(_texture);
				if (_texture is ISearchableScene)
					(_texture as ISearchableScene).getDescendantsByType(type, descendants);
			}
			
			if (_mesh)
			{
				if (_mesh is type)
					descendants.push(_mesh);
				if (_mesh is ISearchableScene)
					(_mesh as ISearchableScene).getDescendantsByType(type, descendants);
			}
			
			return descendants;
		}
	}
}