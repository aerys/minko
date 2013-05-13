/**
 * Created with IntelliJ IDEA.
 * User: promethe
 * Date: 12/04/13
 * Time: 20:03
 * To change this template use File | Settings | File Templates.
 */
package aerys.minko.scene.controller.animation
{
    import aerys.minko.scene.controller.AbstractController;

    public class MasterAnimationController extends AbstractController implements IAnimationController
    {
        private var _animations : Vector.<IAnimationController>;
        private var _isPlaying  : Boolean;

        private var _labelNames : Vector.<String>;
        private var _labelTimes : Vector.<Number>;

        public function get numLabels() : uint
        {
            return _labelNames.length;
        }

        public function get isPlaying() : Boolean
        {
            return _isPlaying;
        }

        public function MasterAnimationController(animations : Vector.<IAnimationController>)
        {
            super();

            _animations = animations.concat();

            _labelNames = new <String>[];
            _labelTimes = new <Number>[];
        }

        public function addLabel(name : String, time : Number) : IAnimationController
        {
            if (_labelNames.indexOf(name) >= 0)
                throw new Error('A label with the same name already exists.');

            _labelNames.push(name);
            _labelTimes.push(time);

            var numAnimations : uint = _animations.length;

            for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
                _animations[animationId].addLabel(name,  time);

            return this;
        }

        public function removeLabel(name : String) : IAnimationController
        {
            var index : int = _labelNames.indexOf(name);

            if (index < 0)
                throw new Error('The time label named \'' + name + '\' does not exist.');

            var numLabels : uint = _labelNames.length - 1;

            _labelNames[index] = _labelNames[numLabels];
            _labelNames.length = numLabels;

            _labelTimes[index] = _labelTimes[numLabels];
            _labelTimes.length = numLabels;

            var numAnimations : uint = _animations.length;

            for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
                _animations[animationId].removeLabel(name);

            return this;
        }

        public function seek(time : Object) : IAnimationController
        {
            var numAnimations : uint = _animations.length;

            for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
                _animations[animationId].seek(time);

            return this;
        }

        public function play() : IAnimationController
        {
            var numAnimations : uint = _animations.length;

            _isPlaying = true;

            for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
                _animations[animationId].play();

            return this;
        }

        public function stop() : IAnimationController
        {
            var numAnimations : uint = _animations.length;

            _isPlaying = false;

            for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
                _animations[animationId].stop();

            return this;
        }

        public function setPlaybackWindow(beginTime : Object = null, endTime : Object = null) : IAnimationController
        {
            var numAnimations : uint = _animations.length;

            for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
                _animations[animationId].setPlaybackWindow(beginTime, endTime);

            return this;
        }

        public function resetPlaybackWindow() : IAnimationController
        {
            var numAnimations : uint = _animations.length;

            for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
                _animations[animationId].resetPlaybackWindow();

            return this;
        }

        public function getLabelName(index : uint) : String
        {
            return _labelNames[index];
        }

        public function getLabelTime(index : uint) : Number
        {
            return _labelTimes[index];
        }
		
		override public function clone():AbstractController
		{			
			var newController : MasterAnimationController = new MasterAnimationController(_animations);
			newController._labelNames = _labelNames;
			newController._labelTimes = _labelTimes;
			
			return newController;
		}
    }
}
