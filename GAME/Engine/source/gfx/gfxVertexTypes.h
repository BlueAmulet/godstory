//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

DEFINE_VERT( GFXVertexPT, GFXVertexFlagXYZ | GFXVertexFlagTextureCount1 | GFXVertexFlagUV0 )
{
   Point3F point;
   Point2F texCoord;
};

DEFINE_VERT( GFXVertexPTT, GFXVertexFlagXYZ | GFXVertexFlagTextureCount2 | GFXVertexFlagUV0 | GFXVertexFlagUV1  )
{
   Point3F point;
   Point2F texCoord1;
   Point2F texCoord2;
};

DEFINE_VERT( GFXVertexPTTT, GFXVertexFlagXYZ | GFXVertexFlagTextureCount3 | GFXVertexFlagUV0 | GFXVertexFlagUV1 | GFXVertexFlagUV2 )
{
   Point3F point;
   Point2F texCoord1;
   Point2F texCoord2;
   Point2F texCoord3;
};

DEFINE_VERT( GFXVertexPC, GFXVertexFlagXYZ | GFXVertexFlagDiffuse )
{
   Point3F point;
   GFXVertexColor color;
};

DEFINE_VERT( GFXVertexPCN, GFXVertexFlagXYZ | GFXVertexFlagNormal | GFXVertexFlagDiffuse)
{
   Point3F point;
   Point3F normal;
   GFXVertexColor color;
};

DEFINE_VERT( GFXVertexPCT, 
            GFXVertexFlagXYZ | GFXVertexFlagDiffuse | GFXVertexFlagTextureCount1 | GFXVertexFlagUV0 )
{
   Point3F point;
   GFXVertexColor color;
   Point2F texCoord;
};

DEFINE_VERT( GFXVertexPCTT, 
			GFXVertexFlagXYZ | GFXVertexFlagDiffuse | GFXVertexFlagTextureCount2 | GFXVertexFlagUV0  | GFXVertexFlagUV1 )
{
	Point3F point;
	GFXVertexColor color;
	Point2F texCoord;
	Point2F texCoord2;
};

DEFINE_VERT( GFXVertexPN, GFXVertexFlagXYZ | GFXVertexFlagNormal )
{
   Point3F point;
   Point3F normal;
};

DEFINE_VERT( GFXVertexPNT, 
            GFXVertexFlagXYZ | GFXVertexFlagNormal | GFXVertexFlagTextureCount1 | GFXVertexFlagUV0 )
{
   Point3F point;
   Point3F normal;
   Point2F texCoord;
};

DEFINE_VERT( GFXVertexPCNT, 
            GFXVertexFlagXYZ | GFXVertexFlagNormal | GFXVertexFlagDiffuse | GFXVertexFlagTextureCount1 | GFXVertexFlagUV0)
{
   Point3F point;
   Point3F normal;
   GFXVertexColor color;
   Point2F texCoord;
};

DEFINE_VERT( GFXVertexPCNTT, 
            GFXVertexFlagXYZ | GFXVertexFlagNormal | GFXVertexFlagDiffuse | GFXVertexFlagTextureCount2 | GFXVertexFlagUV0 | GFXVertexFlagUV1)
{
   Point3F point;
   Point3F normal;
   GFXVertexColor color;
   Point2F texCoord[2];
};

DEFINE_VERT( GFXAtlasVert2,
            GFXVertexFlagXYZ | GFXVertexFlagNormal | GFXVertexFlagUV0 | GFXVertexFlagTextureCount1)
{
   Point3F point;
   Point3F normal;
   Point2F texCoord;
};

DEFINE_VERT( GFXVertexPNTTB,
			GFXVertexFlagXYZ | GFXVertexFlagNormal | GFXVertexFlagTextureCount3 | 
			GFXVertexFlagUV0 | GFXVertexFlagUV1 | GFXVertexFlagUVW2 | GFXVertexFlagUVW3)
{
	Point3F point;
	Point3F normal;
	Point2F texCoord;
	Point2F texCoord2;
	Point3F T;
	Point3F B;
};

DEFINE_VERT( GFXVertexPNTTT,
            GFXVertexFlagXYZ | GFXVertexFlagNormal | GFXVertexFlagTextureCount3 | 
            GFXVertexFlagUV0 | GFXVertexFlagUV1 | GFXVertexFlagUVWQ2)
{
   Point3F point;
   Point3F normal;
   Point2F texCoord;
   Point2F texCoord2;
   Point4F T;
};

//Ray: support vertex color
DEFINE_VERT( GFXVertexPCNTTT,
			GFXVertexFlagXYZ | GFXVertexFlagNormal | GFXVertexFlagDiffuse | GFXVertexFlagTextureCount3 | 
			GFXVertexFlagUV0 | GFXVertexFlagUV1 | GFXVertexFlagUVWQ2  )
{
	Point3F point;
	Point3F normal;
	GFXVertexColor color;
	Point2F texCoord;
	Point2F texCoord2;
	Point4F T;
};

