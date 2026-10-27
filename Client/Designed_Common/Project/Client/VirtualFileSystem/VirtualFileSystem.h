//////////////////////////////////////////////////////////////////////////////
/// @file iovfs_base.h
/// @author chyaya
/// @date 2007.06.26
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include <WTypes.h>
#include <string>
#include "CNFS.h"

// Darkeden Ư��ȭ
//#define SPECIALIZE_DARKEDEN

#define VFS_VERSION		1	///< VirtualFileSystem ����

class IFileIO;

//////////////////////////////////////////////////////////////////////////////
/// @struct VirtualFileHeader
/// @brief Virtual File Header
///
/// ���� ������ �ֻ�ܿ� �ٰԵǴ� ���� ���
//////////////////////////////////////////////////////////////////////////////
struct VirtualFileHeader
{
	DWORD	FileSize;		///< ������ ���� ũ��
	BYTE	Version;		///< VFS Version (���� �̰� �� 2��°�� �־�����? - _-;;)
	BYTE	IsCompressed;	///< ������ �Ǿ����� ����

	/// @brief �⺻ ������
	/// @sa Init()
	VirtualFileHeader()	{ Init(); }

	/// @brief ��� ������ �ʱ�ȭ �Ѵ�.
	void Init()
	{
		FileSize		= 0;
		Version			= VFS_VERSION;
		IsCompressed	= 0;
	}
};

/// struct VirtualFileHeader�� ũ��
/// sizeof()�� ����ϸ� 4����Ʈ ������ �������� ������
/// ���� ũ�⸦ ���� �������ش�.
#define VF_HEADER_SIZE		6


//////////////////////////////////////////////////////////////////////////////
/// @class VirtualFileSystem
/// @brief Virtual File System
///
/// ���� ���� �ý����� �����ϴ� Ŭ����
//////////////////////////////////////////////////////////////////////////////
class VirtualFileSystem
{
public:
	/// @brief �⺻ ������
	/// @sa Init()
	VirtualFileSystem();

	/// @brief start ������
	/// @param filename ���� �̸�
	/// @param flags ���� ���
	/// @sa Start()
	/// @warning Ŭ���̾�Ʈ���� ����� ���� �б� �������� �ϴ� ���� ����. (flags�� FS_READ)\n
	///          VFS�� �����͸� ��� �߿� ������ ����� ��� ������ ���Ἲ�� ���� �� �� ���� �����̴�.
	VirtualFileSystem(LPCTSTR filename, int flags = FS_READ);

	/// @brief �ı���
	~VirtualFileSystem();

	/// @brief ������ ���� �Ҷ� ���� ũ��
	enum { FILE_COPY_BUFFER_SIZE = 65536 };

public:
	/// @brief ��� ������ �ʱ�ȭ �Ѵ�.
	void		Init();

	/// @brief Virtual File System�� �����Ѵ�.
	/// @param filename ���� �̸�
	/// @param flags ���� ���
	/// @return ���� ����
	/// @warning Ŭ���̾�Ʈ���� ����� ���� �б� �������� �ϴ� ���� ����. (flags�� FS_READ)\n
	///          VFS�� �����͸� ��� �߿� ������ ����� ��� ������ ���Ἲ�� ���� �� �� ���� �����̴�.
	bool		Start(LPCTSTR filename, int flags = FS_READ);

	/// @brief Virtual File System ����
	void		End();

	/// @brief VFS�� ���� ������ �����Ѵ�.
	/// @param srcname �ҽ��� ���� ������ ���
	/// @param compress �����ؼ� ī���� ������ ����
	/// @warning ����(2007.06.26) �������� ������ ���� ���� ��츦 �׽�Ʈ ���غô�.
	///          ������ ���� �ʰ� ����ϱ� ���Ѵٸ� �ݵ�� �׽�Ʈ �Ŀ� ����� ��.
	/// @sa AddFileRename()
	void		AddFile(LPCTSTR srcname, bool compress = false);

	/// @brief VFS�� ���� ������ �̸��� �ٲ� �����Ѵ�.
	/// @param srcname �ҽ��� ���� ������ ���
	/// @param destname ��ǥ�� ���� ������ ���
	/// @param compress �����ؼ� ī���� ������ ����
	/// @warning ����(2007.06.26) �������� ������ ���� ���� ��츦 �׽�Ʈ ���غô�.
	///          ������ ���� �ʰ� ����ϱ� ���Ѵٸ� �ݵ�� �׽�Ʈ �Ŀ� ����� ��.
	/// @sa AddFile()
	void		AddFileRename(LPCTSTR srcname, LPCTSTR destname, bool compress = false);

#ifdef SPECIALIZE_DARKEDEN
	/// @brief VFS�� ���� ������ �����Ѵ�. (TypePackFile Ư��ȭ ����)
	/// @param srcname �ҽ��� ���� ������ ���
	/// @param compress �����ؼ� ī���� ������ ����
	/// @warning ����(2007.06.26) �������� ������� ����
	/// @sa AddTypePackFileRename(), AddFile(), AddFileRename()
	///
	/// TypePack�� �� Typeũ�⸸ŭ ������ �������Ͽ� ����ִ� ����� ����������
	/// �ʹ� ������ �������� �ٶ��� �����ߴ�. - _-;;
	/// ������� ����
	void		AddTypePackFile(LPCTSTR srcname, bool compress = false);

