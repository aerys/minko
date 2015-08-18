/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "minko/Common.hpp"
#include "minko/Signal.hpp"

#include "minko/component/AbstractComponent.hpp"
#include "minko/component/AbstractRebindableComponent.hpp"

namespace minko
{
	namespace component
	{
		class AbstractAnimation :
			public AbstractRebindableComponent
		{
		public:
			typedef std::shared_ptr<AbstractAnimation>		Ptr;

		private:
			typedef std::shared_ptr<scene::Node>			NodePtr;
			typedef std::shared_ptr<AbstractComponent>		AbsCmpPtr;
			
			struct Label
			{
				std::string name;
				uint		time; // label time in milliseconds

				inline
				Label(const std::string n, uint t):
					name(n), time(t)
				{
				}
			};

		protected:
			uint		_maxTime;
			uint		_currentTime;	// relative to animation 
			Signal<AbsCmpPtr, NodePtr>::Slot				            _targetAddedSlot;
			Signal<AbsCmpPtr, NodePtr>::Slot				            _targetRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot			            _addedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot			            _removedSlot;

		private:
			uint		_loopMinTime;
			uint		_loopMaxTime;
			uint		_loopTimeRange;
			uint		_previousTime;	// relative to animation
			uint		_previousGlobalTime;

			bool		_isPlaying;
			bool		_isLooping;
			bool		_isReversed;
			bool		_mustUpdateOnce;


			clock_t		_clockStart;

			std::function<uint(uint)>						            _timeFunction;

			std::vector<Label>								            _labels;
			std::unordered_map<std::string, uint>			            _labelNameToIndex;
			std::vector<uint>								            _nextLabelIds;

			std::shared_ptr<SceneManager>					            _sceneManager;

			std::shared_ptr<Signal<Ptr>>					            _started;
			std::shared_ptr<Signal<Ptr>>					            _looped;
			std::shared_ptr<Signal<Ptr>>					            _stopped;
			std::shared_ptr<Signal<Ptr, std::string, uint>>	            _labelHit;

			Signal<std::shared_ptr<SceneManager>, float, float>::Slot	_frameBeginSlot;

			

		public:
			NodePtr														_target;

			virtual
			Ptr
			play();

			virtual
			Ptr
			stop();

			virtual
			AbstractComponent::Ptr
			clone(const CloneOption& option) = 0;

            virtual
			Ptr
			seek(uint time);

            virtual
			Ptr
			seek(const std::string&);

			inline
			uint
			currentTime() const
			{
				return _currentTime;
			}

			inline
			uint
			loopStartTime() const
			{
				return !_isReversed ? _loopMinTime : _loopMaxTime;
			}

			inline
			uint
			loopEndTime() const
			{
				return !_isReversed ? _loopMaxTime : _loopMinTime;
			}

			bool
			hasLabel(const std::string& name) const;

			virtual
			Ptr
			addLabel(const std::string& name, uint time);

			virtual
			Ptr
			changeLabel(const std::string& name, const std::string& newName);

			virtual
			Ptr
			setTimeForLabel(const std::string& name, uint newTime);

			virtual
			Ptr
			removeLabel(const std::string& name);

			virtual
			Ptr
			setPlaybackWindow(uint, uint, bool forceRestart = false);

			virtual
			Ptr
			setPlaybackWindow(const std::string&, const std::string&, bool forceRestart = false);

			virtual
			Ptr
			resetPlaybackWindow();

			uint
			numLabels() const
			{
				return _labels.size();
			}

			const std::string&
			labelName(uint labelId) const
			{
				return _labels[labelId].name;
			}

			uint
			labelTime(uint labelId) const
			{
				return _labels[labelId].time;
			}

			uint
			labelTime(const std::string& name) const;

			inline
			bool
			isPlaying() const
			{
				return _isPlaying;
			}

			inline
			void
			isPlaying(bool value)
			{
				_isPlaying = value;
			}

			inline
			bool
			isLooping() const
			{
				return _isLooping;
			}

			inline
			void
			isLooping(bool value)
			{
				_isLooping = value;
			}

			inline
			bool
			isReversed() const
			{
				return _isReversed;
			}

            virtual
            inline
			void
            isReversed(bool value)
            {
                _isReversed = value;
            }

			inline
			uint
			maxTime() const
			{
				return _maxTime;
			}

            virtual
			inline
			void
			timeFunction(const std::function<uint(uint)>& func)
			{
                _timeFunction = func;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			started() const
			{
				return _started;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			looped() const
			{
				return _looped;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			stopped() const
			{
				return _stopped;
			}

			inline
			std::shared_ptr<Signal<Ptr, std::string, uint>>
			labelHit() const
			{
				return _labelHit;
			}			

		protected:
			AbstractAnimation(bool isLooping);

			AbstractAnimation(const AbstractAnimation& absAnimation, const CloneOption& option);

			virtual
			inline
			~AbstractAnimation()
			{
				_targetAddedSlot	= nullptr;
				_targetRemovedSlot	= nullptr;
				_addedSlot			= nullptr;
				_removedSlot		= nullptr;
				_frameBeginSlot		= nullptr;
			}

			virtual
			void
			initialize();

			void
			targetAdded(NodePtr node);

			void
			targetRemoved(NodePtr node);

			virtual
            void
            addedHandler(NodePtr node, NodePtr target, NodePtr parent);

			virtual
			void
            removedHandler(NodePtr node, NodePtr target, NodePtr parent);

			void
			componentAddedHandler(NodePtr node, NodePtr	target, AbsCmpPtr component);

			void
			componentRemovedHandler(NodePtr	node, NodePtr target, AbsCmpPtr	component);

			void
			findSceneManager();

			void
			setSceneManager(std::shared_ptr<SceneManager>);

			virtual
			void
			frameBeginHandler(std::shared_ptr<SceneManager>, float, float);

			// record the indices of the labels that lie directly after the specified time value
			// in the animation.
			virtual
			void
			updateNextLabelIds(uint time);

			virtual
			void 
			checkLabelHit(uint previousTime, uint newTime);

			bool
			isInPlaybackWindow(uint) const;

			virtual 
			bool
			update(uint rawGlobalTime); // absolute, untransformed animation time (in milliseconds)

			virtual
			void
			update() = 0;

			uint
			getTimerMilliseconds() const;

			uint
			getNewLoopTime(uint time, int deltaTime) const;
		};
	}
}
