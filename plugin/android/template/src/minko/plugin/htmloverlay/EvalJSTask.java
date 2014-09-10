package minko.plugin.htmloverlay;

import android.webkit.WebView;
import static android.util.Log.*;

public class EvalJSTask implements Runnable 
{
	WebView _webView;
	String _js;
	
	public EvalJSTask(WebView webView, String js)
	{
		_webView = webView;
		_js = js;
		d("MINKO", "INSTANTIATE EVAL JS TASK");
		d("MINKO", "WebView: " + _webView);
	}

    @Override
    public void run() 
	{
		d("MINKO", "TRY TO EVALUATE JS => " + _js);
	
		_webView.loadUrl("javascript: " + _js);
    }
}