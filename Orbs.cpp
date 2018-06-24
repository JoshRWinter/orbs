#include "Orbs.h"
#include "asset.h"

extern const char *vertexshader, *fragmentshader;

static PFNGLCREATESHADERPROC glCreateShader;
static PFNGLSHADERSOURCEPROC glShaderSource;
static PFNGLCOMPILESHADERPROC glCompileShader;
static PFNGLGETSHADERIVPROC glGetShaderiv;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
static PFNGLATTACHSHADERPROC glAttachShader;
static PFNGLLINKPROGRAMPROC glLinkProgram;
static PFNGLDELETESHADERPROC glDeleteShader;
static PFNGLCREATEPROGRAMPROC glCreateProgram;
static PFNGLUSEPROGRAMPROC glUseProgram;
static PFNGLDELETEPROGRAMPROC glDeleteProgram;
static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
static PFNGLGENBUFFERSPROC glGenBuffers;
static PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
static PFNGLBINDBUFFERPROC glBindBuffer;
static PFNGLBUFFERDATAPROC glBufferData;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
static PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
static PFNGLDELETEBUFFERSPROC glDeleteBuffers;
static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

Orbs::Orbs(int w, int h, int count)
{
	init_extensions();

	// opengl settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	// shaders
	const unsigned vshader = glCreateShader(GL_VERTEX_SHADER);
	const unsigned fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vshader, 1, &vertexshader, NULL);
	glShaderSource(fshader, 1, &fragmentshader, NULL);
	glCompileShader(vshader);
	glCompileShader(fshader);
	int success = 0;
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if(success == 0)
	{
		char buffer[2000];
		glGetShaderInfoLog(vshader, 2000, NULL, buffer);
		throw std::runtime_error(std::string("vertex shader:\n") + buffer);
	}
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if(success == 0)
	{
		char buffer[2000];
		glGetShaderInfoLog(fshader, 2000, NULL, buffer);
		throw std::runtime_error(std::string("fragment shader:\n") + buffer);
	}
	program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glUseProgram(program);

	float matrix[16];
	initortho(matrix, -8.0f, 8.0f, 4.5f, -4.5f, -1.0f, 1.0f);
	uniform.projection = get_uniform("projection");
	glUniformMatrix4fv(uniform.projection, 1, false, matrix);

	load_texture();

	const float verts[] =
	{
		-0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, -0.5f, 1.0f, 1.0f
	};

	const unsigned int indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 4, NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(float) * 4, (void*)(sizeof(float)*2));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void Orbs::step()
{
}

void Orbs::render() const
{
	glClear(GL_COLOR_BUFFER_BIT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Orbs::stop()
{
	glDeleteProgram(program);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteTextures(1, &texture);
}

void Orbs::load_texture()
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_data.width, image_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data.pixel_data);
}

void *Orbs::getproc(const char *name)
{
	void *fn = SDL_GL_GetProcAddress(name);

	if(fn == NULL)
		throw std::runtime_error(std::string("could not load function address \"") + name + "\"");

	return fn;
}

int Orbs::get_uniform(const char *name) const
{
	const int u = glGetUniformLocation(program, name);
	if(u == -1)
		throw std::runtime_error(std::string("could get uniform variable \"") + name + "\"");

	return u;
}

void Orbs::init_extensions()
{
	glCreateShader = (decltype(glCreateShader))getproc("glCreateShader");
	glShaderSource = (decltype(glShaderSource))getproc("glShaderSource");
	glCompileShader = (decltype(glCompileShader))getproc("glCompileShader");
	glGetShaderiv = (decltype(glGetShaderiv))getproc("glGetShaderiv");
	glGetShaderInfoLog = (decltype(glGetShaderInfoLog))getproc("glGetShaderInfoLog");
	glAttachShader = (decltype(glAttachShader))getproc("glAttachShader");
	glLinkProgram = (decltype(glLinkProgram))getproc("glLinkProgram");
	glDeleteShader = (decltype(glDeleteShader))getproc("glDeleteShader");
	glCreateProgram = (decltype(glCreateProgram))getproc("glCreateProgram");
	glUseProgram = (decltype(glUseProgram))getproc("glUseProgram");
	glDeleteProgram = (decltype(glDeleteProgram))getproc("glDeletProgram");
	glGenVertexArrays = (decltype(glGenVertexArrays))getproc("glGenVertexArrays");
	glGenBuffers = (decltype(glGenBuffers))getproc("glGenBuffers");
	glBindVertexArray = (decltype(glBindVertexArray))getproc("glBindVertexArray");
	glBindBuffer = (decltype(glBindBuffer))getproc("glBindBuffer");
	glBufferData = (decltype(glBufferData))getproc("glBufferData");
	glVertexAttribPointer = (decltype(glVertexAttribPointer))getproc("glVertexAttribPointer");
	glEnableVertexAttribArray = (decltype(glEnableVertexAttribArray))getproc("glEnableVertexAttribArray");
	glDeleteVertexArrays = (decltype(glDeleteVertexArrays))getproc("glDeleteVertexArrays");
	glDeleteBuffers = (decltype(glDeleteBuffers))getproc("glDeleteBuffers");
	glGetUniformLocation = (decltype(glGetUniformLocation))getproc("glGetUniformLocation");
	glUniformMatrix4fv = (decltype(glUniformMatrix4fv))getproc("glUniformMatrix4fv");
}

void Orbs::initortho(float *matrix,float left,float right,float bottom,float top,float znear,float zfar)
{
	matrix[0]=2.0f/(right-left);
	matrix[1]=0.0f;
	matrix[2]=0.0f;
	matrix[3]=0.0f;
	matrix[4]=0.0f;
	matrix[5]=2.0f/(top-bottom);
	matrix[6]=0.0f;
	matrix[7]=0.0f;
	matrix[8]=0.0f;
	matrix[9]=0.0f;
	matrix[10]=-2.0f/(zfar-znear);
	matrix[11]=0.0f;
	matrix[12]=-((right+left)/(right-left));
	matrix[13]=-((top+bottom)/(top-bottom));
	matrix[14]=-((zfar+znear)/(zfar-znear));
	matrix[15]=1.0f;
}
