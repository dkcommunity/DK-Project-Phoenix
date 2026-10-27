//--------------------------------------------------------------------------------
//
// Filename    : CURequestHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "CURequest.h"

#ifdef __UPDATE_SERVER__
#include <fcntl.h>
#include <stdio.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <unistd.h>

#include "Properties.h"

#include "Resource.h"
#include "Upackets/UCUpdate.h"
#include "UpdateServerPlayer.h"

#endif

//--------------------------------------------------------------------------------
//
// 클라이언트가 요청한 리소스가 존재하는지 체크한 후,
// 존재할 경우, sendfile()을 사용해서 클라이언트로 전송한다.
//
//--------------------------------------------------------------------------------
void CURequestHandler::execute(CURequest *pPacket,
                               Player *pPlayer) throw(ProtocolException,
                                                      Error) {
  __BEGIN_TRY

#ifdef __UPDATE_SERVER__

  UpdateServerPlayer *pUpdateServerPlayer =
      dynamic_cast<UpdateServerPlayer *>(pPlayer);

  Socket *pSocket = pUpdateServerPlayer->getSocket();

  const Resource &resource = pPacket->getResource();

  Version_t ClientVersion = 0;

  if (resource.getVersion() < 2000) {
    ClientVersion = 1;
  }

  char filename[maxFilename + 1];
  sprintf(filename, "%s/%s/v%05d/%s", g_pConfig->getProperty("HomeDir").c_str(),
          g_pConfig->getProperty("PatchDir").c_str(),
          //		resource.getVersion() ,
          ClientVersion, resource.getFilename().c_str());

  // cout << "Try to open " << filename << "..." << endl;
  if (access(filename, F_OK)) {
    StringStream msg;
    msg << "file does not exist : " << filename;
    throw Error(msg.toString());
  }

  int fd = open(filename, O_RDONLY);

  if (fd < 0) {
    // 文件不存在的情况，抛出异常。
    // 记录日志后，可能需要封禁。
    throw InvalidProtocolException(filename);
  }

  // 为了获取文件大小，移到文件末尾。
  FileSize_t filesize = lseek(fd, 0, SEEK_END);

  // cout << "Original FileSize  : " << filesize << endl;
  // cout << "Requested FileSize : " << resource.getFileSize() << endl;

  // 클라이언트에서 요청한 파일의        //
  // 客户端请求的文件大小不一致时，抛出异常。
  if (resource.getFileSize() != filesize) {
    close(fd);
    throw InvalidProtocolException("invalid filesize");
  }

  //--------------------------------------------------------------------------------
  // CURequest 에 담겨온 리소스를 다시 UCUpdate 에 담아서 먼저 전송한다.
  // 그다음 파일 내용을 전송한다.
  //--------------------------------------------------------------------------------
  UCUpdate ucUpdate;
  ucUpdate.setResource(resource);

  pUpdateServerPlayer->sendPacket(&ucUpdate);

  //--------------------------------------------------------------------------------
  // write file content
  //--------------------------------------------------------------------------------
  // 파일 포인터를        // 重新定位到文件开头。
  lseek(fd, 0, SEEK_SET);
  off_t offset = 0;

  // cout <<
  // "======================================================================="
  // << endl; cout << resource.getFilename() << " (" << filesize << "bytes) : ";

  // 发送整个文件。
  ssize_t nSent = sendfile(pSocket->getSOCKET(), fd, &offset, filesize);

  // cout << nSent << " bytes sent to client." << endl;
  // cout <<
  // "======================================================================="
  // << endl;

  if (nSent != (ssize_t)filesize) {
    close(fd);
    throw Error("发送文件失败");
  }

  // 关闭文件。
  close(fd);

  pUpdateServerPlayer->setPlayerStatus(USPS_AFTER_SENDING_UC_UPDATE);

#endif

  __END_CATCH
}