	/// @brief VFS�� ���� ������ �̸��� �ٲ� �����Ѵ�. (TypePackFile Ư��ȭ ����)
	/// @param srcname �ҽ��� ���� ������ ���
	/// @param destname ��ǥ�� ���� ������ ���
	/// @param compress �����ؼ� ī���� ������ ����
	/// @warning ����(2007.06.26) �������� ������� ����
	/// @sa AddTypePackFile(), AddFile(), AddFileRename()
	///
	/// TypePack�� �� Typeũ�⸸ŭ ������ �������Ͽ� ����ִ� ����� ����������
	/// �ʹ� ������ �������� �ٶ��� �����ߴ�. - _-;;
	/// ������� ����
	void		AddTypePackFileRename(LPCTSTR srcname, LPCTSTR destname, bool compress = false);
#endif // SPECIALIZE_DARKEDEN

	/// @brief VFS ���� ���������� �����Ѵ�.
	/// @param filename ������ ���� ���� ���
	///
	/// filename���� �ѱ� ��ο� ��ġ�ϴ� ���� ������ ������
	/// �ƹ� �ϵ� �Ͼ�� �ʴ´�.
	void		DeleteFile(LPCTSTR filename);

	/// @brief Bypass/Disk-First modunu ayarlar.
	void		set_bypass_path(LPCSTR path) { m_strBypassPath = path; m_bBypassVFS = true; }
	const std::string& GetBypassPath() const { return m_strBypassPath; }
	bool		IsBypassVFS() const { return m_bBypassVFS; }

	/// @brief ���� �� �ִ� ��� ������ �ִ��� �˻�
	/// @param filename �˻��� ���� ���
	/// @return ������ �����ϸ� true
	///
	/// ������ ���� �� �Լ��� ���� ������ �����ϴ��� ���η� ����ؼ��� �ȵȴٴ� ����̴�.
	/// �� �Լ��� ������ �����͸� �о���� �� �ִ� ������ �ִ��� �˻��ϴ� �Լ���
	/// ��������, �Ϲ����� �� �ϳ��� ���ϸ� ���� �ϴ��� true�� �����Ѵ�.
	/// ������ �켱 ������ �Ϲ������� �� ����.
	/// ���� �����غ��� �Լ��̸��� �߸� ���Ѱ� ���⵵ �ϴ�. - _-;;
	bool		IsFileExist(LPCTSTR filename) const;

	/// @brief ���� �� �ִ� ��� ������ ũ�� �˻�
	/// @param filename �˻��� ���� ���
	/// @return ������ ũ�� (BYTE����)
	///
	/// �� �Լ��� ������ �����͸� �о���� �� �ִ� ������ ũ�⸦ �˻��ϴ� �Լ���
	/// �������ϸ� �����ϸ� ���������� ũ�⸦, �Ϲ� ���ϸ� �����ϸ� �Ϲ� ������ ũ�⸦,
	/// �Ѵ� �����ϸ� �Ϲ� ������ ũ�⸦ ��� �Լ���.
	/// �� �Լ� ���� ���� �����غ��� �Լ��̸��� �߸� ���Ѱ� ���⵵ �ϴ�. - _-;;
	int			GetFileSize(LPCTSTR filename);

	/// @brief ���� ������ �Ϲ� ���Ϸ� ����� �ش�.
	/// @param srcname �ҽ��� ���� ���� ���
	/// @return ���� ����
	/// @sa CreateTempFileRename()
	///
	/// CreateTempFileRename()�� ���� ����� �ϴ� �Լ�����
	/// make_temp_filepath()�� ����ؼ� �ڵ����� �ӽ� ���� ��θ� ����� �ش�.
	
	bool		CreateTempFile(LPCTSTR srcname);

	/// @brief ���� ������ �Ϲ� ���Ϸ� ����� �ش�.
	/// @param srcname �ҽ��� ���� ���� ���
	/// @param destname ����� �Ϲ� ���� ���
	/// @return ���� ����
	/// @sa CreateTempFile()
	///
	/// ���� ������ �Ϲ� ���Ϸ� ����� �ش�.
	/// ���丮�� ������ ��� �ڵ����� ������
	bool		CreateRealFile(LPCTSTR srcname, LPCTSTR destname);

	/// @brief NFS Wrapper Ŭ������ �����͸� ��´�.
	/// @return CNFS ������
	CNFS*		GetNFS()	{ return &m_NFS;	}

protected:
	/// @brief �ش� ���丮�� ����� �ش�.
	/// @param szFullPath ���丮 ��γ� ���� ���
	/// @return ���� ����
	///
	/// ���� ��θ� ������ Ǯ ���丮�� �������ش�.
	bool		BuildFullPathDirectory(LPCTSTR szFullPath);
	
private:
	CNFS		m_NFS;	///< NFS Wrapper
	std::string m_strBypassPath;
	bool        m_bBypassVFS;
};

typedef VirtualFileSystem	VFSYSTEM;
typedef VFSYSTEM*			LPVFSYSTEM;
