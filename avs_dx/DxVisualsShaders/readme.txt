Some statically-compiled shaders.

VC++ doesn't easily supports resources in statis libraries, that's why we're including the compiler shaders inside the source code.

Because of this, we need all static shaders to compile before we compile the DxVisuals project.

A separate static library project is the simple way to do so.