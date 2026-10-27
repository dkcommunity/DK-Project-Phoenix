//---------------------------------------------------------------------------------
// ExperienceTable.h
//---------------------------------------------------------------------------------
// 이미 화일이 존재하고.. 거기서 읽어온다고 가정했기 때문에.. 
// save함수는 없당..
//---------------------------------------------------------------------------------

#ifndef __EXPERIENCETABLE_H__
#define __EXPERIENCETABLE_H__

#include <Windows.h>
#include "ExpInfo.h"
#include "RaceType.h"
std::ifstream;

//---------------------------------------------------------------------------------
// ExperienceTable
//---------------------------------------------------------------------------------
class ExperienceTable {

	public :
		ExperienceTable();
		~ExperienceTable();
		
		//----------------------------------------------------------------
		// Release
		//----------------------------------------------------------------
		void				Release();

		//----------------------------------------------------------------
		// Get Status Info
		//----------------------------------------------------------------
		const ExpInfo&		GetSTRInfo(int level) const;
		const ExpInfo&		GetDEXInfo(int level) const;
		const ExpInfo&		GetINTInfo(int level) const;
		const ExpInfo&		GetVampireInfo(int level) const;
		const ExpInfo&		GetOustersInfo(int level) const;
		const ExpInfo&		GetRankInfo(int level, Race_t race) const;
		const ExpInfo&		GetSlayerRankInfo(int level) const;
		const ExpInfo&		GetVampireRankInfo(int level) const;
		const ExpInfo&		GetOustersRankInfo(int level) const;
		const ExpInfo&		GetPetExp(int level) const;
		const ExpInfo&		GetAdvanceMent(int level) const;

		//----------------------------------------------------------------
		// Load From File
		//----------------------------------------------------------------
		void				LoadFromFileSTR(ivfstream& file);
		void				LoadFromFileDEX(ivfstream& file);
		void				LoadFromFileINT(ivfstream& file);
		void				LoadFromFileVampire(ivfstream& file);
		void				LoadFromFileOusters(ivfstream& file);
		void				LoadFromFileSlayerRank(ivfstream& file);
		void				LoadFromFileVampireRank(ivfstream& file);
		void				LoadFromFileOustersRank(ivfstream& file);
		void				LoadFromFilePetExp(ivfstream& file);
		void				LoadFromFileAdvanceMent(ivfstream& file);

	public :
		ExpTable			m_STRExp;
		ExpTable			m_DEXExp;
		ExpTable			m_INTExp;
		ExpTable			m_VampireExp;
		ExpTable			m_OustersExp;
		ExpTable			m_SlayerRankExp;
		ExpTable			m_VampireRankExp;
		ExpTable			m_OustersRankExp;
		ExpTable			m_PetExp;
		ExpTable			m_advanceSkillExp;
};

//---------------------------------------------------------------------------------
// Global
//---------------------------------------------------------------------------------
extern ExperienceTable*		g_pExperienceTable;

#endif
