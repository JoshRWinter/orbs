const char *vertexshader=
"#version 330 core\n"
"layout (location=0) in vec2 pos;\n"
"void main(){\n"
"gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);\n"
"}\n"
,*fragmentshader=
"#version 330 core\n"
"out vec4 color;\n"
"void main(){\n"
"color = vec4(1.0, 0.0, 0.0, 1.0);"
"}\n"
;
