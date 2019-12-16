//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "sim/netConnection.h"

#include "lightingSystem/synapseGaming/sgMissionLightingFilter.h"
#include "lightingSystem/synapseGaming/sgLighting.h"


IMPLEMENT_CO_DATABLOCK_V1(sgMissionLightingFilterData);
IMPLEMENT_CO_NETOBJECT_V1(sgMissionLightingFilter);

void sgMissionLightingFilterData::initPersistFields()
{
	Parent::initPersistFields();
	
	addField("CinematicFilter", TypeBool, Offset(sgCinematicFilter, sgMissionLightingFilterData));
	addField("LightingIntensity", TypeF32, Offset(sgLightingIntensity, sgMissionLightingFilterData));
	addField("LightingFilter", TypeColorF, Offset(sgLightingFilter, sgMissionLightingFilterData));
	addField("CinematicFilterAmount", TypeF32, Offset(sgCinematicFilterAmount, sgMissionLightingFilterData));
	addField("CinematicFilterReferenceIntensity", TypeF32, Offset(sgCinematicFilterReferenceIntensity, sgMissionLightingFilterData));
	addField("CinematicFilterReferenceColor", TypeColorF, Offset(sgCinematicFilterReferenceColor, sgMissionLightingFilterData));
}

void sgMissionLightingFilterData::packData(BitStream *stream)
{
	Parent::packData(stream);
	
	stream->write(sgCinematicFilter);
	stream->write(sgLightingIntensity);
	stream->write(sgLightingFilter);
	stream->write(sgCinematicFilterAmount);
	stream->write(sgCinematicFilterReferenceIntensity);
	stream->write(sgCinematicFilterReferenceColor);
}

void sgMissionLightingFilterData::unpackData(BitStream *stream)
{
	Parent::unpackData(stream);
	
	stream->read(&sgCinematicFilter);
	stream->read(&sgLightingIntensity);
	stream->read(&sgLightingFilter);
	stream->read(&sgCinematicFilterAmount);
	stream->read(&sgCinematicFilterReferenceIntensity);
	stream->read(&sgCinematicFilterReferenceColor);
}

void sgMissionLightingFilter::initPersistFields()
{
	Parent::initPersistFields();
}

void sgMissionLightingFilter::inspectPostApply()
{
	Parent::inspectPostApply();
	setMaskBits(0xffffffff);
}

bool sgMissionLightingFilter::onAdd()
{
   if(!Parent::onAdd())
      return false;
	  
	mObjBox.min.set( -0.5, -0.5, -0.5 );
	mObjBox.max.set( 0.5, 0.5, 0.5 );
	resetWorldBox();
	setRenderTransform(mObjToWorld);

   SimSet *filters = Sim::getsgMissionLightingFilterSet();
   if(filters && isClientObject())
      filters->addObject(this);
   
   addToScene();
   return true;
}

void sgMissionLightingFilter::onRemove()
{
   SimSet *filters = Sim::getsgMissionLightingFilterSet();
   if(filters && isClientObject())
      filters->removeObject(this);

   removeFromScene();
   Parent::onRemove();
}

U64 sgMissionLightingFilter::packUpdate(NetConnection *con, U64 mask, BitStream *stream)
{
	U64 res = Parent::packUpdate(con, mask, stream);
	
	stream->writeAffineTransform(mObjToWorld);
	
	return res;
}

void sgMissionLightingFilter::unpackUpdate(NetConnection *con, BitStream *stream)
{
	Parent::unpackUpdate(con, stream);
	
	MatrixF ObjectMatrix;
	stream->readAffineTransform(&ObjectMatrix);
	setTransform(ObjectMatrix);
}


