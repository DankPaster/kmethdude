#include "PushNotifications.h"
#include "DrawTools.h"

std::vector<Notification> Notifications;

void PushNotifications::Update()
{
	int i = 0;
	while (i < Notifications.size())
	{
		if (Notifications[i].Ticks >= 0)
		{
			if (i < 8)
				DrawTools::DrawStringOutlined(Font[FONT_ESP], Vector2D(24, 340 + i * 14), Color(217, 237, 0, 255), 0, Notifications[i].Message.c_str());
			Notifications[i].Ticks--;
			i++;
		}
		else
			Notifications.erase(Notifications.begin() + i);
	}
}

void PushNotifications::Push(std::string Message)
{
	Notification n;
	n.Message = Message;
	Notifications.push_back(n);
}