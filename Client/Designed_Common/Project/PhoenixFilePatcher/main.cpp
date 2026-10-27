#include "PhoenixProtectedFile.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>

namespace fs = std::filesystem;

namespace
{
	struct PatchItem
	{
		fs::path InputPath;
		std::string LogicalPath;
	};

	enum ControlId
	{
		ID_LIST_FILES = 1001,
		ID_EDIT_OUTPUT = 1002,
		ID_BUTTON_BROWSE_OUTPUT = 1003,
		ID_BUTTON_ADD_FILES = 1004,
		ID_BUTTON_ADD_FOLDER = 1005,
		ID_BUTTON_REMOVE = 1006,
		ID_BUTTON_CLEAR = 1007,
		ID_BUTTON_PATCH = 1008,
		ID_CHECK_VERIFY = 1009,
		ID_EDIT_LOG = 1010,
	};

	HWND g_hListFiles = NULL;
	HWND g_hEditOutput = NULL;
	HWND g_hCheckVerify = NULL;
	HWND g_hEditLog = NULL;
	std::vector<PatchItem> g_Items;

	void PrintUsage()
	{
		std::cout
			<< "PhoenixFilePatcher\n"
			<< "\n"
			<< "Commands:\n"
			<< "  gui\n"
			<< "  pack-file --input <file.inf> --output <file.inf.phx> --logical <relative/original/path.inf>\n"
			<< "  pack-inf  --input <root>     --output <root>          [--verify]\n"
			<< "  verify-file --input <file.inf> --phx <file.inf.phx> --logical <relative/original/path.inf>\n"
			<< "\n"
			<< "Notes:\n"
			<< "  .phx files are runtime-protected data files, not legacy updater .dp2 files.\n"
			<< "  The tool intentionally does not provide a plaintext unpack command.\n";
	}

	std::string GetArg(int argc, char** argv, const char* name)
	{
		for(int i = 0; i < argc - 1; ++i)
		{
			if(std::string(argv[i]) == name)
				return argv[i + 1];
		}
		return std::string();
	}

	bool HasFlag(int argc, char** argv, const char* name)
	{
		for(int i = 0; i < argc; ++i)
		{
			if(std::string(argv[i]) == name)
				return true;
		}
		return false;
	}

	bool ReadAll(const fs::path& path, std::vector<char>& data)
	{
		std::ifstream file(path, std::ios::binary);
		if(!file.is_open())
			return false;

		file.seekg(0, std::ios::end);
		const std::streamoff size = file.tellg();
		if(size < 0)
			return false;

		file.seekg(0, std::ios::beg);
		data.resize(static_cast<size_t>(size));
		if(!data.empty())
			file.read(data.data(), static_cast<std::streamsize>(data.size()));

		return file.good() || file.eof();
	}

	std::string ToGenericPath(const fs::path& path)
	{
		return path.generic_string();
	}

	bool IsInfFile(const fs::path& path)
	{
		std::string ext = path.extension().string();
		for(char& ch : ext)
		{
			if(ch >= 'A' && ch <= 'Z')
				ch = static_cast<char>(ch - 'A' + 'a');
		}
		return ext == ".inf";
	}

	bool PackFile(const fs::path& input, const fs::path& output, const std::string& logicalPath, bool verify)
	{
		std::vector<char> data;
		if(!ReadAll(input, data))
		{
			std::cerr << "read failed: " << input << "\n";
			return false;
		}

		fs::create_directories(output.parent_path());
		if(!PhoenixProtectedFile::Save(logicalPath.c_str(), data.empty() ? nullptr : data.data(), data.size(), output.string().c_str()))
		{
			std::cerr << "pack failed: " << input << " -> " << output << "\n";
			return false;
		}

		if(verify)
		{
			std::vector<char> decoded;
			if(!PhoenixProtectedFile::LoadWithOriginalPath(output.string().c_str(), logicalPath.c_str(), decoded))
			{
				std::cerr << "verify decode failed: " << output << "\n";
				return false;
			}

			if(decoded != data)
			{
				std::cerr << "verify mismatch: " << output << "\n";
				return false;
			}
		}

		std::cout << "packed: " << logicalPath << " -> " << output.string() << "\n";
		return true;
	}

