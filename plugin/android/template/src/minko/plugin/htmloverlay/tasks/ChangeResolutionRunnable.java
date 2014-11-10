package minko.plugin.htmloverlay;

import android.webkit.WebView;
import android.view.ViewGroup;
import android.util.Log;

public class ChangeResolutionRunnable implements Runnable 
{
    private int _width;
	private int _height;
	private WebView _webView;

    public ChangeResolutionRunnable(WebView webView , int width, int height)
    {
		_width = width;
		_height = height;
		_webView = webView;
    }

    @Override
    public void run()
	{
		Log.i("minko-java", "[ChangeResolutionRunnable] Change WebView resolution (width: " + _width + ", height: " + _height + ").");

		ViewGroup.LayoutParams layoutParams = _webView.getLayoutParams();

		layoutParams.width = _width;
		layoutParams.height = _height;
		
		_webView.setLayoutParams(layoutParams);
    }
}