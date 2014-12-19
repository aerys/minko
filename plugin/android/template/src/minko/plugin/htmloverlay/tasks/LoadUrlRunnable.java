package minko.plugin.htmloverlay;

import android.webkit.WebView;
import android.util.Log;

public class LoadUrlRunnable implements Runnable 
{
    private WebView _webView;
    private String _url;

    public LoadUrlRunnable(WebView webView, String url)
    {
		Log.i("minko-java", "[LoadUrlRunnable] Instantiate LoadUrlRunnable (webView: " + webView + ", URL: " + url + ").");
        _url = url;
        _webView = webView;
    }

    @Override
    public void run()
	{
		Log.i("minko-java", "[LoadUrlRunnable] Run LoadUrlRunnable (webView: " + _webView + ", URL: " + _url + ").");
		_webView.loadUrl(_url);
    }
}