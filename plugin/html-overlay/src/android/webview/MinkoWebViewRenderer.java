package minko.plugin.htmloverlay;

import android.content.Context;
import android.view.Surface;
import android.graphics.SurfaceTexture;
import android.graphics.Canvas;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.util.Log;
import android.view.ViewGroup;
import android.view.TextureView;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MinkoWebViewRenderer implements GLSurfaceView.Renderer
{
    private static final String TAG = "Minko/Java";

    private Surface _webViewSurface;
    private Canvas _surfaceCanvas;

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        final String extensions = GLES20.glGetString(GLES20.GL_EXTENSIONS);
        Log.d(TAG, "[MinkoWebViewRenderer] OpenGL ES2 extensions: " + extensions);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
        Log.i(TAG, "[MinkoWebViewRenderer] onSurfaceChanged");

        _webViewSurface = null;
    }

    @Override
    public void onDrawFrame(GL10 gl)
    {
        // NOTHING
    }

    public void setWebViewSurface(Surface webViewSurface)
    {
        _webViewSurface = webViewSurface;
    }

    public Canvas onDrawViewBegin()
    {
        _surfaceCanvas = null;

        if (_webViewSurface != null) 
        {
            try
            {
                _surfaceCanvas = _webViewSurface.lockCanvas(null);
            }
            catch(Exception e)
            {
                Log.e(TAG, "[MinkoWebViewRenderer] Error while rendering view to GL: " + e);
            }
        }

        return _surfaceCanvas;
    }

    public void onDrawViewEnd()
    {
        if(_surfaceCanvas != null)
            _webViewSurface.unlockCanvasAndPost(_surfaceCanvas);

        _surfaceCanvas = null;
    }
}