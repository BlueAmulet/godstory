//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SGLIGHTINGFEATURES_H_
#define _SGLIGHTINGFEATURES_H_

#include "shaderGen/shaderFeature.h"
#include "shaderGen/shaderOp.h"
#include "materials/matInstance.h"
#include SHADER_CONSTANT_INCLUDE_FILE


class ExposureFeatureX2 : public ShaderFeature
{
protected:
   F32 amount;
public:
   ExposureFeatureX2() {amount = 2.0;}
   void processPix(Vector<ShaderComponent*> &componentList, 
      GFXShaderFeatureData &fd)
   {
      Var *color = (Var*)LangElement::find("col");
      if(color)
      {
         char temp[256];
         dSprintf(temp, sizeof(temp), "   @ *= %f;\r\n", amount);
         output = new GenOp(temp, color);
      }
      else
      {
         // no color?  then at least call the default, because a NULL output == crash...
         ShaderFeature::processPix(componentList, fd);
      }
   }
   virtual Material::BlendOp getBlendOp() {return Material::None;}
};

class ExposureFeatureX4 : public ExposureFeatureX2
{
public:
   ExposureFeatureX4() {amount = 4.0;}
};

class DynamicLightingFeature : public ShaderFeature
{
public:
   void processPix( Vector<ShaderComponent*> &componentList, 
                    GFXShaderFeatureData &fd)
   {
      MultiLine *multi = new MultiLine;

      // Find color and reset the alpha value to be 1.0.  This assumes that the color
      // has been set to the diffuse texture color prior to this operation.
      // This is here to prevent the dynamic light from being modulated by the the
      // diffuse texture's alpha value.
      Var *color = (Var*) LangElement::find( "col" );
      if( color )
      {
         multi->addStatement( new GenOp( "   @.a = 1.0;\r\n", color ) );
      }
      
      ConnectorStruct *connectComp = dynamic_cast<ConnectorStruct *>(componentList[C_CONNECTOR]);
      Var *dlightshading = NULL;
      if(!fd.features[GFXShaderFeatureData::BumpMap])
      {
         dlightshading = connectComp->getElement(RT_COLOR);
         dlightshading->setName("dlightshading");
         dlightshading->setStructName("IN");
         dlightshading->setType("float4");
      }
      
      // grab connector texcoord register
      Var *inTex = connectComp->getElement(RT_TEXCOORD);
      inTex->setName("dlightCoord");
      inTex->setStructName("IN");
      inTex->setType("float4");
      inTex->mapsToSampler = true;

      // create texture var
      Var *dlightMap = new Var;
      dlightMap->setType("sampler3D");
      dlightMap->setName("dlightMap");
      dlightMap->uniform = true;
      dlightMap->sampler = true;
      dlightMap->constNum = Var::getTexUnitNum();

      Var *lightcolor = new Var;
      lightcolor->setType("float4");
      lightcolor->setName("lightColor");
      lightcolor->uniform = true;
      lightcolor->constNum = PC_DIFF_COLOR;
      
      Var *attnvar = new Var();
      attnvar->setName("attn");
      attnvar->setType("float4");
      LangElement *attn = new DecOp(attnvar);

      if(dlightshading)
         multi->addStatement(new GenOp("   @ = tex3D(@, @) * @ * @.x;\r\n", attn, dlightMap, inTex, lightcolor, dlightshading));
      else
         multi->addStatement(new GenOp("   @ = tex3D(@, @) * @;\r\n", attn, dlightMap, inTex, lightcolor));

      if((fd.features[GFXShaderFeatureData::DynamicLightMask]) &&
         (!fd.features[GFXShaderFeatureData::DynamicLightDual]))
      {
         // create mask var
         Var *dlightMask = new Var;
         dlightMask->setType("samplerCUBE");
         dlightMask->setName("dlightMask");
         dlightMask->uniform = true;
         dlightMask->sampler = true;
         dlightMask->constNum = Var::getTexUnitNum();

         multi->addStatement(new GenOp("   @ *= texCUBE(@, (@.xzy - 0.5));\r\n", attnvar, dlightMask, inTex));
      }

      if(fd.features[GFXShaderFeatureData::DynamicLightDual])
      {
         Var *inTexsec = connectComp->getElement(RT_TEXCOORD);
         inTexsec->setName("dlightCoordSec");
         inTexsec->setStructName("IN");
         inTexsec->setType("float4");
         inTexsec->mapsToSampler = true;

         Var *dlightMapsec = new Var;
         dlightMapsec->setType( "sampler3D" );
         dlightMapsec->setName( "dlightMapSec" );
         dlightMapsec->uniform = true;
         dlightMapsec->sampler = true;
         dlightMapsec->constNum = Var::getTexUnitNum();

         Var *lightcolorsec = new Var;
         lightcolorsec->setType("float4");
         lightcolorsec->setName("lightColorSec");
         lightcolorsec->uniform = true;
         lightcolorsec->constNum = PC_DIFF_COLOR2;

         Var *attnsecvar = new Var();
         attnsecvar->setName("attnsec");
         attnsecvar->setType("float4");
         LangElement *attnsec = new DecOp(attnsecvar);

         if(dlightshading)
            multi->addStatement(new GenOp("   @ = tex3D(@, @) * @ * @.w;\r\n", attnsec, dlightMapsec, inTexsec, lightcolorsec, dlightshading));
         else
            multi->addStatement(new GenOp("   @ = tex3D(@, @) * @;\r\n", attnsec, dlightMapsec, inTexsec, lightcolorsec));

         if(fd.features[GFXShaderFeatureData::BumpMap])
            multi->addStatement(new GenOp("   @;\r\n", assignColor(attnvar)));// secondary gets added later...
         else
         {
            LangElement *attntotal = new GenOp("(@*4 + @*4)", attnvar, attnsecvar);  //Ray: ¼ÓÁÁµã¹â
            multi->addStatement(new GenOp("   @;\r\n", assignColor(attntotal)));
         }
      }
      else
	  {
		  LangElement *attntotal = new GenOp("(@*4)", attnvar);
         multi->addStatement(new GenOp("   @;\r\n", assignColor(attntotal)));
	  }

      output = multi;
   }
   void processVert( Vector<ShaderComponent*> &componentList, 
      GFXShaderFeatureData &fd )
   {
      // grab tex register from incoming vert
      Var *inpos = (Var*)LangElement::find("position");
      Var *innorm = (Var*)LangElement::find("normal");

      ConnectorStruct *connectComp = dynamic_cast<ConnectorStruct *>(componentList[C_CONNECTOR]);
      Var *dlightshading = NULL;
      if(!fd.features[GFXShaderFeatureData::BumpMap])
      {
         dlightshading = connectComp->getElement(RT_COLOR);
         dlightshading->setName("dlightshading");
         dlightshading->setStructName("OUT");
         dlightshading->setType("float4");
      }

      // grab connector texcoord register
      Var *outTex = connectComp->getElement(RT_TEXCOORD);
      outTex->setName("dlightCoord");
      outTex->setStructName("OUT");
      outTex->setType("float4");
      outTex->mapsToSampler = true;

      // grab light position var
      Var *lightpos = new Var;
      lightpos->setType("float4");
      lightpos->setName("lightPos");
      lightpos->uniform = true;
      lightpos->constNum = VC_LIGHT_POS1;

      Var *objTrans = new Var;
      objTrans->setType("float4x4");
      objTrans->setName("objTrans");
      objTrans->uniform = true;
      objTrans->constNum = VC_OBJ_TRANS;

      Var *lighting = new Var;
      lighting->setType("float4x4");
      lighting->setName("lightingMatrix");
      lighting->uniform = true;
      lighting->constNum = VC_LIGHT_TRANS;

      MultiLine *multi = new MultiLine;
      Var *lightdir = new Var;
      lightdir->setType("float3");
      lightdir->setName("lightdir");
      LangElement *lightdirdec = new DecOp(lightdir);

      Var *worldpos = new Var;
      worldpos->setType("float3");
      worldpos->setName("worldpos");
      LangElement *worldposdec = new DecOp(worldpos);

      multi->addStatement(new GenOp("\r\n"));
      multi->addStatement(new GenOp("   @;\r\n", lightdirdec));
      multi->addStatement(new GenOp("   @ = mul(@, @.xyz);\r\n",
         worldposdec, objTrans, inpos));
      multi->addStatement(new GenOp("   @ = @ - mul(@, @.xyz);\r\n",
         lightdir, worldpos, objTrans, lightpos));
      multi->addStatement(new GenOp("   @.xyz = @ * @.w;\r\n",
         outTex, lightdir, lightpos));
      multi->addStatement(new GenOp("   @.xyz = mul(@, @.xyz) + 0.5;\r\n", outTex, lighting, outTex));

      Var *worldnorm = NULL;
      if(dlightshading)
      {
         multi->addStatement(new GenOp("   @.w = 1.0;\r\n\r\n", outTex));

         if(!innorm)
            multi->addStatement(new GenOp("   @.xyz = 1.0;\r\n", dlightshading));
         else
         {
            worldnorm = new Var;
            worldnorm->setType("float3");
            worldnorm->setName("worldnorm");
            LangElement *worldnormdec = new DecOp(worldnorm);

            multi->addStatement(new GenOp("   @ = normalize(mul((float3x3)@, @));\r\n",
               worldnormdec, objTrans, innorm));
            multi->addStatement(new GenOp("   @.xyz = saturate(dot(normalize(-@), @));\r\n",
               dlightshading, lightdir, worldnorm));
         }
      }

      if(fd.features[GFXShaderFeatureData::DynamicLightDual])
      {
         Var *outTexsec = connectComp->getElement(RT_TEXCOORD);
         outTexsec->setName("dlightCoordSec");
         outTexsec->setStructName("OUT");
         outTexsec->setType("float4");
         outTexsec->mapsToSampler = true;

         // grab light position var
         Var *lightpossec = new Var;
         lightpossec->setType("float4");
         lightpossec->setName("lightPosSec");
         lightpossec->uniform = true;
         lightpossec->constNum = VC_LIGHT_POS2;

         Var *lightingsec = new Var;
         lightingsec->setType("float4x4");
         lightingsec->setName("lightingMatrixSec");
         lightingsec->uniform = true;
         lightingsec->constNum = VC_LIGHT_TRANS2;

         multi->addStatement(new GenOp("\r\n"));
         multi->addStatement(new GenOp("   @ = @ - mul(@, @.xyz);\r\n",
            lightdir, worldpos, objTrans, lightpossec));
         multi->addStatement(new GenOp("   @.xyz = @ * @.w;\r\n",
            outTexsec, lightdir, lightpossec));
         multi->addStatement(new GenOp("   @.xyz = mul(@, @.xyz) + 0.5;\r\n", outTexsec, lightingsec, outTexsec));

         if(dlightshading)
         {
            multi->addStatement(new GenOp("   @.w = 1.0;\r\n", outTexsec));

            if(!worldnorm)
               multi->addStatement(new GenOp("   @.w = 1.0;\r\n", dlightshading));
            else
            {
               multi->addStatement(new GenOp("   @.w = saturate(dot(normalize(-@), @));\r\n",
                  dlightshading, lightdir, worldnorm));
            }
         }
      }
      else if(dlightshading)
      {
         multi->addStatement(new GenOp("   @.w = 1.0;\r\n", dlightshading));
      }

      output = multi;
   }
   ShaderFeature::Resources getResources(GFXShaderFeatureData &fd)
   {
      Resources res;
      if(fd.features[GFXShaderFeatureData::DynamicLightDual])
      {
         res.numTex = 2;
         res.numTexReg = 2;
      }
      else
      {
         if(fd.features[GFXShaderFeatureData::DynamicLightMask])
         {
            res.numTex = 2;
            res.numTexReg = 1;
         }
         else
         {
            res.numTex = 1;
            res.numTexReg = 1;
         }
      }
      return res;
   }
   void setTexData(Material::StageData &stageDat, GFXShaderFeatureData &fd,
      RenderPassData &passData, U32 &texIndex)
   {
      passData.texFlags[texIndex++] = Material::DynamicLight;
      if(fd.features[GFXShaderFeatureData::DynamicLightDual])
         passData.texFlags[texIndex++] = Material::DynamicLightSecondary;
      else if(fd.features[GFXShaderFeatureData::DynamicLightMask])
         passData.texFlags[texIndex++] = Material::DynamicLightMask;
   }
   virtual Material::BlendOp getBlendOp() {return Material::None;}
};

class DynamicLightingDummyFeature : public ShaderFeature
{
public:
   virtual Material::BlendOp getBlendOp() {return Material::None;}
};

class SelfIlluminationFeature : public ShaderFeature
{
public:
   void processPix(Vector<ShaderComponent*> &componentList, 
      GFXShaderFeatureData &fd)
   {
      Var *lightcolor = new Var;
      lightcolor->setType("float4");
      lightcolor->setName("lightColor");
      lightcolor->uniform = true;
      lightcolor->constNum = PC_DIFF_COLOR;

      output = new GenOp("   @;\r\n", assignColor(lightcolor));
   }
   virtual Material::BlendOp getBlendOp() {return Material::None;}
};


#endif//_SGLIGHTINGFEATURES_H_
