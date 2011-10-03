package aerys.minko.scene.node
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IRenderingEffect;
	import aerys.minko.render.effect.Style;
	import aerys.minko.scene.action.ModelAction;
	import aerys.minko.scene.action.effect.PopEffectAction;
	import aerys.minko.scene.action.effect.PushEffectAction;
	import aerys.minko.scene.action.style.PopStyleAction;
	import aerys.minko.scene.action.style.PushStyleAction;
	import aerys.minko.scene.action.transform.PopTransformAction;
	import aerys.minko.scene.action.transform.PushTransformAction;
	import aerys.minko.scene.node.group.Group;
	import aerys.minko.scene.node.group.IGroup;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.scene.node.texture.ITexture;
	import aerys.minko.type.IVersionable;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.events.Event;

	/**
	 * Model objects are visible scene objects. They contain references to:
	 * <ul>
	 * <li>an IMesh object (geometry)</li>
	 * <li>an ITexture object (texture data)</li>
	 * <li>a Style object (cascading rendering style parameters)</li>
	 * <li>and a Matrix3D object (position, rotation and scale)</li>
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
	public class Model extends AbstractScene implements ISearchableScene, ITransformableScene, IStylableScene, IEffectScene
	{
		use namespace minko;
		
		private var _version		: uint				= 0;
		
		private var _mesh			: IMesh				= null;
		private var _textures		: IGroup			= null;
		
		private var _transform		: Matrix4x4			= new Matrix4x4();
		private var _visible		: Boolean			= true;
		
		private var _effect			: IRenderingEffect	= null;
		private var _style			: Style				= new Style();
		private var _styleEnabled	: Boolean			= true;
		
		/**
		 * The Matrix3D object (position, rotation and scale) used
		 * to render the object.
		 *  
		 * @return 
		 * 
		 */
		public function get transform() : Matrix4x4	{ return _transform; }
		
		/**
		 * The IMesh3D object (geometry) used to render the object.
		 *  
		 * @return 
		 * 
		 */
		public function get mesh()		: IMesh	{ return _mesh; }
		
		/**
		 * The IMaterial3D object (texture data) used to render the
		 * object.
		 * 
		 * @return 
		 * 
		 */
		public function get textures()	: IGroup	{ return _textures; }
		
		/**
		 * Indicates whether the object is visible or not. Invisible
		 * objects are not traversed during scene rendering.
		 *  
		 * @return 
		 * 
		 */
		public function get visible()	: Boolean	{ return _visible; }
		
		/**
		 * The IEffect3D objects used to render the object during
		 * scene rendering.
		 *  
		 * @return 
		 * 
		 */
		public function get effect()	: IRenderingEffect	{ return _effect; }
		
		/**
		 * The Style3D object used to parametrize rendering.
		 * 
		 * @return 
		 * 
		 */
		public function get style()	: Style		{ return _style; }
		
		public function get styleEnabled() : Boolean		{ return _styleEnabled; }
		
		public function set mesh(value : IMesh) : void
		{
			if (!value)
				throw new Error("The 'value' argument cannot be null.");
			
			if (_mesh)
				_mesh.dispatchEvent(new Event(Event.ADDED));
			
			_mesh = value;
			_mesh.dispatchEvent(new Event(Event.REMOVED));
		}
		
		public function set textures(value : IGroup) : void
		{
			if (_textures)
				_textures.dispatchEvent(new Event(Event.REMOVED));
			
			_textures = value;
			
			if (_textures)
				_textures.dispatchEvent(new Event(Event.ADDED));
		}
		
		public function set visible(value : Boolean) : void
		{
			_visible = value;
		}
		
		public function set effect(value : IRenderingEffect) : void
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
		
		public function Model(mesh : IMesh	= null, ...textures)
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
			_textures = new Group(textures);
		}
		
		public function getDescendantByName(name : String) : IScene
		{
			var result : IScene = null;
			
			if (_textures)
			{
				if (_textures.name == name)
					result = _textures;
				else if (_textures is ISearchableScene)
					result = (_textures as ISearchableScene).getDescendantByName(name);
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
			
			if (_textures)
			{
				if (_textures is type)
					descendants.push(_textures);
				if (_textures is ISearchableScene)
					(_textures as ISearchableScene).getDescendantsByType(type, descendants);
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