	fs::path GetProjectRootFromExe()
	{
		char exePath[MAX_PATH] = {0};
		GetModuleFileNameA(NULL, exePath, MAX_PATH);

		fs::path root = fs::absolute(exePath).parent_path();
		// tools/PhoenixFilePatcher/bin/PhoenixFilePatcher.exe -> project root
		for(int i = 0; i < 3 && root.has_parent_path(); ++i)
			root = root.parent_path();

		return root;
	}

	bool IsPathInside(const fs::path& path, const fs::path& root)
	{
		if(root.empty())
			return false;

		std::error_code ec;
		const fs::path canonicalPath = fs::weakly_canonical(path, ec);
		if(ec)
			return false;

		const fs::path canonicalRoot = fs::weakly_canonical(root, ec);
		if(ec)
			return false;

		fs::path relative = canonicalPath.lexically_relative(canonicalRoot);
		if(relative.empty())
			return false;

		const std::string rel = relative.generic_string();
		return rel != "." && rel.rfind("../", 0) != 0 && rel != "..";
	}

	std::string MakeLogicalPath(const fs::path& input, const fs::path& selectedRoot)
	{
		const fs::path projectRoot = GetProjectRootFromExe();
		const fs::path gameDataRoot = projectRoot / "GameData";

		std::error_code ec;
		if(IsPathInside(input, gameDataRoot))
			return ToGenericPath(fs::weakly_canonical(input, ec).lexically_relative(fs::weakly_canonical(gameDataRoot, ec)));

		if(!selectedRoot.empty() && IsPathInside(input, selectedRoot))
			return ToGenericPath(fs::weakly_canonical(input, ec).lexically_relative(fs::weakly_canonical(selectedRoot, ec)));

		return ToGenericPath(input.filename());
	}

	void AppendLog(const std::string& text)
	{
		if(!g_hEditLog)
			return;

		const int length = GetWindowTextLengthA(g_hEditLog);
		SendMessageA(g_hEditLog, EM_SETSEL, length, length);

		std::string line = text;
		line += "\r\n";
		SendMessageA(g_hEditLog, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(line.c_str()));
	}

	std::string GetWindowString(HWND hwnd)
	{
		const int len = GetWindowTextLengthA(hwnd);
		std::string text(static_cast<size_t>(len) + 1, '\0');
		if(len > 0)
			GetWindowTextA(hwnd, &text[0], len + 1);
		text.resize(strlen(text.c_str()));
		return text;
	}

	void RefreshFileList()
	{
		SendMessageA(g_hListFiles, LB_RESETCONTENT, 0, 0);
		for(const PatchItem& item : g_Items)
		{
			std::string display = item.InputPath.string();
			display += "  ->  ";
			display += item.LogicalPath;
			SendMessageA(g_hListFiles, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(display.c_str()));
		}
	}

	bool AddPatchItem(const fs::path& input, const fs::path& selectedRoot)
	{
		if(!fs::exists(input) || !fs::is_regular_file(input) || !IsInfFile(input))
			return false;

		const fs::path absoluteInput = fs::absolute(input);
		for(const PatchItem& item : g_Items)
		{
			if(fs::equivalent(item.InputPath, absoluteInput))
				return false;
		}

		PatchItem item;
		item.InputPath = absoluteInput;
		item.LogicalPath = MakeLogicalPath(absoluteInput, selectedRoot);
		g_Items.push_back(item);
		return true;
	}

	void BrowseOutputFolder(HWND owner)
	{
		BROWSEINFOA bi = {};
		bi.hwndOwner = owner;
		bi.lpszTitle = "Select output folder";
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

		LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
		if(!pidl)
			return;

		char path[MAX_PATH] = {0};
		if(SHGetPathFromIDListA(pidl, path))
			SetWindowTextA(g_hEditOutput, path);

		CoTaskMemFree(pidl);
	}

	void AddFilesDialog(HWND owner)
	{
		std::vector<char> buffer(65536, 0);

		OPENFILENAMEA ofn = {};
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = owner;
		ofn.lpstrFilter = "INF files (*.inf)\0*.inf\0All files (*.*)\0*.*\0";
		ofn.lpstrFile = buffer.data();
		ofn.nMaxFile = static_cast<DWORD>(buffer.size());
		ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrTitle = "Select INF files";

		if(!GetOpenFileNameA(&ofn))
			return;

		const char* first = buffer.data();
		const char* next = first + strlen(first) + 1;

		size_t added = 0;
		if(*next == '\0')
		{
			if(AddPatchItem(first, fs::path(first).parent_path()))
				++added;
		}
		else
		{
			const fs::path folder = first;
			while(*next)
			{
				if(AddPatchItem(folder / next, folder))
					++added;
				next += strlen(next) + 1;
			}
		}

		RefreshFileList();
		AppendLog("Added files: " + std::to_string(added));
	}

