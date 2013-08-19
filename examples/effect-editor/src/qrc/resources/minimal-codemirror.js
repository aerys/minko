var delay;

var codeMirror = CodeMirror(
	document.body,
	{
		value: "int square(int x){ return x*x; }\n",
		mode: "x-shader/x-vertex",
		smartIndent: true,
		indentWithTabs: true,
		tabSize: 4,
		lineWrapping: true,
		lineNumbers: true,
		highlightSelectionMatches: {showToken: /\w/},
		extraKeys: {"Ctrl-Q": function(cm){ cm.foldCode(cm.getCursor()); }},
		foldGutter: true,
		gutters: ["CodeMirror-linenumbers", "CodeMirror-foldgutter"]
	}
);

codeMirror.on("change", function()
{
	clearTimeout(delay);
	delay = setTimeout(updatePreview, 300);
});
  
function updatePreview()
{
	qMinkoEffectEditor.updateSource(qObjectID["type"]);
}
setTimeout(updatePreview, 300);

codeMirror.setSize("100%", "100%");
