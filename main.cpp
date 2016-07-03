#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <iostream>
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <string>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <atlconv.h> // Convert to tchar

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
using namespace std;
using std::cout; using std::endl;

std::vector<wstring> SubKeys;
std::vector<wstring> Keys;

std::string ConvertWString(std::wstring arg){
	setlocale(LC_CTYPE, "");

    const std::wstring ws = arg;
    const std::string s( ws.begin(), ws.end() );
	return s;
}
HKEY OpenKey(HKEY hRootKey, wchar_t* strKey)
{
	HKEY hKey;
	LONG nError = RegOpenKeyEx(hRootKey, strKey, NULL, KEY_ALL_ACCESS, &hKey);
	 
	//Added by me
	if(nError == ERROR_SUCCESS){
		return hKey;
	}
	//Added by me (end)
	if (nError==ERROR_FILE_NOT_FOUND)
	{
		cout << "Creating registry key: " << strKey << endl;
		nError = RegCreateKeyEx(hRootKey, strKey, NULL, NULL, REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL, &hKey, NULL);
	}

	if (nError)
		cout << "Error: " << nError << " Could not find or create " << strKey << endl;

	return hKey;
}
void SetVal(HKEY hKey, LPCTSTR lpValue, DWORD data)
{
	LONG nError = RegSetValueEx(hKey, lpValue, NULL, REG_DWORD, (LPBYTE)&data, sizeof(DWORD));

	if (nError)
		cout << "Error: " << nError << " Could not set registry value: " << (char*)lpValue << endl;
}
std::string GetValSTRING(HKEY hKey, LPCTSTR lpValue)
{
	char *value = "";
	DWORD value_size = 0;
	char buf[255];

	 value_size = sizeof(buf);
	memset(buf,0,sizeof(buf));

	LONG nError = RegQueryValueEx(hKey, lpValue, NULL, NULL, (unsigned char*) buf, &value_size);

	std::string Buffer;
	for(int i = 0; i < 255;i++){
		if(buf[i] != '\0'){
			Buffer += buf[i];
		}
	}

	return Buffer;
}
DWORD GetVal(HKEY hKey, LPCTSTR lpValue)
{
	DWORD data;		DWORD size = sizeof(data);	DWORD type = REG_DWORD;
	LONG nError = RegQueryValueEx(hKey, lpValue, NULL, &type, (LPBYTE)&data, &size);

	if (nError==ERROR_FILE_NOT_FOUND)
		data = 0; // The value will be created and set to data next time SetVal() is called.
	else if (nError)
		cout << "Error: " << nError << " Could not get registry value " << (char*)lpValue << endl;

	return data;
}
bool Color(WORD Color, std::string arg){

	HANDLE hstdin  = GetStdHandle( STD_INPUT_HANDLE  );
	HANDLE hstdout = GetStdHandle( STD_OUTPUT_HANDLE );
	WORD   index   = 0;

	// Remember how things were when we started
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( hstdout, &csbi );

	// Tell the user how to stop
	SetConsoleTextAttribute( hstdout, Color );
	std::cout << arg;

	FlushConsoleInputBuffer( hstdin );

	// Keep users happy
	SetConsoleTextAttribute( hstdout, csbi.wAttributes );





	return 0;
}
void QueryKey(HKEY hKey) 
	{ 
	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string 
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys=0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 

	DWORD i, retCode; 

	TCHAR  achValue[MAX_VALUE_NAME]; 
	DWORD cchValue = MAX_VALUE_NAME; 

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

	// Enumerate the subkeys, until RegEnumKeyEx fails.

	if (cSubKeys)
		{
	

		for (i=0; i<cSubKeys; i++) 
			{ 
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hKey, i,
				achKey, 
				&cbName, 
				NULL, 
				NULL, 
				NULL, 
				&ftLastWriteTime); 
			if (retCode == ERROR_SUCCESS) 
				{
					wstring s2 ( achKey );
					SubKeys.push_back(s2);
				}
			}
		} 

	// Enumerate the key values. 

	BYTE* buffer = new BYTE[cbMaxValueData];
	ZeroMemory(buffer, cbMaxValueData);

	if (cValues){


		for (i=0, retCode = ERROR_SUCCESS; i<cValues; i++) 
			{ 
			cchValue = MAX_VALUE_NAME; 
			achValue[0] = '\0'; 
			retCode = RegEnumValue(hKey, i, 
				achValue, 
				&cchValue, 
				NULL, 
				NULL,
				NULL,
				NULL);

			if (retCode == ERROR_SUCCESS ){ 
				
				DWORD lpData = cbMaxValueData;
				buffer[0] = '\0';
				LONG dwRes = RegQueryValueEx(hKey, achValue, 0, NULL, buffer, &lpData);

				wstring s2 ( achValue );
				Keys.push_back(s2);
				} 
			}
		}
	delete [] buffer;
}

