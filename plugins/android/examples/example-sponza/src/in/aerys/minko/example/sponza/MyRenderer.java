package in.aerys.minko.example.sponza;


import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.util.Log;

public class MyRenderer implements GLSurfaceView.Renderer {

	private LibraryClass lib;
	
	public MyRenderer()
	{
		lib = new LibraryClass();
	}
	
	@Override
	public void onDrawFrame(GL10 arg0) {
		lib.DrawFrame();
	}

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		lib.initialize();
	}


	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
		
		gl.glViewport(0, 0, width-10, height-10);
	}

}
