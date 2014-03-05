//
//  MinkoDelegate.h
//  minko-example-iphone-cube
//
//  Created by Kevin Gomes on 9/3/13.
//
//

#import <UIKit/UIKit.h>
#import "EAGLView.h"

@interface MinkoDelegate : UIResponder <UIApplicationDelegate>
{
    EAGLView *openglView;
    UIWindow *window;
}

@property (strong, nonatomic) IBOutlet UIWindow *window;
@property (nonatomic, strong) IBOutlet EAGLView *openglView;
@end