	void AddFolderDialog(HWND owner)
	{
		BROWSEINFOA bi = {};
		bi.hwndOwner = owner;
		bi.lpszTitle = "Select folder containing INF files";
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

		LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
		if(!pidl)
			return;

		char path[MAX_PATH] = {0};
		if(!SHGetPathFromIDListA(pidl, path))
		{
			CoTaskMemFree(pidl);
			return;
		}
		CoTaskMemFree(pidl);

		const fs::path folder = fs::absolute(path);
		size_t added = 0;
		for(const fs::directory_entry& entry : fs::recursive_directory_iterator(folder))
		{
			if(entry.is_regular_file() && IsInfFile(entry.path()) && AddPatchItem(entry.path(), folder))
				++added;
		}

		RefreshFileList();
		AppendLog("Added folder INF files: " + std::to_string(added));
	}

	void RemoveSelectedItems()
	{
		const int count = static_cast<int>(SendMessageA(g_hListFiles, LB_GETSELCOUNT, 0, 0));
		if(count <= 0)
			return;

		std::vector<int> selected(count);
		SendMessageA(g_hListFiles, LB_GETSELITEMS, count, reinterpret_cast<LPARAM>(selected.data()));

		for(int i = count - 1; i >= 0; --i)
		{
			const int index = selected[i];
			if(index >= 0 && static_cast<size_t>(index) < g_Items.size())
				g_Items.erase(g_Items.begin() + index);
		}

		RefreshFileList();
		AppendLog("Removed selected files.");
	}

	std::vector<int> GetAllIndices()
	{
		std::vector<int> indices;
		for(size_t i = 0; i < g_Items.size(); ++i)
			indices.push_back(static_cast<int>(i));
		return indices;
	}

	void PatchAllItems()
	{
		if(g_Items.empty())
		{
			AppendLog("No INF files in the list.");
			return;
		}

		const bool verify = SendMessageA(g_hCheckVerify, BM_GETCHECK, 0, 0) == BST_CHECKED;
		const std::string outputRootText = GetWindowString(g_hEditOutput);
		if(outputRootText.empty())
		{
			AppendLog("Select an output folder before patching.");
			MessageBoxA(NULL, "Select an output folder before patching.", "Phoenix File Patcher", MB_OK | MB_ICONWARNING);
			return;
		}

		const fs::path outputRoot = fs::absolute(outputRootText);

		size_t ok = 0;
		size_t fail = 0;
		const std::vector<int> indices = GetAllIndices();

		AppendLog("Patch started. File count: " + std::to_string(indices.size()));

		for(int index : indices)
		{
			if(index < 0 || static_cast<size_t>(index) >= g_Items.size())
				continue;

			const PatchItem& item = g_Items[index];
			fs::path output = outputRoot / fs::path(item.LogicalPath);
			output += PhoenixProtectedFile::Extension();

			if(PackFile(item.InputPath, output, item.LogicalPath, verify))
			{
				++ok;
				AppendLog("OK: " + item.LogicalPath + " -> " + output.string());
			}
			else
			{
				++fail;
				AppendLog("FAIL: " + item.InputPath.string());
			}
		}

		AppendLog("Patch finished. OK=" + std::to_string(ok) + " FAIL=" + std::to_string(fail));
	}

