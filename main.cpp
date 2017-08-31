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
	init_pair(1,COLOR_WHITE, COLOR_BLUE);
	init_pair(2,COLOR_WHITE, COLOR_CYAN);//Words
	init_pair(3,COLOR_WHITE, COLOR_MAGENTA);//Spaces
	init_pair(4,COLOR_WHITE, COLOR_RED);//Tabs
	init_pair(5,COLOR_WHITE, COLOR_GREEN);//special Characters
	init_pair(6,COLOR_WHITE, COLOR_WHITE);//Endline
	refresh();
}

struct vec2D
{
	int x;
	int y;
};

class cTextWindow
{
	public: //change to protected when done debugging!
	vector<string> textBuffer;
	WINDOW* win;
	WINDOW* border;
	vec2D pos;
	vec2D cursorPos;
	vec2D windowPos;
	unsigned int width; //excluding Border
	unsigned int height; //excluding Border
	vec2D origin;
	bool selectMode;
	vec2D selectStart;
	string filePath;
	int tabSpaces;
	
	public:
	cTextWindow()
	{
		pos.y = 5;
		pos.x = 10;
		width = 60;
		height = 15;
		win = newwin(height, width, pos.y, pos.x);
		border = newwin(height+2, width+2, pos.y-1, pos.x-1);
		origin.y =0;
		origin.x =0;
		selectMode = false;
		selectStart.y = 0;
		selectStart.x = 0;
		tabSpaces = 4;
	}
	cTextWindow(int newPosY, int newPosX, int newWidth, int newHeight)
	{
		pos.y = newPosY;
		pos.x = newPosX;
		width = newWidth;
		height = newHeight;
		win = newwin(height, width, pos.y, pos.x);
		border = newwin(height+2, width+2, pos.y-1, pos.x-1);
		origin.y =0;
		origin.x =0;
		selectMode = false;
		selectStart.y = 0;
		selectStart.x = 0;
		tabSpaces = 4;
	}
	void LoadFromFile(string path) //loads all lines from file and strips \n (no error checking)
	{
		ifstream file;
		file.open(path, ios::in);
		if(file)
		{
			textBuffer.clear();
			path = filePath;
		
			string line;
			
			file.seekg(0, ios_base::beg);
			while(!file.eof())
			{
				getline(file, line);
				
				//replace Tabs with spaces--------
				for(unsigned int i = 0; i < line.length(); i++)
				{
					if(line[i] == 9) //ascii 9 = tab
					{
						line[i] = ' ';
						line.insert(i, tabSpaces-1, ' ');
					}
				}
				//-------------------------------
				
				textBuffer.push_back(line);
			}
				

			file.close();
		}
		else
		{
			textBuffer.push_back("this file does not exits");
		}
	}
	void SaveToFile(string path) //Save File, Add \n, replace Spaces with Tabs
	{
		ofstream file;
		file.open(path);
		
		string line;
		for(unsigned int i = 0; i < textBuffer.size(); i++)
		{
			line = textBuffer[i];
			
			int spaceCount = 0;
			for(unsigned int u = 0; u < line.length(); u++)
			{
				if(line[u] == ' ')
				{
					spaceCount++;
				}
				if(spaceCount == tabSpaces)
				{
					spaceCount = 0;
					line[u] = 9;
					line.erase(u+1, tabSpaces-1);
				}
			}
			file.write(line.c_str(), line.length());
		}
		
		file.close();
	}
	void SaveToFile()
	{
		ofstream file;
		file.open(filePath);
		
		string line;
		for(unsigned int i = 0; i < textBuffer.size(); i++)
		{
			line = textBuffer[i];
			
			int spaceCount = 0;
			for(unsigned int u = 0; u < line.length(); u++)
			{
				if(line[u] == ' ')
				{
					spaceCount++;
				}
				if(spaceCount == tabSpaces)
				{
					spaceCount = 0;
					line[u] = 9;
					line.erase(u+1, tabSpaces-1);
				}
			}
			file.write(line.c_str(), line.length());
		}
		
		file.close();
	}
	//void MoveCursorLeft();
	//void MoveCursorRight()
	//void MoveCursorUp();
	//void MoveCursorDown();
	//void MoveCursorWordLeft();
	//void MoveCursorWordRight();
	//void CarriageReturn();
	//void EnableSelect();
	//void DisableSelect();
	//void MoveSelectStartLeft();
	//void MoveSelectStartRight();
	//void MoveSelectStartWordLeft();
	//void MoveSelectStartWordRight();
	void Show()
	{
		wbkgd(border, COLOR_PAIR(1));
		wbkgd(win, COLOR_PAIR(1));
		box(border, 0,0);
		
		wmove(win, 0,0);
		string line;
		for(unsigned int i = origin.y; i < (unsigned int)getmaxy(win)+origin.y && i < textBuffer.size(); i++) //or EOF
		{
			line = textBuffer[i];
			wprintw(win,"%i: %s\n", i, line.c_str());
		}
		
		wmove(win, cursorPos.y, cursorPos.x);
		wrefresh(border);
		wrefresh(win);
	}
};

int main()
{
	
	InitCurses();
	
	cTextWindow textWindow;
	textWindow.LoadFromFile("./main.cpp");
	
	int ch = 0;
	while(ch != 27)
	{
		textWindow.Show();
		
		ch = getch();
			
		
/*		if(ch == KEY_UP)
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
		wrefresh(textWindow.GetWin());*/
		

	}	
	
	//getch();
	endwin();
	return 0;
}
