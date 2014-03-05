//
//  OpenGLView.h
//  minko-example-iphone-cube
//
//  Created by Kevin Gomes on 9/2/13.
//
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <time.h>
#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

@interface OpenGLView : UIView
{
    BOOL animating;
	BOOL displayLinkSupported;
	NSInteger animationFrameInterval;
	// Use of the CADisplayLink class is the preferred method for controlling your animation timing.
	// CADisplayLink will link to the main display and fire every vsync when added to a given run-loop.
	// The NSTimer class is used only as fallback when running on a pre 3.1 device where CADisplayLink
	// isn't available.
	id displayLink;
    NSTimer *animationTimer;
    SceneManager::Ptr sceneManager;
    scene::Node::Ptr mesh;
}

@property (nonatomic, strong)EAGLContext *context;
@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;


- (void) initMinko;
- (void) startAnimation;
- (void) stopAnimation;
- (void) drawView:(id)sender;

@end