int main(){
	

	std::string AccountName; // The account name we are trying to fix.
	bool Error = false;

	cout << "The User Profile Service failed the login fix.\n";
	cout << "Account Name: ";
	cin >> AccountName;
	cout << "Acquiring amount infomation:"; 
	
	HKEY hKey;
	LONG dwRegOPenKey = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\"), 0, KEY_READ, &hKey);
	if(dwRegOPenKey == ERROR_SUCCESS){
		//Worked
		Color(100, "Success");
		cout << ":";
		QueryKey(hKey);

		//All the results we got.
		for(int i = 0; i < SubKeys.size(); i++){
			Sleep(30);
			Color(107, "|");
		}
		cout << ":"; //Spacer
		//Only the ones that are longer then 
		for(int i = 0; i < SubKeys.size(); i++){
			if(SubKeys[i].size() < 10){
				SubKeys.erase(SubKeys.begin() + i);
				i--;
			}else{
				Sleep(30);
				Color(07, "|");
			}
		}

		cout << "\n";
		//We need to clean the keys because of we don't want what is in it.
		Keys.clear();

	} else {
		Color(107, "Failure");
		cout << "\n";
		Error = true;
	}
	RegCloseKey(hKey); // We close no matter what.

	//lists the keys inside a file
	
	//We have all the Subkeys for the users so we need to copy them and test each one.

	std::vector<wstring> ToTestSubKeys;
	for(int i = 0; i < SubKeys.size(); i++){
		ToTestSubKeys.push_back(SubKeys[i]);
	}
	SubKeys.clear();
	
	int WhichTestIsRight = -1;

	for(int i = 0; i < ToTestSubKeys.size();i++){
		//Search through the keys looking for profiles with the right names.
		//Just in care we will clear both of our storeages.
		cout << " SubKey:";
		std::wcout << ToTestSubKeys[i].c_str();
		cout << ":";
		Keys.clear();
		SubKeys.clear();

		HKEY hKey;
		//We want to make where the new subkey is and open it.
		wstring NewSubKeyPath = TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\");
		NewSubKeyPath += ToTestSubKeys[i];
		//NewSubKeyPath += TEXT("\\");

		DWORD dwRegOPenKey = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NewSubKeyPath.c_str(), 0, KEY_READ, &hKey);
		if(dwRegOPenKey == ERROR_SUCCESS){
		//Worked
			Color(100, "Success");
			cout << ":";
			QueryKey(hKey);

			//All the results we got.
			cout << ":"; //Spacer


			std::wstring KeyName = NewSubKeyPath;
			std::string buffer = ConvertWString(KeyName);

			
			//FROM STD::STRING TO WCHAR_T*
			wchar_t* wide_string = new wchar_t[ buffer.length() + 1 ];
			std::copy( buffer.begin(), buffer.end(), wide_string );
			wide_string[ buffer.length() ] = 0;


			static DWORD v1;
			
			HKEY Key = OpenKey(HKEY_LOCAL_MACHINE, wide_string);

			delete [] wide_string; // Clean up. wchar_t

			//v1 = GetVal(Key, L"Sid");
			std::string Getg = GetValSTRING(Key, L"ProfileImagePath");

			cout << "\n   " << Getg;
			//We look for it in the path
			unsigned found = Getg.find( AccountName);
			if (found!=std::string::npos){
				//It's somewhere inside the path HOORAY@
				cout << "-- Account name found";
				WhichTestIsRight = i;
				break;
			}
			cout << "\n";
			

		cout << "\n";

	} else {
		Color(107, "Failure");
		cout << "\n";
		Error = true;
	}


	}
	static DWORD v1, v2;

	if(WhichTestIsRight == -1){
		int notused = 3;
		cout << "Error Unable to find Account name";
		cin >> notused;

		return -1; // Ended because we did not have a account to fix.
	}
	
	//We have all the subkeys and we just need to change the one to work again.
	SetVal(hKey, L"Value1", v1);


	return 0;
}
