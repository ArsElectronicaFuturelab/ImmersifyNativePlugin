#include "Console.h"

using namespace std;
//Shows C++ LOGs in Unity3d

//-------------------------------------------------------------------
void  Console::LogRM(const char* message) {
	if (callbackInstance != nullptr)
		callbackInstance(message, (int)strlen(message));
	else
		cout << message << endl;
}

//-------------------------------------------------------------------
void  Console::LogRM(std::string message) {
	if (callbackInstance != nullptr)
		callbackInstance(message.c_str(), (int)strlen(message.c_str()));
	else
		cout << message << endl;
}

void Console::send_log(const std::stringstream &ss) {
	const std::string tmp = ss.str();
	const char* tmsg = tmp.c_str();
	if (callbackInstance != nullptr)
		callbackInstance(tmsg, (int)strlen(tmsg));
	else
		cout << tmp << endl;
}
//-------------------------------------------------------------------

//Create a callback delegate
void Console::RegisterDebugCallback(FuncCallBack cb) {
	callbackInstance = cb;
}