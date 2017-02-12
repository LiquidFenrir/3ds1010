#include "draw.h"
#include "lodepng.h"
#include "vshader_shbin.h"

// Used to transfer the final rendered display to the framebuffer
#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

// Used to convert textures to 3DS tiled format
// Note: vertical flip flag set so 0,0 is top left of texture
#define TEXTURE_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(1) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

struct { float left, right, top, bottom; } images[] = {
	{0.0f, 0.25f, 0.5f, 0.75f}, //blank
	{0.25f, 0.5f, 0.5f, 0.75f}, //full 1
	{0.5f, 0.75f, 0.5f, 0.75f}, //full 2
	{0.75f, 1.0f, 0.5f, 0.75f}, //full 3
	{0.0f, 0.25f, 0.75f,  1.0f}, //themes
	{0.25f, 0.5f, 0.75f,  1.0f}, //reset
	{0.5f, 0.75f, 0.75f,  1.0f}, //exit
	{0.75f, 0.875f, 0.75f, 0.875f}, //small 1
	{0.875f, 1.0f, 0.75f, 0.875f}, //small 2
	{0.75f, 0.875f, 0.875f,1.0f}, //small 3
	{0.0f, 1.0f, 0.0f, 0.5f} //banner
};

static C3D_RenderTarget* target;

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection;
static C3D_Mtx projection;

static C3D_Tex spritesheet_tex;

void drawSpriteWithZ(int x, int y, int width, int height, int image, float z)
{
	float left = images[image].left;
	float right = images[image].right;
	float top = images[image].top;
	float bottom = images[image].bottom;

	// Draw a textured quad directly
	C3D_ImmDrawBegin(GPU_TRIANGLE_STRIP);
		C3D_ImmSendAttrib(x, y, z+0.5f, 0.0f); // v0=position
		C3D_ImmSendAttrib( left, top, z, 0.0f);

		C3D_ImmSendAttrib(x, y+height, z+0.5f, 0.0f);
		C3D_ImmSendAttrib( left, bottom, z, 0.0f);

		C3D_ImmSendAttrib(x+width, y, z+0.5f, 0.0f);
		C3D_ImmSendAttrib( right, top, z, 0.0f);

		C3D_ImmSendAttrib(x+width, y+height, z+0.5f, 0.0f);
		C3D_ImmSendAttrib( right, bottom, z, 0.0f);
	C3D_ImmDrawEnd();
}

void drawSprite(int x, int y, int width, int height, int image)
{
	drawSpriteWithZ(x, y, width, height, image, 0.1f);
}

void setupTextures(u8 * imgData, u32 imgSize, u32 clear_color)
{

	// Initialize the render target
	target = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetClear(target, C3D_CLEAR_ALL, clear_color, 0);
	C3D_RenderTargetSetOutput(target, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	
	// Load the vertex shader, create a shader program and bind it
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
	C3D_BindProgram(&program);

	// Get the location of the uniforms
	uLoc_projection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");

	// Configure attributes for use with the vertex shader
	// Attribute format and element count are ignored in immediate mode
	C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v2=texcoord

	// Compute the projection matrix
	// Note: we're setting top to 240 here so origin is at top left.
	Mtx_OrthoTilt(&projection, 0.0, 320.0, 240.0, 0.0, 0.0, 1.0, true);

	// Configure buffers
	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);

	unsigned char * image;
	unsigned width, height;

	lodepng_decode32(&image, &width, &height, imgData, imgSize);

	u8 * gpusrc = linearAlloc(width*height*4);

	// GX_DisplayTransfer needs input buffer in linear RAM
	u8 * src = image;
	u8 * dst = gpusrc;

	// lodepng outputs big endian rgba so we need to convert
	for(int i = 0; (unsigned int)i < width*height; i++) {
		int r = *src++;
		int g = *src++;
		int b = *src++;
		int a = *src++;

		*dst++ = a;
		*dst++ = b;
		*dst++ = g;
		*dst++ = r;
	}

	// ensure data is in physical ram
	GSPGPU_FlushDataCache(gpusrc, width*height*4);

	// Load the texture and bind it to the first texture unit
	C3D_TexInit(&spritesheet_tex, width, height, GPU_RGBA8);

	// Convert image to 3DS tiled texture format
	C3D_SafeDisplayTransfer ((u32*)gpusrc, GX_BUFFER_DIM(width,height), (u32*)spritesheet_tex.data, GX_BUFFER_DIM(width,height), TEXTURE_TRANSFER_FLAGS);
	gspWaitForPPF();

	C3D_TexSetFilter(&spritesheet_tex, GPU_LINEAR, GPU_NEAREST);
	C3D_TexBind(0, &spritesheet_tex);

	free(image);
	linearFree(gpusrc);

	// Configure the first fragment shading substage to just pass through the texture color
	// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, 0, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
	
	// Configure depth test to overwrite pixels with the same depth (needed to draw overlapping sprites)
	C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
}

void sceneExit(void)
{
	// Free the shader program
	shaderProgramFree(&program);
	DVLB_Free(vshader_dvlb);
	freeTheme();
}

void startDraw(void)
{
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C3D_FrameDrawOn(target);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);
}

void endDraw(void)
{
	C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
	C3D_FrameEnd(0);
}