//----------------------------------------------------------------------
// PhoenixMobaHUD.h
//----------------------------------------------------------------------
// AOS/MOBA queue HUD overlay.
//
// Runtime contract:
//   - GCMobaJoin packet updates this state.
//   - PhoenixImGui::Render() draws the window only while queue is waiting.
//   - No ini/log/txt file is written by this feature.
//----------------------------------------------------------------------

#ifndef _PHOENIX_MOBA_HUD_H_
#define _PHOENIX_MOBA_HUD_H_

struct IDirect3DDevice9;

namespace PhoenixImGui {

void UpdateMobaQueueState(
    unsigned char state,
    unsigned char joinedCount,
    unsigned char maxCount,
    unsigned short remainingSeconds);

void DrawMobaQueueHUD(IDirect3DDevice9* device);
void ShutdownMobaQueueHUD();

} // namespace PhoenixImGui

#endif // _PHOENIX_MOBA_HUD_H_
