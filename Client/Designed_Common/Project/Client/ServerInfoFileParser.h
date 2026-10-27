#pragma once



class ServerInfoFileParser
{
public :
	ServerInfoFileParser(std::string infofilename);
	~ServerInfoFileParser();

	std::string		getProperty(int dimension, std::string key);
	int				getPropertyInt( int dimension, std::string key);

private :
	std::string						m_FileName;
//	std::vector< std::string >		m_LineList;
};
//업데이터 변경에 따라 서버 정보를 읽어 오는 방식이 변경되어 Properties 클래스로 변경
//extern ServerInfoFileParser * g_pConfigForeign;
