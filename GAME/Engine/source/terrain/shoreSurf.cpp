//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "terrain/shoreSurf.h"
#include "terrain/waterBlock.h"
#include "terrain/terrData.h"
#include "sceneGraph/sceneGraph.h"
#include "gui/3d/guiTSControl.h"
#include SHADER_CONSTANT_INCLUDE_FILE


Vector<ShoreSurf::ShoreEdge> ShoreSurf::smTempEdges( 400 );
Vector<GFXVertexShoreSurf> ShoreSurf::smTempVerts( 400 );
GFXShader* ShoreSurf::smMaskShader = NULL;
GFXShader* ShoreSurf::smShader = NULL;

ShoreSurf::ShoreSurf()
{
    mTerrainBlock = NULL;
    mWaterBlock = NULL;

    mShoreLineNeedUpdate = false;
    mShoreVBNeedUpdate = false;
    mSurfTexNeedUpdate = false;    

    mPrevPassIsMask = false;

    mSurfTex = NULL;
    mVB = NULL;

    setShoreSurfEnable( false );

    setShoreHeight( 0.1f );

    setSurfRandSeed( 2009 );
    setSurfWidth( 5.0f, 3.0f );
    setSurfHeight( 5.0f, 3.0f );
    setSurfPos( 0.0f, 1.0f );
    setSurfSpeed( 0.1f, 0.05f );
    setSurfAlpha( 1.0f, 0.0f );
    setSurfMiddle( 0.5f, 0.3f );

    setSurfTexFilename( "~/data/environments/water/waterShoreSurf" );
}

ShoreSurf::~ShoreSurf()
{
    mSurfTex = NULL;
    mVB = NULL;

    mShoreEdges.clear();
    for ( int i = 0; i < mShoreLines.size(); i++ )
        mShoreLines[i].clear();
    mShoreLines.clear();
}


void ShoreSurf::setShoreSurfEnable( bool enable )
{
    mShoreSurfEnable = enable;
}

void ShoreSurf::setShoreHeight( F32 shoreHeight )
{
    mClampF( shoreHeight, -999999.0f, 999999.0f );
    if ( mShoreHeight != shoreHeight )
    {
        mShoreHeight = shoreHeight;
        mShoreLineNeedUpdate = true;
    }
}

void ShoreSurf::setSurfRandSeed( S32 seed )
{
    if ( mSurfRandSeed != seed )
    {
        mSurfRandSeed = seed;
        mShoreVBNeedUpdate = true;
    }
}

void ShoreSurf::setSurfWidth( F32 surfWidth, F32 surfWidthRand )
{
    mClampF( surfWidth, 0.0f, 999999.0f );
    mClampF( surfWidthRand, 0.0f, surfWidth );
    if ( mSurfWidth != surfWidth || mSurfWidthRand != surfWidthRand )
    {
        mSurfWidth = surfWidth;
        mSurfWidthRand = surfWidthRand;
        mShoreVBNeedUpdate = true;
    }
}

void ShoreSurf::setSurfHeight( F32 surfHeight, F32 surfHeightRand )
{
    mClampF( surfHeight, 0.0f, 999999.0f );
    mClampF( surfHeightRand, 0.0f, surfHeight );
    if ( mSurfHeight != surfHeight || mSurfHeightRand != surfHeightRand )
    {
        mSurfHeight = surfHeight;
        mSurfHeightRand = surfHeightRand;
        mShoreVBNeedUpdate = true;
    }
}

void ShoreSurf::setSurfPos( F32 surfPos, F32 surfPosRand )
{
    mClampF( surfPos, -999999.0f, 999999.0f );
    mClampF( surfPosRand, 0.0f, 999999.0f );
    if ( mSurfPos != surfPos || mSurfPosRand != surfPosRand )
    {
        mSurfPos = surfPos;
        mSurfPosRand = surfPosRand;
        mShoreVBNeedUpdate = true;
    }
}