	void LayoutControls(HWND hwnd)
	{
		RECT rc;
		GetClientRect(hwnd, &rc);
		const int w = rc.right - rc.left;
		const int h = rc.bottom - rc.top;
		const int margin = 10;
		const int buttonH = 28;
		const int labelY = margin;
		const int outputY = labelY + 20;
		const int yButtons = outputY + 34;
		const int listY = yButtons + buttonH + 8;
		const int logH = 120;
		int listH = h - listY - logH - 3 * margin;
		if(listH < 80)
			listH = 80;

		HWND outputLabel = GetDlgItem(hwnd, 2001);
		if(outputLabel) MoveWindow(outputLabel, margin, labelY, 110, 18, TRUE);
		MoveWindow(g_hEditOutput, margin, outputY, w - 150, 24, TRUE);
		MoveWindow(GetDlgItem(hwnd, ID_BUTTON_BROWSE_OUTPUT), w - 130, outputY - 1, 120, buttonH, TRUE);

		int x = margin;
		MoveWindow(GetDlgItem(hwnd, ID_BUTTON_ADD_FILES), x, yButtons, 95, buttonH, TRUE); x += 100;
		MoveWindow(GetDlgItem(hwnd, ID_BUTTON_ADD_FOLDER), x, yButtons, 105, buttonH, TRUE); x += 110;
		MoveWindow(GetDlgItem(hwnd, ID_BUTTON_REMOVE), x, yButtons, 85, buttonH, TRUE); x += 90;
		MoveWindow(GetDlgItem(hwnd, ID_BUTTON_CLEAR), x, yButtons, 70, buttonH, TRUE); x += 78;
		MoveWindow(g_hCheckVerify, x, yButtons + 4, 80, 22, TRUE); x += 90;
		MoveWindow(GetDlgItem(hwnd, ID_BUTTON_PATCH), x, yButtons, 150, buttonH, TRUE);

		MoveWindow(g_hListFiles, margin, listY, w - 2 * margin, listH, TRUE);
		MoveWindow(g_hEditLog, margin, listY + listH + margin, w - 2 * margin, logH, TRUE);
	}

