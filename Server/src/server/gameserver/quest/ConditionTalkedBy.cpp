//////////////////////////////////////////////////////////////////////////////
// Filename    : ConditionTalkedBy.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ConditionTalkedBy.h"

void ConditionTalkedBy::read(PropertyBuffer &propertyBuffer) {
  __BEGIN_TRY

  try {
    // Read Script Type
    m_ScriptType = propertyBuffer.getPropertyInt("ScriptType");
  } catch (NoSuchElementException &nsee) {
    m_ScriptType = 0;
  }

  __END_CATCH
}

bool ConditionTalkedBy::isSatisfied(Creature *pNPC, Creature *pPC,
                                    void *pParam) const {
  int ScriptType;
  // ScriptType = (int)pParam;
  ScriptType = (unsigned long)pParam;

  return pNPC != NULL && pNPC->isNPC() && pPC != NULL && pPC->isPC() &&
                 m_ScriptType == 0
             ? true
             : m_ScriptType == ScriptType;
}
