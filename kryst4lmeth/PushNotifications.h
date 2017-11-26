#pragma once

#include <string>
#include <vector>

struct Notification
{
	std::string Message = "";
	int Ticks = 1024;
};

namespace PushNotifications
{	
	void Update();
	void Push(std::string Message);
};

