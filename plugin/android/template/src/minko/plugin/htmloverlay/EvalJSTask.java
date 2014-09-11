package minko.plugin.htmloverlay;

import android.webkit.WebView;
import java.util.concurrent.Callable;
import static android.util.Log.*;

public class EvalJSTask implements Callable<String> 
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
    public String call() throws Exception 
	{
		_webView.loadUrl("javascript:MinkoNativeInterface.onNativeJSResult(eval(\"" + _js + "\"))");

		return "JS RETURN COUCOU";
    }
/*
    @Override
    public void run() 
	{
		d("MINKO", "TRY TO EVALUATE JS => " + _js);
		
		_webView.loadUrl("javascript:MinkoNativeInterface.onNativeJSResult(eval(\"" + _js + "\"))");
    }
*/
}