DEFINE_VERT( GFXVertexPNTTBBBBT,
			GFXVertexFlagXYZ | GFXVertexFlagNormal | GFXVertexFlagTextureCount7 | 
			GFXVertexFlagUV0 | GFXVertexFlagUV1 | 
			GFXVertexFlagUV2 | GFXVertexFlagUV3 | GFXVertexFlagUV4 | GFXVertexFlagUV5 |
			GFXVertexFlagUVWQ6)
{
	Point3F point;
	Point3F normal;
	Point2F texCoord;
	Point2F texCoord2;
	Point2F Bone0;
	Point2F Bone1;
	Point2F Bone2;
	Point2F Bone3;
	Point4F T;
};

DEFINE_VERT( GFXVertexPCNTTBBBBT,
			GFXVertexFlagXYZ | GFXVertexFlagNormal | GFXVertexFlagDiffuse | GFXVertexFlagTextureCount7 | 
			GFXVertexFlagUV0 | GFXVertexFlagUV1 |  
			GFXVertexFlagUV2 | GFXVertexFlagUV3 | GFXVertexFlagUV4 | GFXVertexFlagUV5 |
			GFXVertexFlagUVWQ6)
{
	Point3F point;
	Point3F normal;
	GFXVertexColor color;
	Point2F texCoord;
	Point2F texCoord2;
	Point2F Bone0;
	Point2F Bone1;
	Point2F Bone2;
	Point2F Bone3;
	Point4F T;
};

DEFINE_VERT( GFXVertexTTT,
			GFXVertexFlagTextureCount3 | 
			GFXVertexFlagUV0 | GFXVertexFlagUV1 | 
			GFXVertexFlagUVWQ2)
{
	Point2F texCoord;
	Point2F texCoord2;
	Point4F T;
};

DEFINE_VERT( GFXVertexCTTT,
			GFXVertexFlagDiffuse | GFXVertexFlagTextureCount3 | 
			GFXVertexFlagUV0 | GFXVertexFlagUV1 | 
			GFXVertexFlagUVW2)
{
	GFXVertexColor color;
	Point2F texCoord;
	Point2F texCoord2;
	Point4F T;
};

DEFINE_VERT( GFXVertexPN4R, GFXVertexFlagXYZRHW | GFXVertexFlagNormal)
{
	Point4F point;
	Point4F normal;
};

DEFINE_VERT( GFXVertexP4RT, GFXVertexFlagXYZRHW | GFXVertexFlagTextureCount1 | GFXVertexFlagUV0)
{
	Point4F point;
	Point2F texCoord;
};

DEFINE_VERT( GFXVertexP4WT, GFXVertexFlagXYZW | GFXVertexFlagTextureCount1 | GFXVertexFlagUV0 )
{
	Point4F point;
	Point2F texCoord;
};

// Currently unused
DEFINE_VERT( GFXVertexP, GFXVertexFlagXYZ )
{
	Point3F point;
};

DEFINE_VERT( GFXVertexPWCT, GFXVertexFlagXYZW | GFXVertexFlagDiffuse | GFXVertexFlagTextureCount1 | GFXVertexFlagUV0)
{
	Point4F point;
	GFXVertexColor color;
	Point2F texCoord;
};

/*
DEFINE_VERT( GFXVertexPTT, GFXVertexFlagXYZ | GFXVertexFlagTextureCount2 | GFXVertexFlagUV0 | GFXVertexFlagUV1 )
{
Point3F point;
Point2F texCoord1;
Point2F texCoord2;
};
*/

//Ray: 流格式定义，注意和顶点的填充顺序需要一样
DECL_VERT(GFXDeclareP4T)
{
	0, GFX_TYPE_VERTEX,		GFX_FORMAT_FLOAT, 4,
	0, GFX_TYPE_TEXCOORD,	GFX_FORMAT_FLOAT, 2,
};

DECL_VERT(GFXDeclareP3T)
{
	0, GFX_TYPE_VERTEX,		GFX_FORMAT_FLOAT, 3,
	0, GFX_TYPE_TEXCOORD,	GFX_FORMAT_FLOAT, 2,
};

DECL_VERT(GFXDeclarePN4TTT)
{
	1, GFX_TYPE_VERTEX,		GFX_FORMAT_FLOAT, 4,
	1, GFX_TYPE_NORMAL,		GFX_FORMAT_FLOAT, 4,
	0, GFX_TYPE_TEXCOORD,	GFX_FORMAT_FLOAT, 2,
	0, GFX_TYPE_TEXCOORD,	GFX_FORMAT_FLOAT, 2,
	0, GFX_TYPE_TEXCOORD,	GFX_FORMAT_FLOAT, 4,
};

DECL_VERT(GFXDeclarePN4CTTT)
{
	1, GFX_TYPE_VERTEX,		GFX_FORMAT_FLOAT,	4,
	1, GFX_TYPE_NORMAL,		GFX_FORMAT_FLOAT,	4,
	0, GFX_TYPE_COLOR,		GFX_FORMAT_D3DCOLOR,1,
	0, GFX_TYPE_TEXCOORD,	GFX_FORMAT_FLOAT,	2,
	0, GFX_TYPE_TEXCOORD,	GFX_FORMAT_FLOAT,	2,
	0, GFX_TYPE_TEXCOORD,	GFX_FORMAT_FLOAT,	4,
};