	LRESULT CALLBACK GuiWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch(msg)
		{
		case WM_CREATE:
		{
			HFONT font = reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
			CreateWindowA("STATIC", "Output folder", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(2001), NULL, NULL);
			g_hEditOutput = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
				WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_EDIT_OUTPUT), NULL, NULL);
			CreateWindowA("BUTTON", "Browse...", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_BUTTON_BROWSE_OUTPUT), NULL, NULL);
			CreateWindowA("BUTTON", "Add Files", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_BUTTON_ADD_FILES), NULL, NULL);
			CreateWindowA("BUTTON", "Add Folder", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_BUTTON_ADD_FOLDER), NULL, NULL);
			CreateWindowA("BUTTON", "Remove", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_BUTTON_REMOVE), NULL, NULL);
			CreateWindowA("BUTTON", "Clear", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_BUTTON_CLEAR), NULL, NULL);
			g_hCheckVerify = CreateWindowA("BUTTON", "Verify", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_CHECK_VERIFY), NULL, NULL);
			SendMessageA(g_hCheckVerify, BM_SETCHECK, BST_CHECKED, 0);
			CreateWindowA("BUTTON", "Patch All", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_BUTTON_PATCH), NULL, NULL);

			g_hListFiles = CreateWindowExA(WS_EX_CLIENTEDGE, "LISTBOX", "",
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_EXTENDEDSEL | LBS_NOINTEGRALHEIGHT,
				0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_LIST_FILES), NULL, NULL);
			g_hEditLog = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
				0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_EDIT_LOG), NULL, NULL);

			HWND children[] = {
				GetDlgItem(hwnd, 2001), g_hEditOutput, GetDlgItem(hwnd, ID_BUTTON_BROWSE_OUTPUT),
				GetDlgItem(hwnd, ID_BUTTON_ADD_FILES), GetDlgItem(hwnd, ID_BUTTON_ADD_FOLDER),
				GetDlgItem(hwnd, ID_BUTTON_REMOVE), GetDlgItem(hwnd, ID_BUTTON_CLEAR),
				g_hCheckVerify, GetDlgItem(hwnd, ID_BUTTON_PATCH), g_hListFiles, g_hEditLog
			};
			for(HWND child : children)
				SendMessageA(child, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);

			AppendLog("PhoenixFilePatcher GUI ready.");
			AppendLog("Select an output folder, add INF files or a folder, then press Patch All.");
			LayoutControls(hwnd);
			return 0;
		}
		case WM_SIZE:
			LayoutControls(hwnd);
			return 0;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case ID_BUTTON_BROWSE_OUTPUT:
				BrowseOutputFolder(hwnd);
				return 0;
			case ID_BUTTON_ADD_FILES:
				AddFilesDialog(hwnd);
				return 0;
			case ID_BUTTON_ADD_FOLDER:
				AddFolderDialog(hwnd);
				return 0;
			case ID_BUTTON_REMOVE:
				RemoveSelectedItems();
				return 0;
			case ID_BUTTON_CLEAR:
				g_Items.clear();
				RefreshFileList();
				AppendLog("List cleared.");
				return 0;
			case ID_BUTTON_PATCH:
				PatchAllItems();
				return 0;
			default:
				break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			break;
		}

		return DefWindowProcA(hwnd, msg, wParam, lParam);
	}

	int RunGui()
	{
		HINSTANCE hInstance = GetModuleHandleA(NULL);
		const char* className = "PhoenixFilePatcherGui";

		WNDCLASSA wc = {};
		wc.lpfnWndProc = GuiWndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = className;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);

		RegisterClassA(&wc);

		HWND hwnd = CreateWindowExA(
			0,
			className,
			"Phoenix File Patcher",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			920,
			640,
			NULL,
			NULL,
			hInstance,
			NULL);

		if(!hwnd)
			return 1;

		MSG msg;
		while(GetMessageA(&msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		return static_cast<int>(msg.wParam);
	}

	int PackFileCommand(int argc, char** argv)
	{
		const std::string input = GetArg(argc, argv, "--input");
		const std::string output = GetArg(argc, argv, "--output");
		const std::string logical = GetArg(argc, argv, "--logical");

		if(input.empty() || output.empty() || logical.empty())
		{
			PrintUsage();
			return 2;
		}

		return PackFile(input, output, logical, true) ? 0 : 1;
	}

	int PackInfCommand(int argc, char** argv)
	{
		const std::string inputRootArg = GetArg(argc, argv, "--input");
		const std::string outputRootArg = GetArg(argc, argv, "--output");
		const bool verify = HasFlag(argc, argv, "--verify");

		if(inputRootArg.empty() || outputRootArg.empty())
		{
			PrintUsage();
			return 2;
		}

		const fs::path inputRoot = fs::absolute(inputRootArg);
		const fs::path outputRoot = fs::absolute(outputRootArg);

		if(!fs::exists(inputRoot) || !fs::is_directory(inputRoot))
		{
			std::cerr << "input root not found: " << inputRoot << "\n";
			return 1;
		}

		size_t packedCount = 0;
		size_t failedCount = 0;

		for(const fs::directory_entry& entry : fs::recursive_directory_iterator(inputRoot))
		{
			if(!entry.is_regular_file() || !IsInfFile(entry.path()))
				continue;

			const fs::path relative = fs::relative(entry.path(), inputRoot);
			fs::path output = outputRoot / relative;
			output += PhoenixProtectedFile::Extension();

			if(PackFile(entry.path(), output, ToGenericPath(relative), verify))
				++packedCount;
			else
				++failedCount;
		}

		std::cout << "summary: packed=" << packedCount << " failed=" << failedCount << "\n";
		return failedCount == 0 ? 0 : 1;
	}

	int VerifyFileCommand(int argc, char** argv)
	{
		const std::string input = GetArg(argc, argv, "--input");
		const std::string phx = GetArg(argc, argv, "--phx");
		const std::string logical = GetArg(argc, argv, "--logical");

		if(input.empty() || phx.empty() || logical.empty())
		{
			PrintUsage();
			return 2;
		}

		std::vector<char> original;
		if(!ReadAll(input, original))
		{
			std::cerr << "read failed: " << input << "\n";
			return 1;
		}

		std::vector<char> decoded;
		if(!PhoenixProtectedFile::LoadWithOriginalPath(phx.c_str(), logical.c_str(), decoded))
		{
			std::cerr << "verify decode failed: " << phx << "\n";
			return 1;
		}

		if(decoded != original)
		{
			std::cerr << "verify mismatch: " << phx << "\n";
			return 1;
		}

		std::cout << "verify ok: " << phx << "\n";
		return 0;
	}
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		return RunGui();
	}

	const std::string command = argv[1];
	if(command == "gui")
		return RunGui();
	if(command == "--help" || command == "-h")
	{
		PrintUsage();
		return 0;
	}
	if(command == "pack-file")
		return PackFileCommand(argc - 1, argv + 1);
	if(command == "pack-inf")
		return PackInfCommand(argc - 1, argv + 1);
	if(command == "verify-file")
		return VerifyFileCommand(argc - 1, argv + 1);

	PrintUsage();
	return 2;
}