void ShoreSurf::setSurfSpeed( F32 surfSpeed, F32 surfSpeedRand )
{
    mClampF( surfSpeed, -999999.0f, 999999.0f );
    mClampF( surfSpeedRand, 0.0f, 999999.0f );
    if ( mSurfSpeed != surfSpeed || mSurfSpeedRand != surfSpeedRand )
    {
        mSurfSpeed = surfSpeed;
        mSurfSpeedRand = surfSpeedRand;
        mShoreVBNeedUpdate = true;
    }
}

void ShoreSurf::setSurfAlpha( F32 surfAlpha, F32 surfAlphaRand )
{
    mClampF( surfAlpha, 0.0f, 1.0f );
    mClampF( surfAlphaRand, 0.0f, surfAlpha );
    if ( mSurfAlpha != surfAlpha || mSurfAlphaRand != surfAlphaRand )
    {
        mSurfAlpha = surfAlpha;
        mSurfAlphaRand = surfAlphaRand;
        mShoreVBNeedUpdate = true;
    }
}

void ShoreSurf::setSurfMiddle( F32 surfMiddle, F32 surfMiddleRand )
{
    mClampF( surfMiddle, 0.0f, 1.0f );
    mClampF( surfMiddleRand, 0.0f, surfMiddle );
    if ( mSurfMiddle != surfMiddle || mSurfMiddleRand != surfMiddleRand )
    {
        mSurfMiddle = surfMiddle;
        mSurfMiddleRand = surfMiddleRand;
        mShoreVBNeedUpdate = true;
    }
}

void ShoreSurf::setSurfTexFilename( const char* surfTexFilename )
{
    if ( surfTexFilename == NULL || surfTexFilename[0] == 0 )
    {
        if ( mSurfTexFilename[0] != 0 )
        {
            mSurfTexFilename[0] = 0;
            mSurfTexNeedUpdate = true;
        }
    }
    else
    {
        if ( mSurfTexFilename[0] == 0 || dStrcmp( mSurfTexFilename, surfTexFilename ) != 0 )
        {
            dStrcpy( mSurfTexFilename, 260, surfTexFilename );
            mSurfTexNeedUpdate = true;
        }
    }
}

void ShoreSurf::setTerrainBlock( TerrainBlock* terrainBlock )
{
    if ( mTerrainBlock != terrainBlock )
    {
        mTerrainBlock = terrainBlock;
        mShoreLineNeedUpdate = true;
    }
}

void ShoreSurf::setWaterBlock( WaterBlock* waterBlock )
{
    if ( mWaterBlock != waterBlock )
    {
        mWaterBlock = waterBlock;
        mShoreLineNeedUpdate = true;
    }
}

void ShoreSurf::setShoreLineNeedUpdate()
{
    mShoreLineNeedUpdate = true;
}
void ShoreSurf::setShoreVBNeedUpdate()
{
    mShoreVBNeedUpdate = true;
}


bool ShoreSurf::initResource()
{
    if ( !smMaskShader )
        smMaskShader = GFX->createShader( "gameres/shaders/water/shoreSurfMaskV.hlsl", "gameres/shaders/water/shoreSurfMaskP.hlsl", GFX->getPixelShaderVersion() );

    if ( !smShader )
        smShader = GFX->createShader( "gameres/shaders/water/shoreSurfV.hlsl", "gameres/shaders/water/shoreSurfP.hlsl", GFX->getPixelShaderVersion() );

    if ( mSurfTexNeedUpdate )
    {
        mSurfTex.set( mSurfTexFilename, &GFXDefaultStaticDiffuseProfile );
        mSurfTexNeedUpdate = false;
    }

    return ( smMaskShader && smShader && mSurfTex );
}


