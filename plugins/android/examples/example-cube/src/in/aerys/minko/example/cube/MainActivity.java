package in.aerys.minko.example.cube;

import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.view.Menu;

public class MainActivity extends Activity {
	
	public static AssetManager Assets;
	
	private class MyGLSurfaceView extends GLSurfaceView
	{
		public MyGLSurfaceView(Context context){
			super(context);

			setDebugFlags(DEBUG_CHECK_GL_ERROR | DEBUG_LOG_GL_CALLS);
			setEGLContextClientVersion(2);
			
			super.setEGLConfigChooser(8 , 8, 8, 8, 16, 0);
			
			setRenderer(new MyRenderer());
			setRenderMode(RENDERMODE_CONTINUOUSLY);
		}
	}

	private GLSurfaceView mGLView;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        Assets = getAssets();
        
        mGLView = new MyGLSurfaceView(this);
        setContentView(mGLView);
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        return true;
    }
    
}
