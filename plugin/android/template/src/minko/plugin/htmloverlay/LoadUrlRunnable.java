package minko.plugin.htmloverlay;

import android.webkit.WebView;

import static android.util.Log.*;

public class LoadUrlRunnable implements Runnable 
{
    private WebView _webView;
    private String _url;

    public LoadUrlRunnable(WebView webView, String url)
    {
		d("MINKOJAVA", "INSTANCIATE LOADURLRUNNABLE WITH WEBVIEW: " + webView);
        _url = url;
        _webView = webView;
    }

    @Override
    public void run()
	{
		d("MINKOJAVA", "RUN WEBVIEW LOADURL (webView: " + _webView + ", url: " + _url + ")");
		_webView.loadUrl(_url);
    }
}