void ShoreSurf::calcIntersection( const Point3F& terrPosA, const Point3F& terrNormA, const Point3F& terrPosB, const Point3F& terrNormB, F32 planeHeight, Intersection& mid )
{
    // ���㳬���߶� AB ��Χ���޽���
    if ( ( planeHeight > terrPosA.z && planeHeight > terrPosB.z )
        || ( planeHeight < terrPosA.z && planeHeight < terrPosB.z ) )
        mid.flag = false;
    else
    {
        // AB ���и���ƽ�����޽���
        F32 deltaZ = terrPosA.z - terrPosB.z;
        if ( deltaZ == 0.0f )
            mid.flag = false;
        else
        {
            mid.flag = true;

            // �� AB ���и��潻��
            F32 h_bz = planeHeight - terrPosB.z;
            F32 az_h = terrPosA.z - planeHeight;

            mid.vert.pos.x = ( terrPosA.x * h_bz + terrPosB.x * az_h ) / deltaZ;
            mid.vert.pos.y = ( terrPosA.y * h_bz + terrPosB.y * az_h ) / deltaZ;
            mid.vert.pos.z = planeHeight;

            // �� AB ����ķ�������ֵ��ý��㴦ƽ���и��淽���ϵķ�����
            mid.vert.norm = terrNormA + terrNormB;
            mid.vert.norm.z = 0.0f;
            mid.vert.norm.normalize();		
        }
    }
}

void ShoreSurf::linkIntersection( const Intersection& intersectionA, const Intersection& intersectionB, Vector<ShoreEdge>& shoreEdges )
{
    // ������������Ϊһ�����߶α�, ����ӵ��߼�����
    if ( intersectionA.flag && intersectionB.flag )
    {
        ShoreEdge edge;
        edge.start = intersectionA.vert;
        edge.end = intersectionB.vert;
        shoreEdges.push_back( edge );
    }
}


