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

#endif //__SYSTEM_UTILS_H__
