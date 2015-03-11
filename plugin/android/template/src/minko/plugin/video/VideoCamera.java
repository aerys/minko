package minko.plugin.video;

import java.io.IOException;
import java.util.List;

import org.libsdl.app.*;
import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.webkit.WebView;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.os.Bundle;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;

import android.graphics.Point;
import android.view.WindowManager;
import android.view.Display;

import android.util.Log;
import android.util.AttributeSet;

import android.content.res.Configuration;
import android.view.ViewGroup;

import android.graphics.ImageFormat;

public class VideoCamera
{
    private static final class Size
    {
        public Size(int x, int y)
        {
            this.x = x;
            this.y = y;
        }

        final int x;
        final int y;
    }

    private static abstract class DummyCameraPreviewCallback
    {
        public DummyCameraPreviewCallback()
        {

        }

        public abstract void run(SurfaceHolder surfaceHolder, int format, int width, int height);
    }

    private static final class DummyCameraPreview
        extends SurfaceView
        implements SurfaceHolder.Callback
    {
        private boolean _isActive = false;

        private final DummyCameraPreviewCallback _callback;

        public DummyCameraPreview(Context context, Camera camera, DummyCameraPreviewCallback callback)
        {
            super(context);

            Log.d("minko", "super(context)");

            _callback = callback;

            // tmp
            Log.d("minko", "before settings preview display");

            getHolder().addCallback(this);

            setScaleX(1.0f);
            setScaleY(1.0f);
        }

        public final boolean isActive()
        {
            return _isActive;
        }

        public final void activate(boolean isActive)
        {
            _isActive = isActive;
        }

        @Override
        public void surfaceCreated(SurfaceHolder surfaceHolder)
        {
            Log.d("minko", "DummyCameraPreview.surfaceCreated");

            if (isActive())
                _callback.run(surfaceHolder, 0, 0, 0);
        }

        @Override
        public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i2, int i3)
        {
            Log.d("minko", "DummyCameraPreview.surfaceChanged");

            if (isActive())
                _callback.run(surfaceHolder, i, i2, i3);
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder surfaceHolder)
        {
        }
    }

    private static final Size _defaultDesiredSize = new Size(600, 600);

    private static Camera _camera = null;

    private static Size _desiredSize = _defaultDesiredSize;
    private static boolean _desiredSizeChanged = false;

    private static Size _size = null;

    private static byte[][] _buffers = null;
    private static int _readBufferIndex = 0;
    private static int _writeBufferIndex = 0;
    private static boolean _frameChanged = false;

    private static int _srcFormat = 0;

    private static DummyCameraPreview _dummyCameraPreview = null;

    public static byte[] retrieveFrame(int[] metaData)
    {
        if (!_frameChanged)
            return null;

        _frameChanged = false;

        Size size = _size;

        metaData[0] = size.x;
        metaData[1] = size.y;
        metaData[2] = _srcFormat;

        return readBuffer();
    }

    private static boolean isActive()
    {
        return _camera != null;
    }

    private static void resetParameters(Size size)
    {
        Camera.Parameters cameraParams = _camera.getParameters();

        int maxPreviewFrameRate = 0;
        int bestPreviewFrameRate = 0;

        int maxPreviewFpsRangeUpperBound = 0;
        int[] bestPreviewFpsRange = null;

        for (int format : cameraParams.getSupportedPreviewFormats())
        {
            Log.d("minko", "available format: " + format);
        }

        for (int fps : cameraParams.getSupportedPreviewFrameRates())
        {
            Log.d("minko", "available fps: " + fps);

            if (fps > maxPreviewFrameRate)
                maxPreviewFrameRate = fps;
        }

        for (int[] fpsRange : cameraParams.getSupportedPreviewFpsRange())
        {
            Log.d("minko", "available fps range: " + fpsRange[0] + ", " + fpsRange[1]);

            if (fpsRange[1] > maxPreviewFpsRangeUpperBound)
            {
                maxPreviewFpsRangeUpperBound = fpsRange[1];

                bestPreviewFpsRange = fpsRange;
            }
        }

        //for (Camera.Area focusArea : cameraParams.getFocusAreas())
        //{
        //    Log.d("minko", "focus area: " + focusArea.rect.left + ", " + focusArea.rect.top + "//, " + focusArea.rect.right + ", " + focusArea.rect.bottom);
        //}

        bestPreviewFrameRate = maxPreviewFrameRate;

        cameraParams.setPreviewFormat(_srcFormat);
        cameraParams.setPreviewFpsRange(bestPreviewFpsRange[0], bestPreviewFpsRange[1]);
        cameraParams.setPreviewFrameRate(bestPreviewFrameRate);
        cameraParams.setRecordingHint(true);
        cameraParams.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        cameraParams.setPreviewSize(size.x, size.y);
        cameraParams.setZoom(0);

        _camera.setParameters(cameraParams);
    }

    private static void sizeChanged(Size size)
    {
        Log.d("minko", "VideoCamera.sizeChanged(" + size.x + ", " + size.y + ")");

        _size = size;

        if (!isActive())
            return;

        _readBufferIndex = 0;
        _writeBufferIndex = 1;

        int bufferSize = (int) ((size.x * size.y * ImageFormat.getBitsPerPixel(_srcFormat)) / 8.0f);

        if (_buffers == null)
            _buffers = new byte[2][];

        _buffers[0] = new byte[bufferSize];
        _buffers[1] = new byte[bufferSize];

        resetParameters(size);
    }

    public static void desiredSize(int width, int height)
    {
        Log.d("minko", "VideoCamera.desiredSize");

        Size newDesiredSize = new Size(width, height);

        if (_desiredSize.x == newDesiredSize.x &&
            _desiredSize.y == newDesiredSize.y)
            return;

        _desiredSizeChanged = true;

        _desiredSize = newDesiredSize;

        if (!isActive())
            return;

        Size bestSize = bestSize(_desiredSize);

        stopPreview();

        startPreview(bestSize);
    }

    private static Size bestSize(Size desiredSize)
    {
        final Camera.Parameters cameraParams = _camera.getParameters();

        final List<Camera.Size> availableSizes = cameraParams.getSupportedPreviewSizes();

        if (availableSizes.isEmpty())
            return new Size(1, 1);

        Camera.Size bestSize = availableSizes.get(0);

        for (Camera.Size size : availableSizes)
        {
            // TODO
            // take orientation into account

            Log.d("minko", "available size: " + size.width + "x" + size.height);

            if (size.width <= desiredSize.x && size.height <= desiredSize.y)
            {
                bestSize = size;

                break;
            }
        }

        return new Size(bestSize.width, bestSize.height);
    }

    public static void start()
    {
        if (isActive())
            return;

        _frameChanged = false;
        _desiredSizeChanged = false;

        Log.d("minko", "VideoCamera.start");

        if (!openCamera())
        {
            Log.d("minko", "VideoCamera: failed to open camera");

            return;
        }
        else
        {
            Log.d("minko", "VideoCamera.start, camera was successfully opened");

            _camera.setErrorCallback(new Camera.ErrorCallback()
            {
                @Override
                public final void onError(int error, Camera camera)
                {
                    switch (error)
                    {
                    case Camera.CAMERA_ERROR_UNKNOWN:
                        Log.d("minko", "Camera: unknown error");
                        break;

                    case Camera.CAMERA_ERROR_SERVER_DIED:
                        Log.d("minko", "Camera: server died");
                        break;

                    default:
                        break;
                    }
                }
            });
        }

        int srcFormat = ImageFormat.NV21;
        _srcFormat = srcFormat;

        final Size bestSize = _size == null ? bestSize(_desiredSize) : _size;

        if (_dummyCameraPreview == null)
        {
            // work around to make preview callback working on all devices (Shield...)

            getContext().runOnUiThread(new Runnable()
            {
                @Override
                public final void run()
                {
                    _dummyCameraPreview = new DummyCameraPreview(getContext(), _camera, new         DummyCameraPreviewCallback()
                    {
                        @Override
                        public final void run(SurfaceHolder surfaceHolder, int format, int width, int height)
                        {
                            Log.d("minko", "surface changed: " + width + "x" + height);

                            try
                            {
                                _camera.setPreviewDisplay(_dummyCameraPreview.getHolder());
                            }
                            catch (Exception exception)
                            {
                                Log.d("minko", exception.getMessage());
                            }

                            // Size actualBestSize = bestSize(new Size(width, height));
                
                            startPreview(bestSize);
                        }
                    });
        
                    _dummyCameraPreview.activate(true);

                    ViewGroup.LayoutParams defaultLayoutParams = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.FILL_PARENT);
        
                    getContext().addContentView(_dummyCameraPreview, defaultLayoutParams);
                }
            });
        }

        //startPreview(bestSize);
    }

    public static void stop()
    {
        if (_dummyCameraPreview != null)
        {
            _dummyCameraPreview.activate(false);

            ((ViewGroup) _dummyCameraPreview.getParent()).removeView(_dummyCameraPreview);

            _dummyCameraPreview = null;
        }

        releaseCamera();
    }

    private static SDLActivity getContext()
    {
        return (SDLActivity) SDLActivity.getContext();
    }

    private static void startPreview(Size size)
    {
        if (!isActive())
            return;

        stopPreview();

        Log.d("minko", "VideoCamera.startPreview");

        sizeChanged(size);

        _camera.setPreviewCallbackWithBuffer(new PreviewCallback()
        {
            @Override
            public void onPreviewFrame(byte[] data, Camera camera)
            {
                VideoCamera.processFrame(data, _srcFormat);
            }
        });

        _camera.addCallbackBuffer(writeBuffer());

        try
        {
            _camera.startPreview();
        }
        catch (Exception exception)
        {
            Log.e("minko", "VideoCamera.startPreview, failed starting camera preview");
        }
    }

    private static void stopPreview()
    {
        if (!isActive())
            return;

        Log.d("minko", "VideoCamera.stopPreview");

        _camera.stopPreview();
        _camera.setPreviewCallbackWithBuffer(null);
    }

    private static boolean openCamera()
    {
        Log.d("minko", "VideoCamera.openCamera");

        releaseCamera();

        Camera.CameraInfo info = new Camera.CameraInfo();

        for (int i = 0; i < Camera.getNumberOfCameras(); ++i) {

            Camera.getCameraInfo(i, info);

            if (info.facing == Camera.CameraInfo.CAMERA_FACING_BACK)
            {
                try
                {
                    _camera = Camera.open(i);

                    if (_camera != null)
                        break;
                }
                catch (Exception e)
                {
                    Log.e("minko", "Fail camera opening try: " + e.getLocalizedMessage());
                }
            }
        }

        return _camera != null;
    }

    private static void releaseCamera()
    {
        if (!isActive())
            return;

        stopPreview();

        _camera.release();
        _camera = null;
    }

    private static long currentNanoTime;
    private static void processFrame(byte[] buffer, int srcFormat)
    {
        long nanoDt = System.nanoTime() - currentNanoTime;
        currentNanoTime = System.nanoTime();
        float dt = nanoDt / 1e9f;

        _frameChanged = true;
 
        swapBuffers();

        _camera.addCallbackBuffer(writeBuffer());
    }

    private static byte[] readBuffer()
    {
        return _buffers[_readBufferIndex];
    }

    private static byte[] writeBuffer()
    {
        return _buffers[_writeBufferIndex];
    }

    private static void swapBuffers()
    {
        int readBufferIndex = _readBufferIndex;
        _readBufferIndex = _writeBufferIndex;
        _writeBufferIndex = readBufferIndex;
    }
}
