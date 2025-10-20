package minko.plugin.htmloverlay;

import android.webkit.WebView;
import java.util.concurrent.Callable;
import java.util.Date;
import android.util.Log;
import android.os.Build;
import android.webkit.ValueCallback;

public class EvalJSCallable implements Callable<String>
{
	private WebView _webView;
	private String _js;

	public EvalJSCallable(WebView webView, String js)
	{
		_webView = webView;
		_js = js;
	}

	@Override
    public String call() throws Exception
	{
		// Use evaluateJavascript for Android 4.4+ (KitKat) - better for large scripts
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT)
		{
			// First execute the JS expression
			_webView.evaluateJavascript(_js, new ValueCallback<String>() {
				@Override
				public void onReceiveValue(String value) {
					// Set the result through the JS interface
					try {
						_webView.evaluateJavascript("MinkoNativeInterface.onNativeJSResult(\"" + _js + "\", " + value + ");", null);
					} catch (Exception e) {
						Log.e("minko-java", "[EvalJSCallable] Error calling onNativeJSResult: " + e.getMessage());
					}
				}
			});
		}
		else
		{
			// Fallback to loadUrl for older Android versions
			// Escape _js for embedding in JavaScript string literals (needs extra escaping for eval)
			String escapedJs = _js.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n").replace("\r", "\\r");
			String evalString = "javascript:MinkoNativeInterface.onNativeJSResult(\"" + escapedJs + "\", eval(\"" + escapedJs + "\"));";
			_webView.loadUrl(evalString);
		}

		long startTime = System.currentTimeMillis();
		long elapsedTime = 0L;

		// We wait for the the result of JS eval
		while(elapsedTime < 100) // Timeout after X milliseconds
		{
			if (WebViewJSInterface.ResultReady)
			{
				WebViewJSInterface.ResultReady = false;

				return WebViewJSInterface.Result;
			}

			elapsedTime = (new Date()).getTime() - startTime;
		}

		Log.e("minko-java", "[EvalJSCallable] For some reason, the eval of the following javascript expression didn't retrieve a result in time: " + _js);

		// The JS eval didn't return anything in time
		return "null";
    }
}
