package minko.plugin.htmloverlay;

import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.content.Context;
import android.util.Log;
import android.app.Activity;
import android.content.ContextWrapper;
import android.view.*;
import org.libsdl.app.*;

public class MinkoWebView extends WebView
{
	private long lastEventTime = -1;

	public MinkoWebView(Context context)
	{
		super(context);
	}

    @Override
    public boolean onTouchEvent(MotionEvent ev)
    {
        long eventTime = ev.getEventTime();
        int action = ev.getAction();

        switch (action)
        {
			case MotionEvent.ACTION_MOVE:
			{
				return super.onTouchEvent(ev);
			}
			case MotionEvent.ACTION_DOWN:
			{
				return super.onTouchEvent(ev);
			}
			case MotionEvent.ACTION_UP:
			{
				/*if (System.currentTimeMillis() - lastEventTime < 1000)
				{
					// Cancel all touch actions
					ev.setAction(MotionEvent.ACTION_CANCEL);
					super.onTouchEvent(ev);
					// Restart the current action
					ev.setAction(MotionEvent.ACTION_DOWN);
					super.onTouchEvent(ev);
				}

				ev.setAction(MotionEvent.ACTION_UP);
				lastEventTime = System.currentTimeMillis();*/

				return super.onTouchEvent(ev);
			}
		}

        return super.onTouchEvent(ev);
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event)
    {
        // Dispatch the key events on the SDL canvas (including joystick buttons)
        SurfaceView surfaceView = (SurfaceView)SDLActivity.getLayout().getChildAt(0);

        if (event.getKeyCode() == KeyEvent.KEYCODE_BACK)
        {
            // FIXME Find out why 'surfaceView.dispatchKeyEvent(event)' returns true
            // while it should not.
            // See Bug https://projects.aerys.in/issues/6838.

            surfaceView.dispatchKeyEvent(event);

            return super.dispatchKeyEvent(event);
        }

        return surfaceView.dispatchKeyEvent(event);
    }

    @Override
    public boolean onGenericMotionEvent(MotionEvent event)
    {
        // Dispatch the motion events on the SDL canvas (including joystick axes)
        SurfaceView surfaceView = (SurfaceView)SDLActivity.getLayout().getChildAt(0);

        return surfaceView.dispatchGenericMotionEvent(event);
    }
}
