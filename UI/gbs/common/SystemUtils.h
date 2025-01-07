#ifndef __SYSTEM_UTILS_H__
#define __SYSTEM_UTILS_H__
#include <iostream>
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

inline std::string GetWindowsProductIDFromRegistery()
{
	HKEY hKey;
	const char *regPath = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";
	const char *regValue = "ProductId";
	char productID[256];
	DWORD bufferSize = sizeof(productID);

	// 打开注册表键
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath, 0,
			  KEY_READ | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS) {
		// 读取 ProductId 值
		if (RegQueryValueExA(hKey, regValue, nullptr, nullptr,
				     (LPBYTE)productID,
				     &bufferSize) == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return std::string(productID);
		}
		RegCloseKey(hKey);
	}

	return "";
}



inline std::string GetMachineIdFromRegistry()
{
	HKEY hKey;
	const std::string subKey = "SOFTWARE\\Microsoft\\SQMClient";
	std::string machineId;

	// 打开注册表键
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKey.c_str(), 0, KEY_READ,
			  &hKey) == ERROR_SUCCESS) {
		char value[256];
		DWORD valueLength = sizeof(value);

		// 读取 MachineId
		if (RegQueryValueExA(hKey, "MachineId", NULL, NULL,
				     (LPBYTE)value,
				     &valueLength) == ERROR_SUCCESS) {
			machineId = std::string(value, valueLength);
		} else {
			std::cerr << "Failed to read MachineId from registry."
				  << std::endl;
		}

		// 关闭注册表键
		RegCloseKey(hKey);
	} else {
		std::cerr << "Failed to open registry key." << std::endl;
	}
	machineId.erase(std::remove(machineId.begin(), machineId.end(), '\0'), machineId.end());
	if (machineId.front() == '{' && machineId.back() == '}') {
		machineId = machineId.substr(1, machineId.size() - 2); // 去掉首尾的大括号
	}

	return machineId;
}

inline  std::string GetDeviceID()
{
	HRESULT hres;
	std::string deviceID;

	// 初始化 COM
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
		return deviceID;

	// 初始化安全
	hres = CoInitializeSecurity(NULL, -1, NULL, NULL,
				    RPC_C_AUTHN_LEVEL_DEFAULT,
				    RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
				    EOAC_NONE, NULL);
	if (FAILED(hres) && hres != RPC_E_TOO_LATE) {
		CoUninitialize();
		return deviceID;
	}

	IWbemLocator *pLoc = NULL;
	hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
				IID_IWbemLocator, (LPVOID *)&pLoc);
	if (FAILED(hres)) {
		CoUninitialize();
		return deviceID;
	}

	IWbemServices *pSvc = NULL;
	hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL,
				   0, 0, &pSvc);
	if (FAILED(hres)) {
		pLoc->Release();
		CoUninitialize();
		return deviceID;
	}

	IEnumWbemClassObject *pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"), bstr_t("SELECT UUID FROM Win32_ComputerSystemProduct"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL,
		&pEnumerator);
	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return deviceID;
	}

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr =
			pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (0 == uReturn)
			break;

		VARIANT vtProp;
		hr = pclsObj->Get(L"UUID", 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR) {
			deviceID = _com_util::ConvertBSTRToString(
				vtProp.bstrVal); // 转换为 std::string
		}
		VariantClear(&vtProp);
		pclsObj->Release();
	}

	// 清理
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return deviceID;
}


inline std::string GetWindowsProductID()
{
	HRESULT hres;
	std::string productID;

	// 初始化 COM
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
		return productID;

	// 初始化安全性
	hres = CoInitializeSecurity(NULL, -1, NULL, NULL,
				    RPC_C_AUTHN_LEVEL_DEFAULT,
				    RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
				    EOAC_NONE, NULL);
	if (FAILED(hres) && hres != RPC_E_TOO_LATE) {
		CoUninitialize();
		return productID;
	}

	IWbemLocator *pLoc = NULL;
	hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
				IID_IWbemLocator, (LPVOID *)&pLoc);
	if (FAILED(hres)) {
		CoUninitialize();
		return productID;
	}

	IWbemServices *pSvc = NULL;
	hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL,
				   0, 0, &pSvc);
	if (FAILED(hres)) {
		pLoc->Release();
		CoUninitialize();
		return productID;
	}

	IEnumWbemClassObject *pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT SerialNumber FROM Win32_OperatingSystem"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL,
		&pEnumerator);
	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return productID;
	}

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr =
			pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (0 == uReturn)
			break;

		VARIANT vtProp;
		hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
		productID = _com_util::ConvertBSTRToString(
			vtProp.bstrVal); // 转换为 std::string
		VariantClear(&vtProp);
		pclsObj->Release();
	}

	// 清理
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return productID;
}

