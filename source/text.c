#include "text.h"
#include "vshader_shbin.h"

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

int glyph_amount;

typedef struct { float position[3]; float texcoord[2]; } textVertex_s;

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static C3D_Tex* glyphSheets;
static textVertex_s* textVtxArray;
static int textVtxArrayPos;

#define TEXT_VTX_ARRAY_COUNT (4*1024)

void textSceneInit(void)
{
	fontEnsureMapped();
	
	// Load the vertex shader, create a shader program and bind it
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
	C3D_BindProgram(&program);

	// Configure attributes for use with the vertex shader
	C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoord
	
	// Configure depth test to overwrite pixels with the same depth (needed to draw overlapping glyphs)
	C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

	// Load the glyph texture sheets
	TGLP_s* glyphInfo = fontGetGlyphInfo();
	glyphSheets = malloc(sizeof(C3D_Tex)*glyphInfo->nSheets);
	for (int i = 0; i < glyphInfo->nSheets; i ++)
	{
		C3D_Tex* tex = &glyphSheets[i];
		tex->data = fontGetGlyphSheetTex(i);
		tex->fmt = glyphInfo->sheetFmt;
		tex->size = glyphInfo->sheetSize;
		tex->width = glyphInfo->sheetWidth;
		tex->height = glyphInfo->sheetHeight;
		tex->param = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR)
			| GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_EDGE) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_EDGE);
	}

	// Create the text vertex array
	textVtxArray = (textVertex_s*)linearAlloc(sizeof(textVertex_s)*TEXT_VTX_ARRAY_COUNT);
}

void setTextColor(u32 color)
{
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_RGB, GPU_CONSTANT, 0, 0);
	C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0, GPU_CONSTANT, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);
	C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
	C3D_TexEnvColor(env, color);
}

static void addTextVertex(float vx, float vy, float tx, float ty)
{
	textVertex_s* vtx = &textVtxArray[textVtxArrayPos++];
	vtx->position[0] = vx;
	vtx->position[1] = vy;
	vtx->position[2] = 0.5f;
	vtx->texcoord[0] = tx;
	vtx->texcoord[1] = ty;
}

static void renderText(float x, float y, float scaleX, float scaleY, bool baseline, const char* text)
{
	ssize_t  units;
	uint32_t code;

	// Configure buffers
	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);
	BufInfo_Add(bufInfo, textVtxArray, sizeof(textVertex_s), 2, 0x10);

	const uint8_t* p = (const uint8_t*)text;
	float firstX = x;
	u32 flags = GLYPH_POS_CALC_VTXCOORD | (baseline ? GLYPH_POS_AT_BASELINE : 0);
	int lastSheet = -1;
	do
	{
		if (!*p) break;
		units = decode_utf8(&code, p);
		if (units == -1)
			break;
		p += units;
		if (code == '\n')
		{
			x = firstX;
			y += scaleY*fontGetInfo()->lineFeed;
		}
		else if (code > 0)
		{
			int glyphIdx = fontGlyphIndexFromCodePoint(code);
			fontGlyphPos_s data;
			fontCalcGlyphPos(&data, glyphIdx, flags, scaleX, scaleY);

			// Bind the correct texture sheet
			if (data.sheetIndex != lastSheet)
			{
				lastSheet = data.sheetIndex;
				C3D_TexBind(0, &glyphSheets[lastSheet]);
			}

			int arrayIndex = textVtxArrayPos;
			if ((arrayIndex+4) >= TEXT_VTX_ARRAY_COUNT)
				break; // We can't render more characters

			// Add the vertices to the array
			addTextVertex(x+data.vtxcoord.left,  y+data.vtxcoord.bottom, data.texcoord.left,  data.texcoord.bottom);
			addTextVertex(x+data.vtxcoord.right, y+data.vtxcoord.bottom, data.texcoord.right, data.texcoord.bottom);
			addTextVertex(x+data.vtxcoord.left,  y+data.vtxcoord.top,    data.texcoord.left,  data.texcoord.top);
			addTextVertex(x+data.vtxcoord.right, y+data.vtxcoord.top,    data.texcoord.right, data.texcoord.top);

			// Draw the glyph
			C3D_DrawArrays(GPU_TRIANGLE_STRIP, arrayIndex, 4);

			x += data.xAdvance;

		}
	} while (code > 0);
}

void drawText(u16 x, u16 y, const char * str, u32 color)
{
	setTextColor(__builtin_bswap32(color));
	renderText((float )x*8, (float )y*11, 0.75f, 0.75f, false, str);
}

void textSceneExit(void)
{
	// Free the textures
	free(glyphSheets);

	// Free the shader program
	shaderProgramFree(&program);
	DVLB_Free(vshader_dvlb);
}

void textStartDraw(void)
{
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	
	C3D_TexBind(0, glyphSheets);
	textVtxArrayPos = 0; // Clear the text vertex array
	C3D_FrameDrawOn(targets[GFX_TOP]);
	
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projections[GFX_TOP]);
}