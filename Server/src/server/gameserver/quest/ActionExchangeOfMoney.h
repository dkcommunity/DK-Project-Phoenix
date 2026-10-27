//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionExchangeOfMoney.h
// Reconstructed by AI due to corruption
//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_EXCHANGE_OF_MONEY_H__
#define __ACTION_EXCHANGE_OF_MONEY_H__

#include "Action.h"
#include "ActionFactory.h"
#include "Exception.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class ActionExchangeOfMoney
//////////////////////////////////////////////////////////////////////////////

class ActionExchangeOfMoney : public Action {
public:
  ActionExchangeOfMoney();
  virtual ~ActionExchangeOfMoney();

  virtual ActionType_t getActionType() const {
    return ACTION_EXCHANGE_OF_MONEY;
  }
  virtual void read(PropertyBuffer &propertyBuffer);
  virtual void execute(Creature *pCreature1, Creature *pCreature2 = NULL);
  virtual string toString() const;

private:
  int m_ExchangeType;
  Gold_t m_CheckMoney;
};

//////////////////////////////////////////////////////////////////////////////
// class ActionExchangeOfMoneyFactory;
//////////////////////////////////////////////////////////////////////////////

class ActionExchangeOfMoneyFactory : public ActionFactory {
public:
  virtual ActionType_t getActionType() const {
    return Action::ACTION_EXCHANGE_OF_MONEY;
  }
  virtual string getActionName() const { return "ExchangeOfMoney"; }
  virtual Action *createAction() const { return new ActionExchangeOfMoney(); }
};

#endif