bool ShoreSurf::findShoreLine()
{
    if ( mShoreSurfEnable && mTerrainBlock && mWaterBlock && mShoreLineNeedUpdate )
    {
        mShoreEdges.clear();
        for ( int i = 0; i < mShoreLines.size(); i++ )
            mShoreLines[i].clear();
        mShoreLines.clear();

        //
        // ��ˮ����ε����н���
        //
        Point2F pos;
        Point3F norm;
        F32 height;
        Point3F terrPos[5], terrNorm[5];

        MatrixF terrMat = mTerrainBlock->getRenderTransform();
        MatrixF invTerrMat = terrMat;
        invTerrMat.affineInverse();

        MatrixF waterMat = mWaterBlock->getRenderTransform();        
        MatrixF invWaterMat = waterMat;
        invWaterMat.affineInverse();

        MatrixF terrSpaceToWaterSpace = invWaterMat * terrMat;
        MatrixF waterSpaceToTerrSpace = invTerrMat * waterMat;

        Point3F planePos;
        waterSpaceToTerrSpace.mulP( Point3F( 0.0f, 0.0f, mShoreHeight ), &planePos );
        F32 planeHeight = planePos.z;

        Box3F waterBox = mWaterBlock->getObjBox();
        waterBox.min.convolve(mWaterBlock->getScale());
        waterBox.max.convolve(mWaterBlock->getScale());
        waterBox.max.z = 999999.0f;
        waterBox.min.z = -999999.0f;
        Point3F tmpPos;

        for ( U32 i = 0; i < 255 - 1; i++ )
        {
            for ( U32 j = 0; j < 255 - 1; j++ )
            {
                // ȡ 5 ������
                // *(0)      *(1)
                //	    *(4)
                // *(2)      *(3)
                pos.x = i * 2.0f;
                pos.y = j * 2.0f;
                mTerrainBlock->getNormalAndHeight( pos, &norm, &height );
                terrPos[0].set( pos.x, pos.y, height );
                terrNorm[0] = norm;

                pos.x = i * 2.0f;
                pos.y = ( j + 1 ) * 2.0f;
                mTerrainBlock->getNormalAndHeight( pos, &norm, &height );
                terrPos[1].set( pos.x, pos.y, height );
                terrNorm[1] = norm;

                pos.x = ( i + 1 ) * 2.0f;
                pos.y = j * 2.0f;
                mTerrainBlock->getNormalAndHeight( pos, &norm, &height );
                terrPos[2].set( pos.x, pos.y, height );
                terrNorm[2] = norm;

                pos.x = ( i + 1 ) * 2.0f;
                pos.y = ( j + 1 ) * 2.0f;
                mTerrainBlock->getNormalAndHeight( pos, &norm, &height );
                terrPos[3].set( pos.x, pos.y, height );
                terrNorm[3] = norm;

                pos.x = i * 2.0f + 1.0f;
                pos.y = j * 2.0f + 1.0f;
                mTerrainBlock->getNormalAndHeight( pos, &norm, &height );
                terrPos[4].set( pos.x, pos.y, height );
                terrNorm[4] = norm;                

                // �жϵ��ζ����Ƿ���ˮ�淶Χ��
                terrSpaceToWaterSpace.mulP( terrPos[4], &tmpPos );
                if ( !waterBox.isContained( tmpPos ) )
                    continue;

                // �󽻵�
                // 0  4-0
                // 1  4-1
                // 2  4-2
                // 3  4-3
                // 4  0-2
                // 5  0-1
                // 6  1-3
                // 7  2-3
                Intersection intersection[8];
                calcIntersection( terrPos[4], terrNorm[4], terrPos[0], terrNorm[0], planeHeight, intersection[0] );
                calcIntersection( terrPos[4], terrNorm[4], terrPos[1], terrNorm[1], planeHeight, intersection[1] );
                calcIntersection( terrPos[4], terrNorm[4], terrPos[2], terrNorm[2], planeHeight, intersection[2] );
                calcIntersection( terrPos[4], terrNorm[4], terrPos[3], terrNorm[3], planeHeight, intersection[3] );
                calcIntersection( terrPos[0], terrNorm[0], terrPos[2], terrNorm[2], planeHeight, intersection[4] );
                calcIntersection( terrPos[0], terrNorm[0], terrPos[1], terrNorm[1], planeHeight, intersection[5] );
                calcIntersection( terrPos[1], terrNorm[1], terrPos[3], terrNorm[3], planeHeight, intersection[6] );
                calcIntersection( terrPos[2], terrNorm[2], terrPos[3], terrNorm[3], planeHeight, intersection[7] );

                // ���ӽ���
                linkIntersection( intersection[0], intersection[1], mShoreEdges );
                linkIntersection( intersection[0], intersection[5], mShoreEdges );
                linkIntersection( intersection[1], intersection[5], mShoreEdges );
                linkIntersection( intersection[0], intersection[2], mShoreEdges );
                linkIntersection( intersection[0], intersection[4], mShoreEdges );
                linkIntersection( intersection[2], intersection[4], mShoreEdges );
                linkIntersection( intersection[1], intersection[3], mShoreEdges );
                linkIntersection( intersection[1], intersection[6], mShoreEdges );
                linkIntersection( intersection[3], intersection[6], mShoreEdges );
                linkIntersection( intersection[2], intersection[3], mShoreEdges );
                linkIntersection( intersection[2], intersection[7], mShoreEdges );
                linkIntersection( intersection[3], intersection[7], mShoreEdges );
            }
        }

        //
        // ���ӽ��߼���Ϊһ�������������
        //
        U32 shoreEdgesCnt = mShoreEdges.size();
        U32 tempEdgesCnt = smTempEdges.capacity();
        if ( shoreEdgesCnt > tempEdgesCnt )
        {
            while ( tempEdgesCnt < shoreEdgesCnt )
            {
                if ( tempEdgesCnt == 0 )
                    tempEdgesCnt = 400;
                else
                    tempEdgesCnt *= 2;
            }
            smTempEdges.reserve( tempEdgesCnt );
        }
        smTempEdges.set( mShoreEdges.address(), shoreEdgesCnt );

        while ( smTempEdges.size() )
        {
            // ����һ��������, ���뵱ǰ���߶μ��е�һ�����߶�
            mShoreLines.push_back(ShoreLine());
            ShoreLine& shoreLine = mShoreLines[mShoreLines.size()-1];
            shoreLine.push_back( smTempEdges.first().start );
            shoreLine.push_back( smTempEdges.first().end );

            // ���ý��߶δӵ�ǰ���߶μ����Ƴ�
            smTempEdges.erase_fast( (U32)0 );

            // ������ǰ���߶μ���ʣ�µĽ��߶�, ������ڿ����ӵ���ǰ��������˻��Ҷ˵Ľ��߶�, �򽫸ý��߶μ��뵽��ǰ������, ����
            // ��ӵ�ǰ���߶μ����Ƴ�
            // ѭ���ù���, ֱ����ǰ���߶μ������޿������ӵ���ǰ�����ߵĽ��߶�, ��������������������, ѭ���˳�, ׼��������һ��
            for ( Vector<ShoreEdge>::iterator itrEdges = smTempEdges.begin(); itrEdges != smTempEdges.end(); )
            {
                ShoreEdge& edge = *itrEdges;

                if ( ( edge.start.pos - shoreLine.front().pos ).lenSquared() < 1.0e-8f )
                    shoreLine.push_front( edge.end );
                else if ( ( edge.end.pos - shoreLine.front().pos ).lenSquared() < 1.0e-8f )
                    shoreLine.push_front( edge.start );
                else if ( ( edge.start.pos - shoreLine.back().pos ).lenSquared() < 1.0e-8f )
                    shoreLine.push_back( edge.end );
                else if ( ( edge.end.pos - shoreLine.back().pos ).lenSquared() < 1.0e-8f )
                    shoreLine.push_back( edge.start );
                else
                {
                    ++itrEdges;
                    continue;
                }

                smTempEdges.erase_fast( itrEdges );
                itrEdges = smTempEdges.begin();
            }

            // �Ը����ɵ����������߽���һ������
            ShoreVert prevShoreVert;
            for ( ShoreLine::iterator itrLine = shoreLine.begin(); itrLine != shoreLine.end(); )
            {
                ShoreVert& currSurfVert = *itrLine;
                if ( itrLine == shoreLine.begin() )
                    currSurfVert.disToHead = 0.0f;
                else
                {
                    F32 disToPrev = ( currSurfVert.pos - prevShoreVert.pos ).len();
                    if ( disToPrev != 0.0f )
                        currSurfVert.disToHead = disToPrev + prevShoreVert.disToHead;
                    else
                    {
                        itrLine = shoreLine.erase( itrLine );
                        continue;
                    }
                }

                prevShoreVert = currSurfVert;
                ++itrLine;
            }
        }

        mShoreLineNeedUpdate = false;
        return true;
    }
    else
        return false;
}

