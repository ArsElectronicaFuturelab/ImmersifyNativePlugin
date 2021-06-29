#pragma once
#include <string>
#include <sstream>
#include <iostream>

#define DLLExport __declspec(dllexport)

extern "C"
{
	//Create a callback delegate for Unity3d 
	typedef void(*FuncCallBack)(const char* message, int size);
	static FuncCallBack callbackInstance = nullptr;
	//DLLExport void RegisterDebugCallback(FuncCallBack cb);
}

class  Console
{
public:
	static void LogRM(const char* message);
	static void LogRM(std::string message);
	static void RegisterDebugCallback(FuncCallBack cb);
private:
	static void send_log(const std::stringstream &ss);
};
