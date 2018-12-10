Some statically-compiled shaders.

VC++ doesn't easily supports resources in static libraries, that's why we're including compiled shaders inside the source code.

Because of this, we need all static shaders to compile before we compile the DxVisuals project that uses them.

This separate static library project is one simple way to accomplish that.