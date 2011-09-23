package aerys.minko.scene.node.group
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IRenderingEffect;
	import aerys.minko.scene.action.effect.PopEffectAction;
	import aerys.minko.scene.action.effect.PushEffectAction;
	import aerys.minko.scene.action.group.MaterialGroupAction;
	import aerys.minko.scene.node.IEffectScene;
	
	import flash.events.Event;

	public class MaterialGroup extends StyleGroup implements IEffectScene
	{
		private var _effect		: IRenderingEffect	= null;
		private var _textures	: IGroup			= new Group();
		
		public function get effect() 	: IRenderingEffect	{ return _effect; }
		public function get textures()	: IGroup			{ return _textures; }
		
		public function set effect(value : IRenderingEffect) : void
		{
			_effect = value;
		}
		
		public function set textures(value : IGroup) : void
		{
			if (!value)
				throw new Error("The 'value' argument cannot be null.");
			
			if (_textures)
				_textures.dispatchEvent(new Event(Event.REMOVED));
			
			_textures = value;
			_textures.dispatchEvent(new Event(Event.ADDED));
		}
		
		public function MaterialGroup(effect : IRenderingEffect, ...children)
		{
			super(children);
			
			_effect = effect;
			
			actions.unshift(MaterialGroupAction.materialGroupAction,
							new PushEffectAction());
			actions.push(new PopEffectAction());
		}
	}
}