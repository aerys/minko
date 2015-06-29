/*
Copyright (c) 2015 Aerys

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

#import <CoreMedia/CoreMedia.h>
#import "VideoSource.h"

@interface VideoSource () <AVCaptureVideoDataOutputSampleBufferDelegate>

@end

@implementation VideoSource

- (id)init
{
    self = [super init];
    if (self)
    {
        AVCaptureSession * captureSession = [[AVCaptureSession alloc] init];
        
        if ([captureSession canSetSessionPreset:AVCaptureSessionPreset640x480])
        {
            [captureSession setSessionPreset:AVCaptureSessionPreset640x480];
            NSLog(@"Capturing video at 640x480");
        }
        else
        {
            NSLog(@"Could not configure AVCaptureSession video input.");
        }
        
        _captureSession = captureSession;
    }
    
    return self;
}

- (void)dealloc {
    [super dealloc];
    [_captureSession stopRunning];
}

- (BOOL)initializeWithDevicePosition:(AVCaptureDevicePosition)devicePosition
{
    // Find camera device at the specific position
    AVCaptureDevice * videoDevice = [self cameraWithPosition:devicePosition];
    if ( !videoDevice )
    {
        NSLog(@"Could not initialize camera at position %d", (int)devicePosition);
        return FALSE;
    }
    
    // Obtain input port for camera device
    NSError * error;
    AVCaptureDeviceInput *videoInput = [AVCaptureDeviceInput deviceInputWithDevice:videoDevice error:&error];
    if (!error)
    {
        [self setDeviceInput:videoInput];
    }
    else
    {
        NSLog(@"Could not open input port for device %@ (%@)", videoDevice, [error localizedDescription]);
        return FALSE;
    }
    
    // Configure input port for captureSession
    if ([self.captureSession canAddInput:videoInput])
    {
        [self.captureSession addInput:videoInput];
    }
    else
    {
        NSLog(@"Could not add input port to capture session %@", self.captureSession);
        return FALSE;
    }
    
    // Configure output port for captureSession
    [self addVideoDataOutput];
    
    return TRUE;
}

- (AVCaptureDevice*)cameraWithPosition:(AVCaptureDevicePosition)position
{
    NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice *device in devices)
    {
        if ([device position] == position)
        {
            return device;
        }
    }
    
    return nil;
}

- (void) addVideoDataOutput
{
    // Instantiate a new video data output object
    AVCaptureVideoDataOutput * captureOutput = [[AVCaptureVideoDataOutput alloc] init];
    captureOutput.alwaysDiscardsLateVideoFrames = YES;
    
    // The sample buffer delegate requires a serial dispatch queue
    dispatch_queue_t queue;
    queue = dispatch_queue_create("com.minko.plugin.video-camera", DISPATCH_QUEUE_SERIAL);
    [captureOutput setSampleBufferDelegate:self queue:queue];
    dispatch_release(queue);
    
    // Define the pixel format for the video data output
    NSString * key = (NSString*)kCVPixelBufferPixelFormatTypeKey;
    NSNumber * value = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA];
    NSDictionary * settings = @{key:value};
    [captureOutput setVideoSettings:settings];
    
    // Configure the output port on the captureSession property
    [self.captureSession addOutput:captureOutput];
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput
       didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection
{
    // Convert CMSampleBufferRef to CVImageBufferRef
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    
    // Lock pixel buffer
    CVPixelBufferLockBaseAddress(imageBuffer, kCVPixelBufferLock_ReadOnly);
    
    // Construct VideoFrame struct
    uint8_t *baseAddress = (uint8_t*)CVPixelBufferGetBaseAddress(imageBuffer);
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    size_t stride = CVPixelBufferGetBytesPerRow(imageBuffer);
    
    // Unlock pixel buffer
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);

    self->frameData.assign(baseAddress, baseAddress + (stride * height));
    self->frameReceived.execute(self->frameData, (int)width, (int)height);
}

- (void)start
{
    [_captureSession startRunning];
}

- (void)stop
{
    [_captureSession stopRunning];
}

@end

// C++

using namespace minko::video;

minko::Signal<const std::vector<unsigned char>&, int, int>::Slot frameReceivedSlot;

VideoSourceImpl::VideoSourceImpl() :
    proxy([[VideoSource alloc] init]),
    frameReceived(minko::Signal<const std::vector<unsigned char>&, int, int>::create())
{
    frameReceivedSlot = proxy->frameReceived.connect(
    [this](const std::vector<unsigned char>& data, int width, int height)
    {
        this->frameReceived->execute(data, width, height);
    });
}

void
VideoSourceImpl::initialize()
{
    [proxy initializeWithDevicePosition:AVCaptureDevicePositionBack];
}

void
VideoSourceImpl::start()
{
    [proxy start];
}

void
VideoSourceImpl::stop()
{
    [proxy stop];
}