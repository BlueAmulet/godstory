//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
// Define the field types for objects that link to the namespace MissionInfo
function SimObject::onDefineFieldTypes( %this )
{
   %this.setFieldType("Locked", "TypeBool");   
}