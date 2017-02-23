#define _CRT_SECURE_NO_WARNINGS

#include "Common.h"
#include "command.h"
#include "Trade.h"
#include "Query.h"

#include <conio.h>
#include <sstream>

COORD command::coord;
char command::buffer[1024] = { 0 };
char* command::pbuf = buffer;
bool command::can_input = true;
std::thread* command::Analyzer = nullptr;

void command::thread_work()
{
	while (true)
	{
		if (char c = _getch())
		{
			if (!can_input)
				continue;
			_Get_Output_Mutex
			StoreCursorPosition(&coord);
			SetCursorPosition(pbuf - buffer + 12, BUFFER_ROW_NUM - 2);
			switch (c)
			{
				case VK_RETURN:
					{
						can_input = false;
						int len = (int)(pbuf - buffer);
						
						for (int i = 0;i < len; i++)
						{
							putchar('\b');
						}
						for (int i = 0;i < len; i++)
						{
							putchar(' ');
						}
						for (int i = 0;i < len; i++)
						{
							putchar('\b');
						}
						*pbuf = 0;
						Analyzer = new std::thread(analyze_command);
					}
					break;
				case VK_BACK:
					{
						if (pbuf>buffer)
						{
							pbuf--;
							puts("\b \b");
						}
					}
					break;
				default:
					{
						if (pbuf - buffer < 50)
						{
							*(pbuf++) = c;
							putchar(c);
						}
						else
							break;
					}
					break;
			}
			RestoreCursorPosition(&coord);
			_Release_Output_Mutex
		}
	}
}

/*
* ���
*   h                ����
*   cl               �����Ļ
*   b [num]          ��num��
*   s [num]          ��num��
*   cb [num]         ƽ��num��
*   cs [num]         ƽ��num��
*   t                �鿴���ж���
*   q                ֹͣ�Զ�����
*   rs               �����£���ʼ�Զ�����
*   exit             �˳�����
*/

void command::analyze_command()
{
	while (!Analyzer);
	char cmd[1024];
	strcpy(cmd, buffer);
	pbuf = buffer;
	can_input = true;

	std::string cmdstr = cmd;
	lgr << Log::t << "[cm]Command Received (" << cmd << ")" << Log::endl;

	if (cmdstr == "h")
	{
		lgr << Log::t << "[cm]b [num]          ��num��" << Log::endl
			<< Log::t << "[cm]s [num]          ��num��" << Log::endl
			<< Log::t << "[cm]cb [num]         ƽ��num��" << Log::endl
			<< Log::t << "[cm]cs [num]         ƽ��num��" << Log::endl
			<< Log::t << "[cm]t                �鿴���ж���" << Log::endl
			<< Log::t << "[cm]q                ֹͣ�Զ�����" << Log::endl
			<< Log::t << "[cm]rs               �����£���ʼ�Զ�����" << Log::endl
			<< Log::t << "[cm]exit             �˳�����" << Log::endl;
	}
	
	else if (cmdstr == "cl")
	{
		system("cls");
		InitScreen();
	}
	
	else if (cmdstr[0] == 'b')
	{
		if (cmdstr[1] == ' ')
		{
			try
			{
				std::stringstream ss(cmd + 2);
				int lot = 0;
				ss >> lot;
				OrderSend(ppInstrumentID[0], THOST_FTDC_OF_Open, THOST_FTDC_D_Buy, lot);
			}

			catch (std::exception)
			{
				lgr << Log::t << "[cm]Illegal Command" << Log::endl;
			}
		}
		else
			lgr << Log::t << "[cm]Illegal Command" << Log::endl;
	}

	else if (cmdstr[0] == 's')
	{
		if (cmdstr[1] == ' ')
		{
			try
			{
				std::stringstream ss(cmd + 2);
				int lot = 0;
				ss >> lot;
				OrderSend(ppInstrumentID[0], THOST_FTDC_OF_Open, THOST_FTDC_D_Sell, lot);
			}

			catch (std::exception)
			{
				lgr << Log::t << "[cm]Illegal Command" << Log::endl;
			}
		}
		else
			lgr << Log::t << "[cm]Illegal Command" << Log::endl;
	}

	else if (cmdstr[0] == 'c')
	{
		if (cmdstr[1] == 'b')
		{
			try
			{
				std::stringstream ss(cmd + 3);
				int lot = 0;
				ss >> lot;
				OrderSend(ppInstrumentID[0], THOST_FTDC_OF_CloseToday, THOST_FTDC_D_Sell, lot);
			}

			catch (std::exception)
			{
				lgr << Log::t << "[cm]Illegal Command" << Log::endl;
			}
		}
		else if (cmdstr[1] == 's')
		{
			try
			{
				std::stringstream ss(cmd + 3);
				int lot = 0;
				ss >> lot;
				OrderSend(ppInstrumentID[0], THOST_FTDC_OF_CloseToday, THOST_FTDC_D_Buy, lot);
			}

			catch (std::exception)
			{
				lgr << Log::t << "[cm]Illegal Command" << Log::endl;
			}
		}
		else
			lgr << Log::t << "[cm]Illegal Command" << Log::endl;
	}

	else if (cmdstr == "accinfo")
	{
		tdspi->ReqQryTradingAccount();
	}

	else if (cmdstr == "exit")
	{
		// TODO: Safely terminate the program.
	}

	else
		lgr << Log::t << "[cm]Illegal Command" << Log::endl;
	Analyzer = nullptr;
}