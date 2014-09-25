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

    private static final Size _defaultDesiredSize = new Size(600, 400);

    private static Camera _camera = null;

    private static Size _desiredSize = _defaultDesiredSize;
    private static boolean _desiredSizeChanged = false;

    private static Size _size = null;

    private static byte[][] _buffers = null;
    private static int _readBufferIndex = 0;
    private static int _writeBufferIndex = 0;
    private static boolean _frameChanged = false;

    private static int _srcFormat = 0;

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

        cameraParams.setPreviewFormat(_srcFormat);
        cameraParams.setPreviewFpsRange(30000, 30000);
        cameraParams.setPreviewFrameRate(30);
        cameraParams.setRecordingHint(true);
        cameraParams.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        cameraParams.setPreviewSize(size.x, size.y);

        _camera.setParameters(cameraParams);
    }

    private static void sizeChanged(Size size)
    {
        Log.i("minko", "VideoCamera.sizeChanged(" + size.x + ", " + size.y + ")");

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
        Log.i("minko", "VideoCamera.desiredSize");

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

            if (size.width <= desiredSize.x || size.height <= desiredSize.y)
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

        Log.i("minko", "VideoCamera.start");

        if (!openCamera())
        {
            Log.i("minko", "VideoCamera: failed to open camera");

            return;
        }

        int srcFormat = ImageFormat.NV21;
        _srcFormat = srcFormat;

        Size bestSize = _size == null ? bestSize(_desiredSize) : _size;

        Camera.Parameters cameraParams = _camera.getParameters();

        for (int format : cameraParams.getSupportedPreviewFormats())
        {
            Log.i("minko", "available format: " + format);
        }

        for (int fps : cameraParams.getSupportedPreviewFrameRates())
        {
            Log.i("minko", "available fps: " + fps);
        }

        for (int[] fpsRange : cameraParams.getSupportedPreviewFpsRange())
        {
            Log.i("minko", "available fps range: " + fpsRange[0] + ", " + fpsRange[1]);
        }

        startPreview(bestSize);
    }

    public static void stop()
    {
        releaseCamera();
    }

    private static void startPreview(Size size)
    {
        if (!isActive())
            return;

        Log.i("minko", "VideoCamera.startPreview");

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

        _camera.startPreview();
    }

    private static void stopPreview()
    {
        if (!isActive())
            return;

        Log.i("minko", "VideoCamera.stopPreview");

        _camera.stopPreview();
        _camera.setPreviewCallbackWithBuffer(null);
    }

    private static boolean openCamera()
    {
        Log.i("minko", "VideoCamera.openCamera");

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

    static class RgbPixel
    {
        RgbPixel(byte r, byte g, byte b)
        {
            _r = r;
            _g = g;
            _b = b;
        }

        byte _r;
        byte _g;
        byte _b;
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
