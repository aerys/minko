package aerys.minko.scene.node.group
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IRenderingEffect;
	import aerys.minko.scene.action.effect.PopEffectAction;
	import aerys.minko.scene.action.effect.PushEffectAction;
	import aerys.minko.scene.action.group.MaterialGroupAction;
	import aerys.minko.scene.node.IEffectScene;

	import flash.events.Event;

	public dynamic class MaterialGroup extends StyleGroup implements IEffectScene
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

		public function MaterialGroup(effect : IRenderingEffect, ...textures)
		{
			super();

			initialize(effect, textures);
		}

		private function initialize(effect : IRenderingEffect, textures : Array) : void
		{
			_effect = effect;
			_textures = new Group(textures);

			actions.unshift(new PushEffectAction(),
							MaterialGroupAction.materialGroupAction);
			actions.push(new PopEffectAction());
		}
	}
}