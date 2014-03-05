//
//  OpenGLView.m
//  minko-example-iphone-cube
//
//  Created by Kevin Gomes on 9/2/13.
//
//

#import "OpenGLView.h"

@implementation OpenGLView

@synthesize animating;
@synthesize context;
@dynamic animationFrameInterval;

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

-(id)initWithCoder:(NSCoder *)coder
{
    if ((self = [super initWithCoder:coder]))
	{
        // Get the layer. By default, CALayers are set to non-opaque (i.e. transparent). However, this is bad for performance reasons (especially with OpenGL), so it’s best to set this as opaque when possible.
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
    
    // Creating context for OpenGLES 2.0 rendering
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!context)
    {
        NSLog(@"Error while creating EAGLContext.");
        NSLog(@"Reminder: You must use iOS 3.1 or later with OpenGLES2");
        [self release];
    }
    
    animating = FALSE;
    displayLinkSupported = FALSE;
    animationFrameInterval = 1;
    displayLink = nil;
    animationTimer = nil;
    
    // A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
    // class is used as fallback when it isn't available.
    NSString *reqSysVer = @"3.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
        displayLinkSupported = TRUE;
    }
        return self;
}

- (void) drawView:(id)sender
{
    [self render];
}

-(void)render
{
    NSLog(@"Frame refreshed");
    
    [EAGLContext setCurrentContext:context];
    // Render Loop
    mesh->component<Transform>()->transform()->prependRotationY(.01f);
    sceneManager->nextFrame();
    
    //[context presentRenderbuffer:GL_RENDERBUFFER];
}

- (NSInteger) animationFrameInterval
{
	return animationFrameInterval;
}

- (void) setAnimationFrameInterval:(NSInteger)frameInterval
{
	// Frame interval defines how many display frames must pass between each time the
	// display link fires. The display link will only fire 30 times a second when the
	// frame internal is two on a display that refreshes 60 times a second. The default
	// frame interval setting of one will fire 60 times a second when the display refreshes
	// at 60 times a second. A frame interval setting of less than one results in undefined
	// behavior.
	if (frameInterval >= 1)
	{
		animationFrameInterval = frameInterval;
		
		if (animating)
		{
			[self stopAnimation];
			[self startAnimation];
		}
	}
}

- (void) startAnimation
{
	if (!animating)
	{
		if (displayLinkSupported)
		{
			// CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will
            // result in a warning, but can be dismissed if the system version runtime check for
            // CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
            // not be called in system versions earlier than 3.1.
			
			displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
			[displayLink setFrameInterval:animationFrameInterval];
			[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		}
		else
			animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawView:) userInfo:nil repeats:TRUE];
		
		animating = TRUE;
	}
}

- (void)stopAnimation
{
	if (animating)
	{
		if (displayLinkSupported)
		{
			[displayLink invalidate];
			displayLink = nil;
		}
		else
		{
			[animationTimer invalidate];
			animationTimer = nil;
		}
		animating = FALSE;
	}
}

-(void)initMinko
{
    // Binding context to current view.
    [EAGLContext setCurrentContext:context];
    
    // Here, put all the Minko code you want. Layer is set to opaque, and context is created and binded.
    
    sceneManager = SceneManager::create(render::OpenGLES2Context::create());
    mesh = scene::Node::create("mesh");
    
	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
    ->registerParser<file::PNGParser>("png")
    ->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
    ->queue("texture/box.png")
    ->queue("effect/Basic.effect");
    
#ifdef DEBUG
    sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#else
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/release");
#endif
    
    auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
    {
         auto root   = scene::Node::create("root");
         auto camera	= scene::Node::create("camera");
         
         root->addComponent(sceneManager);
         
         // setup camera
         auto renderer = Renderer::create();
         renderer->backgroundColor(0x7F7F7FFF);
         camera->addComponent(renderer);
         camera->addComponent(Transform::create());
         camera->component<Transform>()->transform()
         ->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f));
         camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));
         root->addChild(camera);
         
         // setup mesh
         mesh->addComponent(Transform::create());
         mesh->addComponent(Surface::create(
                                            assets->geometry("cube"),
                                            data::Provider::create()
                                            ->set("material.diffuseColor",	Vector4::create(0.f, 0.f, 1.f, 1.f))
                                            ->set("material.diffuseMap",	assets->texture("texture/box.png")),
                                            assets->effect("effect/Basic.effect")
                                            ));
         root->addChild(mesh);
     });
    
	sceneManager->assets()->load();
}

@end