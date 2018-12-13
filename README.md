[![Build status][s1]][av] [![Code quality][s2]][co]  [![License][s3]][li]

[s1]: https://ci.appveyor.com/api/projects/status/y8ew5io98llltkk2?svg=true
[s2]: https://api.codacy.com/project/badge/Grade/377d3777301f4bc09d1626de1c96ec8d
[s3]: https://img.shields.io/badge/licence-GPL%203.0-blue.svg

[av]: https://ci.appveyor.com/project/matt77hias/direct3d-11-projects
[co]: https://www.codacy.com/app/matt77hias/D3D11-Projects?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=matt77hias/D3D11-Projects&amp;utm_campaign=Badge_Grade
[li]: https://raw.githubusercontent.com/matt77hias/D3D11-Projects/master/LICENSE.txt

# D3D11-Projects

## About
Projects to start learning Direct3D 11 (D3D11).

**Note**: [Version 1.2](https://github.com/matt77hias/D3D11-Projects/releases/tag/v1.2) (obsolete) supports Windows 8.1 and uses a more Object-Oriented Design with more verbose documentation as opposed to the current version. Furthermore, it contains some useful code snippets that are not included anymore in the latest version.

<p align="center"><img src="res/d3d11.png"></p>

* [Project 1](https://github.com/matt77hias/D3D11-Projects/tree/master/Projects/Project1): Creating a window for displaying.
* [Project 2](https://github.com/matt77hias/D3D11-Projects/tree/master/Projects/Project2): Rendering the background color of a window.
* [Project 3](https://github.com/matt77hias/D3D11-Projects/tree/master/Projects/Project3): Rendering a triangle that is defined in projection space.
* [Project 4](https://github.com/matt77hias/D3D11-Projects/tree/master/Projects/Project4): Rendering and animating a cube that is defined in object space.
* [Project 5](https://github.com/matt77hias/D3D11-Projects/tree/master/Projects/Project5): Rendering and animating multiple cubes that are defined in object space.
* [Project 6](https://github.com/matt77hias/D3D11-Projects/tree/master/Projects/Project6): Lighting.
* [Project 7](https://github.com/matt77hias/D3D11-Projects/tree/master/Projects/Project7): Textures.
* [Project 8](https://github.com/matt77hias/D3D11-Projects/tree/master/Projects/Project8): Switching between windowed and fullscreen mode.

## Development
* **Platform**: Windows 10 32 Bit and 64 Bit (Windows API + Direct3D 11)
* **Engine Programming Language**: ISO C++ Latest Draft Standard (> C++17)
* **Scripting Language**: ISO C++ Latest Draft Standard (> C++17)
* **Shading Language**: HLSL (Shader Model 5.0)
* **Dependencies**:
  * d3d12.lib
  * dxgi.lib
  * (d3dcompiler.lib)
* **Compiler**: MSVC++ 15.9.4
* **IDE**: Visual Studio 2017

## Codebase
Based on:
[explanation](https://code.msdn.microsoft.com/windowsdesktop/Direct3D-Tutorial-Win32-829979ef) + [code](https://github.com/walbourn/directx-sdk-samples/tree/master/Direct3D11Tutorials)

* [(Dear) ImGui](https://github.com/ocornut/imgui) [[License](https://github.com/ocornut/imgui/blob/master/LICENSE.txt)]
  * [stb](https://github.com/nothings/stb) [[License](https://creativecommons.org/share-your-work/public-domain/cc0/)]
* [{fmt}](https://github.com/fmtlib/fmt) [[License](https://github.com/fmtlib/fmt/blob/master/LICENSE.rst)]
* [MAGE](https://github.com/matt77hias/MAGE) [[License](https://github.com/matt77hias/MAGE/blob/master/LICENSE.txt)]
* Microsoft Corporation Libraries:
  * [Direct3D 11](https://docs.microsoft.com/nl-be/windows/desktop/direct3d11/atoc-dx-graphics-direct3d-11)
  * [DirectXShaderCompiler](https://github.com/Microsoft/DirectXShaderCompiler) [[License](https://github.com/Microsoft/DirectXShaderCompiler/blob/master/LICENSE.TXT)]
  * [DirectXMath](https://github.com/Microsoft/DirectXMath) [[License](https://github.com/Microsoft/DirectXMath/blob/master/LICENSE)]
  * [DirectXTex](https://github.com/Microsoft/DirectXTex) [[License](https://github.com/Microsoft/DirectXTex/blob/master/LICENSE)]
  * [Guidelines Support Library (GSL)](https://github.com/Microsoft/GSL) [[License](https://github.com/Microsoft/GSL/blob/master/LICENSE)]
  * [Microsoft DirectX Graphics Infrastructure (DXGI)](https://docs.microsoft.com/en-us/windows/desktop/direct3ddxgi/d3d10-graphics-programming-guide-dxgi)
  * [Windows Runtime C++ Template Library (WRL)](https://docs.microsoft.com/nl-be/cpp/windows/windows-runtime-cpp-template-library-wrl?view=vs-2017)
* [std](https://en.cppreference.com/w/cpp/header)

<p align="center">Copyright © 2016-2018 Matthias Moulin. All Rights Reserved.</p>