bool ShoreSurf::buildShoreVB()
{
    bool shoreLineUpdated = findShoreLine();
    if ( shoreLineUpdated )
        mShoreVBNeedUpdate = true;

    if ( mShoreSurfEnable && mShoreVBNeedUpdate )
    {
        // �������к����ߣ����ɶ��㻺��, ���Ǵ�����, ÿ���˻���ÿ��������֮�����˻�����������
        U32 shoreVertsCnt = mShoreEdges.size() * 8;
        U32 tempVertsCnt = smTempVerts.capacity();
        if ( shoreVertsCnt > tempVertsCnt )
        {
            while ( tempVertsCnt < shoreVertsCnt )
            {
                if ( tempVertsCnt == 0 )
                    tempVertsCnt = 400;
                else
                    tempVertsCnt *= 2;
            }
            smTempVerts.reserve( tempVertsCnt );
        }

        U32 numVerts = 0;
        mRandGen.setSeed( mSurfRandSeed ); // �̶��������, ȷ��������������б��ֱ���һ��
        for ( U32 i = 0; i < mShoreLines.size(); i++ )
        {
            ShoreLine& shoreLine = mShoreLines[i];
            if ( shoreLine.size() < 2 )
                continue;
            
            U32 j = 0;
            while ( j < shoreLine.size() - 1 )
            {
                // ��ʼ��ÿ���˻�����
                F32 shoreHeight = mShoreHeight;
                F32 surfWidth = mSurfWidth + mRandGen.randF( -mSurfWidthRand, mSurfWidthRand );
                F32 surfHeight = mSurfHeight + mRandGen.randF( -mSurfHeightRand, mSurfHeightRand );
                F32 surfPos = mSurfPos + mRandGen.randF( -mSurfPosRand, mSurfPosRand );
                F32 surfSpeed = mSurfSpeed + mRandGen.randF( -mSurfSpeedRand, mSurfSpeedRand );
                F32 surfAlpha = mSurfAlpha + mRandGen.randF( -mSurfAlphaRand, mSurfAlphaRand );
                F32 surfMiddle = mSurfMiddle + mRandGen.randF( -mSurfMiddleRand, mSurfMiddleRand );

                // �ӵ�ǰ���������ҵ�һ��������������ĵ�
                U32 beginShoreVert = j;
                j++;                
                while ( j < shoreLine.size() - 1 && shoreLine[j].disToHead - shoreLine[beginShoreVert].disToHead < surfWidth )
                    j++;
                U32 endShoreVert = j;

                // �˻�������������, ���� 6, 7, 14, 15 Ϊ�˻������ζ���, �����˻�֮������
                // *(0)  *(1)        *(7) *(8)   *(9)          *(15) ...
                // *(2)  *(3)             *(10)  *(11)               ...
                // *(4)  *(5) *(6)        *(12)  *(13) *(14)         ...
                GFXVertexShoreSurf verts[6];
                
                Point3F extDir;
                extDir = shoreLine[beginShoreVert].norm + shoreLine[endShoreVert].norm;
                extDir.normalize();

                verts[0].point = shoreLine[beginShoreVert].pos;
                verts[1].point = shoreLine[endShoreVert].pos;
                verts[4].point = shoreLine[beginShoreVert].pos + extDir * surfHeight - Point3F( 0.0f, 0.0f, shoreHeight );
                verts[5].point = shoreLine[endShoreVert].pos + extDir * surfHeight - Point3F( 0.0f, 0.0f, shoreHeight );
                verts[2].point = verts[0].point + surfMiddle * ( verts[4].point - verts[0].point ); 
                verts[3].point = verts[1].point + surfMiddle * ( verts[5].point - verts[1].point );

                verts[0].texCoord.x = 0.0f;
                verts[0].texCoord.y = 0.0f;
                verts[0].params.z = 0.0f;

                verts[1].texCoord.x = 1.0f;
                verts[1].texCoord.y = 0.0f;
                verts[1].params.z = 0.0f;

                verts[2].texCoord.x = 0.0f;
                verts[2].texCoord.y = surfMiddle;
                verts[2].params.z = surfAlpha;

                verts[3].texCoord.x = 1.0f;
                verts[3].texCoord.y = surfMiddle;
                verts[3].params.z = surfAlpha;

                verts[4].texCoord.x = 0.0f;
                verts[4].texCoord.y = 1.0f;
                verts[4].params.z = 0.0f;

                verts[5].texCoord.x = 1.0f;
                verts[5].texCoord.y = 1.0f;
                verts[5].params.z = 0.0f;

                for ( U32 m = 0; m < 6; m++ )
                {
                    verts[m].params.x = surfPos;
                    verts[m].params.y = surfSpeed;               
                }

                GFXVertexShoreSurf* pTempVerts = smTempVerts.address();

                // �˻������ζ���
                if ( numVerts > 0 )
                {
                    pTempVerts[numVerts] = pTempVerts[numVerts-1];
                    numVerts++;
                    pTempVerts[numVerts] = verts[0];
                    numVerts++;  
                }

                // �˻�����
                for ( U32 n = 0; n < 6; n++ )
                {
                    pTempVerts[numVerts] = verts[n];
                    numVerts++;                    
                }				
            }
        }
        
        if ( numVerts )
        {
            mVB.set( GFX, numVerts, GFXBufferTypeStatic );
            GFXVertexShoreSurf *pVB = mVB.lock();
            dMemcpy( pVB, smTempVerts.address(), sizeof(GFXVertexShoreSurf) * numVerts );
            mVB.unlock();
        }
        else
            mVB = NULL;

        mShoreVBNeedUpdate = false;
    }
    
    return ( mVB && mVB->mNumVerts > 2 );
}


