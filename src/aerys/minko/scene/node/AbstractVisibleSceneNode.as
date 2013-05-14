package aerys.minko.scene.node
{
    import aerys.minko.type.Signal;

    public class AbstractVisibleSceneNode extends AbstractSceneNode
    {
        private var _visible                    : Boolean;
        private var _computedVisibility         : Boolean;
        
        private var _visibilityChanged          : Signal;
        private var _computedVisibilityChanged  : Signal;
        
        public function get visible() : Boolean
        {
            return _visible;
        }
        public function set visible(value : Boolean) : void
        {
            if (value != _visible)
            {
                _visible = value;
                _visibilityChanged.execute(this, value);
                updateComputedVisibility();
            }
        }
        
        public function get computedVisibility() : Boolean
        {
            return _computedVisibility;
        }
        
        public function get visibilityChanged() : Signal
        {
            return _visibilityChanged;
        }
        
        public function get computedVisibilityChanged() : Signal
        {
            return _computedVisibilityChanged;
        }
        
        public function AbstractVisibleSceneNode()
        {
            super();
        }
        
        override protected function initialize() : void
        {
			super.initialize();
			
            _visible = true;
            _computedVisibility = true;
        }
		
		override protected function initializeSignals() : void
		{
			super.initializeSignals();
			
            _visibilityChanged = new Signal('AbstractVisibleSceneNode.visibilityChanged');
            _computedVisibilityChanged = new Signal('AbstractVisibleSceneNode.computedVisibilityChanged');
		}
		
		override protected function initializeSignalHandlers() : void
		{
			super.initializeSignalHandlers();
			
			added.add(addedHandler);
			removed.add(removedHandler);	
		}
        
		private function addedHandler(child : ISceneNode, ancestor : Group) : void
        {
            // if ancestor == parent then the node was just added as a direct child of ancestor
            if (ancestor == parent)
            {
                parent.computedVisibilityChanged.add(parentComputedVisibilityChangedHandler);
                updateComputedVisibility();
            }
        }
        
        private function removedHandler(child : ISceneNode, ancestor : Group) : void
        {
            // if parent is not set anymore, ancestor is not the direct parent of child anymore
            if (!parent)
            {
               	ancestor.computedVisibilityChanged.remove(parentComputedVisibilityChangedHandler);
                updateComputedVisibility();
            }
        }
        
        private function updateComputedVisibility() : void
        {
            var newComputedVisibility : Boolean = _visible
                && (!parent || parent.computedVisibility);
            
            if (newComputedVisibility != computedVisibility)
            {
                _computedVisibility = newComputedVisibility;
                _computedVisibilityChanged.execute(this, newComputedVisibility);
            }
        }
        
        protected function parentComputedVisibilityChangedHandler(parent        : Group,
                                                                  visibility    : Boolean) : void
        {
            updateComputedVisibility();
        }
    }
}