inline std::string GetMainboardSerialNumber()
{
	HRESULT hres;

	// 初始化COM库
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		std::cerr << "Failed to initialize COM library. Error code: " << hres << std::endl;
		return "";
	}

	// 设置COM安全
	hres = CoInitializeSecurity(NULL,
				    -1,                          // COM authentication
				    NULL,                        // Authentication services
				    NULL,                        // Reserved
				    RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
				    RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
				    NULL,                        // Authentication info
				    EOAC_NONE,                   // Additional capabilities
				    NULL                         // Reserved
	);

	if (FAILED(hres)) {
		std::cerr << "Failed to initialize security. Error code: " << hres << std::endl;
		CoUninitialize();
		return "";
	}

	// 获取WMI服务
	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		std::cerr << "Failed to create IWbemLocator object. Error code: " << hres << std::endl;
		CoUninitialize();
		return "";
	}

	IWbemServices *pSvc = NULL;

	hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), // WMI namespace
				   NULL,                    // User name
				   NULL,                    // User password
				   0,                       // Locale
				   NULL,                    // Security flags
				   0,                       // Authority
				   0,                       // Context object
				   &pSvc                    // IWbemServices proxy
	);

	if (FAILED(hres)) {
		std::cerr << "Could not connect. Error code: " << hres << std::endl;
		pLoc->Release();
		CoUninitialize();
		return "";
	}

	// 设置WMI服务安全级别
	hres = CoSetProxyBlanket(pSvc,                        // Indicates the proxy to set
				 RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
				 RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
				 NULL,                        // Server principal name
				 RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
				 RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
				 NULL,                        // Client identity
				 EOAC_NONE                    // Proxy capabilities
	);

	if (FAILED(hres)) {
		std::cerr << "Could not set proxy blanket. Error code: " << hres << std::endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return "";
	}

	// 执行WMI查询
	IEnumWbemClassObject *pEnumerator = NULL;

	hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT SerialNumber FROM Win32_BaseBoard"),
			       WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

	if (FAILED(hres)) {
		std::cerr << "Query for serial number failed. Error code: " << hres << std::endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return "";
	}

	// 获取查询结果
	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;
	std::string serialNumber;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;
		hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);

		if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR) {
			serialNumber = _bstr_t(vtProp.bstrVal);
		}

		VariantClear(&vtProp);
		pclsObj->Release();
	}

	// 清理
	pEnumerator->Release();
	pSvc->Release();
	pLoc->Release();
	CoUninitialize();

	return serialNumber;
}

inline bool ReadRegistryValue(HKEY rootKey, const std::string &subKey, const std::string &valueName, std::string &outValue)
{
	HKEY hKey;
	if (RegOpenKeyExA(rootKey, subKey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
		return false; // 打开键失败
	}

	char buffer[256];
	DWORD bufferSize = sizeof(buffer);
	DWORD valueType;
	LONG result = RegQueryValueExA(hKey, valueName.c_str(), 0, &valueType, (LPBYTE)buffer, &bufferSize);

	RegCloseKey(hKey);

	if (result == ERROR_SUCCESS && (valueType == REG_SZ || valueType == REG_EXPAND_SZ)) {
		outValue.assign(buffer, bufferSize - 1); // 去掉尾部的 null 字符
		return true;
	}
	return false; // 读取失败或值类型不匹配
}

inline bool WriteRegistryValue(HKEY rootKey, const std::string &subKey, const std::string &valueName,
			       const std::string &value)
{
	HKEY hKey;
	// 创建或打开注册表键
	if (RegCreateKeyExA(rootKey, subKey.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
		return false; // 打开或创建键失败
	}

	// 设置值
	LONG result = RegSetValueExA(hKey, valueName.c_str(), 0, REG_SZ, (const BYTE *)value.c_str(), DWORD(value.size() + 1));
	RegCloseKey(hKey);

	return result == ERROR_SUCCESS;
}

inline bool RegisterVCam()
{
	// 获取当前可执行文件的路径
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	// 获取当前目录的路径
	std::string exePath(buffer);
	size_t pos = exePath.find_last_of("\\/");
	std::string exeDir = exePath.substr(0, pos);

	// 获取上级目录的路径（..\data）
	std::string targetDir = exeDir + "\\..\\..\\data\\obs-plugins\\win-dshow";

	// 拼接完整路径，指向 virtualcam-install.bat
	std::string batFilePath = targetDir + "\\virtualcam-install.bat";

	// 打印调试信息
	std::cout << "Executing batch file: " << batFilePath << std::endl;

	// 创建一个命令行来执行批处理文件
	std::string command = "\"" + batFilePath + "\"";

	// 使用 CreateProcess 执行批处理文件
	STARTUPINFOA si = {0};
	PROCESS_INFORMATION pi = {0};

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW; // 设置窗口样式
	si.wShowWindow = SW_HIDE;          // 隐藏窗口（后台执行）

	if (CreateProcessA(NULL,                   // 可执行文件路径
			   (LPSTR)command.c_str(), // 命令行参数
			   NULL,                   // 进程安全属性
			   NULL,                   // 线程安全属性
			   FALSE,                  // 不继承句柄
			   CREATE_NO_WINDOW,       // 无标志
			   NULL,                   // 环境变量
			   NULL,                   // 当前目录
			   &si,                    // 启动信息
			   &pi                     // 进程信息
			   )) {
		// 等待批处理执行完成
		DWORD dwWaitResult = WaitForSingleObject(pi.hProcess, INFINITE);
		if (dwWaitResult == WAIT_FAILED) {
			std::cerr << "WaitForSingleObject failed with error code " << GetLastError() << std::endl;
		}

		// 清理
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		std::cout << "Batch file executed successfully." << std::endl;
	} else {
		
		std::cerr << "Failed to execute batch file." << std::endl;
		return false;
	}

	return true;
}

#endif //__SYSTEM_UTILS_H__