void ShoreSurf::update()
{
    if ( mShoreSurfEnable )
    {
        initResource();
        buildShoreVB();
    }
}

void ShoreSurf::render( PASS_TYPE passType )
{
    update();

    if ( mShoreSurfEnable && mVB && mVB->mNumVerts )
    {
        // ���ò���
        MatrixF oldViewMat = GFX->getWorldMatrix();
        MatrixF oldProjMat = GFX->getProjectionMatrix();

        MatrixF worldMat = mTerrainBlock->getRenderTransform();
        GFX->multWorld( worldMat );

        MatrixF projMat = GFX->getProjectionMatrix();
        projMat.mul(GFX->getWorldMatrix());
        projMat.transpose();
        GFX->setVertexShaderConstF( 0, (float*)&projMat, 4 );

        GFX->setCullMode( GFXCullNone );        
        GFX->setZEnable( true );
        GFX->setZWriteEnable( false ); 
        GFX->setAlphaTestEnable(true);
        GFX->setAlphaFunc(GFXCmpGreaterEqual);
        GFX->setAlphaRef(2);
        GFX->setAlphaBlendEnable( true );
        GFX->setSrcBlend( GFXBlendSrcAlpha );
        GFX->setDestBlend( GFXBlendInvSrcAlpha );

        F32 depthBias = -0.00001f;
        GFX->setZBias( *( (U32 *)&depthBias ) );
        
        if ( passType == PassType_Mask )
        {
            smMaskShader->process();
            mPrevPassIsMask = true;
        }
        else if ( passType ==  PassType_Surf )
        {
            smShader->process();
            if ( mPrevPassIsMask )
            {
                GFX->setSrcBlend( GFXBlendSrcAlpha );
                GFX->setDestBlend( GFXBlendOne );
                mPrevPassIsMask = false;
            }
        }

        F32 reg[4];
        reg[0] = (F32)Platform::getVirtualMilliseconds() * 0.001f;
        GFX->setVertexShaderConstF( 60, (float*)reg, 1, 1 );         

        GFX->setTexture( 0, mSurfTex );    
        GFX->setTextureStageAddressModeU( 0, GFXAddressWrap );
        GFX->setTextureStageAddressModeV( 0, GFXAddressWrap );
        GFX->setTextureStageMagFilter( 0, GFXTextureFilterLinear );
        GFX->setTextureStageMinFilter( 0, GFXTextureFilterLinear );
        GFX->setTextureStageMipFilter( 0, GFXTextureFilterLinear );

        MatrixF objTrans = worldMat;
        objTrans.transpose();
        GFX->setVertexShaderConstF( VC_OBJ_TRANS, (float*)&objTrans, 4 );           

        Point3F eyePos = gClientSceneGraph->getBaseCameraPosition();
        objTrans = worldMat;
        objTrans.inverse();
        objTrans.mulP( eyePos );
        eyePos.convolveInverse(objTrans.getScale());
        GFX->setVertexShaderConstF( VC_EYE_POS, (float*)&eyePos, 1, 3 );

        reg[0] = gClientSceneGraph->getFogHeightOffset();
        reg[1] = gClientSceneGraph->getFogInvHeightRange(); 
        reg[2] = gClientSceneGraph->getVisibleDistanceMod(); 
        GFX->setVertexShaderConstF( 22, reg, 1, 3 );

        GFX->setTexture( 1, gClientSceneGraph->getFogTexture() );
        GFX->setTextureStageAddressModeU( 1, GFXAddressClamp );
        GFX->setTextureStageAddressModeV( 1, GFXAddressClamp );
        GFX->setTextureStageMagFilter( 1, GFXTextureFilterLinear );
        GFX->setTextureStageMinFilter( 1, GFXTextureFilterLinear );
        GFX->setTextureStageMipFilter( 1, GFXTextureFilterLinear );

        // ��Ⱦ
        GFX->setVertexBuffer( mVB );    
        GFX->drawPrimitive( GFXTriangleStrip, 0, mVB->mNumVerts - 2 );

        GFX->setZBias( 0 );
        GFX->setWorldMatrix( oldViewMat );
        GFX->setProjectionMatrix( oldProjMat );
    }
}