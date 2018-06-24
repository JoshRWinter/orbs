const char *vertexshader=
"#version 330 core\n"
"layout (location=0) in vec2 pos;\n"
"layout (location=1) in vec2 tc;\n"
"out vec2 texcoord;\n"
"uniform mat4 projection;\n"
"void main(){\n"
"texcoord = tc;\n"
"gl_Position = projection * vec4(pos.x, pos.y, 0.0, 1.0);\n"
"}\n"
,*fragmentshader=
"#version 330 core\n"
"out vec4 color;\n"
"in vec2 texcoord;\n"
"uniform sampler2D tex;\n"
"void main(){\n"
"color = texture(tex, texcoord);\n"
"}\n"
;
