#!/usr/bin/env python

import sys

def main():
    if len(sys.argv) != 3:
        print('usage: replace.py <project_name> <file>')
        sys.exit(1)

    project_name = sys.argv[1]
    input_file = sys.argv[2]

    with open(input_file, "r") as f:
        content = f.read()
    with open(input_file, "w") as f:
        f.write(content.replace("{{{ SCRIPT }}}", "<script>\n\
        function WebAssemblyIsSupported() {\n\
            try {\n\
                if (typeof WebAssembly === \"object\"\n\
                    && typeof WebAssembly.instantiate === \"function\") {\n\
                    const module = new WebAssembly.Module(Uint8Array.of(0x0, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00));\n\
                    if (module instanceof WebAssembly.Module) {\n\
                        return new WebAssembly.Instance(module) instanceof WebAssembly.Instance;\n\
                    }\n\
                }\n\
            } catch (e) {\n\
            }\n\
            return false;\n\
        };\n\
\n\
        if (WebAssemblyIsSupported() == true)\n\
            document.write('<script type=\"text/javascript\" async src=\"" + project_name + "-wasm.preload.js\"><\/script><script async type=\"text/javascript\" src=\"" + project_name + "-wasm.js\"><\\/script>');\n\
        else\n\
            document.write(\"<script type=\\\"text/javascript\\\" async src=\\\"" + project_name + "-asmjs.preload.js\\\"><\/script><script>(function() {var memoryInitializer = '" + project_name + "-asmjs.html.mem';if (typeof Module['locateFile'] === 'function') {memoryInitializer = Module['locateFile'](memoryInitializer);} else if (Module['memoryInitializerPrefixURL']) {memoryInitializer = Module['memoryInitializerPrefixURL'] + memoryInitializer;}var meminitXHR = Module['memoryInitializerRequest'] = new XMLHttpRequest();meminitXHR.open('GET', memoryInitializer, true);meminitXHR.responseType = 'arraybuffer';meminitXHR.send(null);})();var script = document.createElement('script');script.src = '" + project_name + "-asmjs.js';document.body.appendChild(script);<\\/script>\");\n\
    </script>"))
    f.write(content.replace("{{{ PRELOAD }}}", " "))

if __name__ == '__main__':
    main()