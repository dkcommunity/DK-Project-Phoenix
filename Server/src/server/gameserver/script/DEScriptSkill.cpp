#include "DEScriptSkill.h"

DEScriptSkill::DEScriptSkill() {
  lua_tinker::class_add<DEScriptSkill>(m_pLuaState, "DEScriptSkill");
  lua_tinker::class_def<DEScriptSkill>(m_pLuaState, "AddMask",
                                       &DEScriptSkill::AddMask);

  lua_tinker::set(m_pLuaState, "DEScriptSkill", this);
}

DEScriptSkill::~DEScriptSkill() {}

void DEScriptSkill::AddMask(lua_tinker::table SimpleSkillInputInfo, int x,
                            int y, int penalty) {
  SIMPLE_SKILL_INPUT *pSimpleSkillInput;

  pSimpleSkillInput =
      SimpleSkillInputInfo.get<SIMPLE_SKILL_INPUT *>("_pSimpleSkillInput");
  pSimpleSkillInput->addMask(x, y, penalty);
}

void DEScriptSkill::NotifyInitialize() {
  try {
    lua_tinker::call<void>(m_pLuaState, "OnInitialize");
  } catch (DEScriptNS::CallException &e) {
    // 스크립트에 해당 함수가 없는 경우

    // do nothing
  }
}

void DEScriptSkill::NotifyComputOutput(const SkillInput &input,
                                       SkillOutput &output) {
  try {
    lua_tinker::table InputInfo(m_pLuaState);
    lua_tinker::table OutputInfo(m_pLuaState);

    InputInfo.set<int>("SkillLevel", input.SkillLevel);
    InputInfo.set<int>("DomainLevel", input.DomainLevel);
    InputInfo.set<int>("STR", input.STR);
    InputInfo.set<int>("DEX", input.DEX);
    InputInfo.set<int>("INTE", input.INTE);
    InputInfo.set<int>("TargetType", input.TargetType);
    InputInfo.set<int>("Range", input.Range);
    InputInfo.set<int>("IClass", input.IClass);
    InputInfo.set<int>("PartySize", input.PartySize);
    InputInfo.set<int>("Distance", input.Distance);

    OutputInfo.set<int>("Distance", output.Damage);
    OutputInfo.set<int>("Duration", output.Duration);
    OutputInfo.set<int>("Duration", output.Tick);
    OutputInfo.set<int>("ToHit", output.ToHit);
    OutputInfo.set<int>("Range", output.Range);
    OutputInfo.set<int>("Delay", output.Delay);

    lua_tinker::call<void>(m_pLuaState, "OnComputeOuput", InputInfo,
                           OutputInfo);

    output.Damage = OutputInfo.get<int>("Damage");
    output.Duration = OutputInfo.get<int>("Duration");
    output.Tick = OutputInfo.get<int>("Tick");
    output.ToHit = OutputInfo.get<int>("ToHit");
    output.Range = OutputInfo.get<int>("Range");
    output.Delay = OutputInfo.get<int>("Delay");
  } catch (DEScriptNS::CallException &e) {
    // 스크립트에 해당 함수가 없는 경우

    // do nothing
  }
}

void DEScriptSkill::NotifyExecuteSkill(SIMPLE_SKILL_INPUT &simpleSkillInput) {
  try {
    lua_tinker::table SkillInputInfo(m_pLuaState);

    SkillInputInfo.set<SIMPLE_SKILL_INPUT *>("_pSimpleSkillInput",
                                             &simpleSkillInput);

    lua_tinker::call<void>(m_pLuaState, "OnExecuteSkill", SkillInputInfo);

    simpleSkillInput.bMagicHitRoll = SkillInputInfo.get<bool>("bMagicHitRoll");
    simpleSkillInput.bMagicDamage = SkillInputInfo.get<bool>("bMagicDamage");
    simpleSkillInput.bAdd = SkillInputInfo.get<bool>("bAdd");

    simpleSkillInput.STRMultiplier = SkillInputInfo.get<int>("STRMultiplier");
    simpleSkillInput.DEXMultiplier = SkillInputInfo.get<int>("DEXMultiplier");
    simpleSkillInput.INTMultiplier = SkillInputInfo.get<int>("INTMultiplier");
    //	simpleSkillInput.ItemClass      =
    //SkillInputInfo.get<Item::ItemClass>("ItemClass");

    simpleSkillInput.Level = SkillInputInfo.get<int>("Level");
    simpleSkillInput.bExpForTotalDamage =
        SkillInputInfo.get<bool>("bExpForTotalDamage");
    simpleSkillInput.Grade = SkillInputInfo.get<int>("Grade");
  } catch (DEScriptNS::CallException &e) {
    // 스크립트에 해당 함수가 없는 경우

    // do nothing
  }
}
