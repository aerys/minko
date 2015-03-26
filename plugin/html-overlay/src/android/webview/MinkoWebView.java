package minko.plugin.htmloverlay;

import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.view.MotionEvent;
import android.content.Context;

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
				if (System.currentTimeMillis() - lastEventTime < 1000)
				{
					// Cancel all touch actions
					ev.setAction(MotionEvent.ACTION_CANCEL);
					super.onTouchEvent(ev);
					// Restart the current action
					ev.setAction(MotionEvent.ACTION_DOWN);
					super.onTouchEvent(ev);
				}
				
				ev.setAction(MotionEvent.ACTION_UP);
				lastEventTime = System.currentTimeMillis();
				
				return super.onTouchEvent(ev);
			}
		}
		
        return super.onTouchEvent(ev);
    }
}
