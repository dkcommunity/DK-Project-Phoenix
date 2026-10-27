////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionInitInstanceDoungeon.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionInitInstanceDoungeon.h"
#include "NPC.h"

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionInitInstanceDoungeon::read (PropertyBuffer & propertyBuffer)
{
	__BEGIN_TRY

	try
	{
		m_strDoungeonScript = propertyBuffer.getProperty("DoungeonScript");
	}
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionInitInstanceDoungeon::execute (Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	try {
		Assert(pCreature1 != NULL);
		Assert(pCreature1->isNPC());

		NPC* pNPC = dynamic_cast<NPC*>(pCreature1);
		
		pNPC->setInstanceDoungeonScript(m_strDoungeonScript);

	} catch (Throwable &t) {
		cout << t.toString() << endl;
		throw; // -_-;
	}
	
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionInitInstanceDoungeon::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionInitInstanceDoungeon("
		<< "DoungeonScript:" << m_strDoungeonScript
		<< ")";
	return msg.toString();

	__END_CATCH
}
