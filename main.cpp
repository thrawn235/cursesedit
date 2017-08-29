#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ncurses.h>
using namespace std;

void InitCurses()
{
	initscr();
	raw();
	keypad(stdscr, true);
	noecho();
	start_color();
	init_pair(1,COLOR_BLUE, COLOR_RED);
	refresh();
}

class cTextBuffer
{
	protected:
	vector<string> text;
	
	public:
	cTextBuffer()
	{
	}
	void LoadFromFile(string path)
	{
		ifstream file;
		file.open(path);
		
		string line;
		
		file.seekg(0, ios_base::beg);
		while(!file.eof())
		{
			getline(file, line);
			text.push_back(line);
		}
			

		file.close();
	}
	string GetLine(unsigned int lineIndex)
	{
		if(lineIndex < text.size())
		{
			return text[lineIndex];
		}
		return "";
	}
	int getNumLines()
	{
		return text.size();
	}
	//int getLength();
};

class cTextWindow
{
	protected:
	int posX;
	int posY;
	WINDOW* win;
	WINDOW* border;
	int width;
	int height;
	int originY;
	int originX;
	cTextBuffer* buffer;
	
	public:
	cTextWindow(int newPosY, int newPosX, int newWidth, int newHeight)
	{
		buffer = new cTextBuffer;
		posX = newPosX;
		posY = newPosY;
		width = newWidth;
		height = newHeight;
		win = newwin(height, width, posY, posX);
		border = newwin(height+2, width+2, posY-1, posX-1);
		originY = 0;
		originX = 0;
		wbkgd(win, COLOR_PAIR(1));
		wbkgd(border, COLOR_PAIR(1));
		box(border, 0,0);
	}
	void Show()
	{
		wmove(win, 0,0);
		for(int i = originY; i< getmaxy(win)+originY; i++) //or EOF
		{
			wprintw(win,"%i: %s\n", i, buffer->GetLine(i).c_str());
		}
		wmove(win, 0,0);
		
		wrefresh(border);
		//wrefresh(win);
	}
	int GetOriginY()
	{
		return originY;
	}
	void SetOriginY(int newOriginY)
	{
		originY = newOriginY;
	}
	void LoadFromFile(string path)
	{
		buffer->LoadFromFile(path);
	}
	WINDOW* GetWin()
	{
		return win;
	}
	int GetHeight()
	{
		return height;
	}
};


int main()
{
	
	InitCurses();
	
	cTextWindow textWindow(5,5, 40, 10);
	textWindow.LoadFromFile("main.cpp");
	
	int ch = 0;
	int cursorPosX = 0;
	int cursorPosY = 0;
	while(ch != 27)
	{
		ch = getch();
		
		
		
		
		
		if(ch == KEY_UP)
		{
			if(cursorPosY <= 0)
			{
				textWindow.SetOriginY(textWindow.GetOriginY()-1);
			}
			else
			{
				cursorPosY--;
			}
		}
		else if(ch == KEY_DOWN)
		{
			if(cursorPosY >= textWindow.GetHeight()-1)
			{
				textWindow.SetOriginY(textWindow.GetOriginY()+1);
			}
			else
			{
				cursorPosY++;
			}
		}
		else if(ch == KEY_LEFT)
		{
			cursorPosX--;
		}
		else if(ch == KEY_RIGHT)
		{
			cursorPosX++;
		}
		textWindow.Show();
		
		wmove(textWindow.GetWin(), cursorPosY, cursorPosX);
		wrefresh(textWindow.GetWin());
		

	}	
	
	
	
	
	
	
	
	
	
	getch();
	endwin();
	return 0;
}
