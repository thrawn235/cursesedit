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
	WINDOW* numberArea;
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
	short mode;
	#define MODE_WORD_WRAP 0
	#define MODE_LINE_SCROLLING 1
	#define MODE_2D_SCROLLING 2
	
	public:
	cTextWindow()
	{
		pos.y = 5;
		pos.x = 10;
		width = 60;
		height = 15;
		win = newwin(height, width, pos.y, pos.x);
		border = newwin(height+2, width+10, pos.y-1, pos.x-9);
		numberArea = newwin(height, 7, pos.y, pos.x-8);
		origin.y =0;
		origin.x =0;
		selectMode = false;
		selectStart.y = 0;
		selectStart.x = 0;
		tabSpaces = 4;
		mode = MODE_2D_SCROLLING;
		wmove(win, 0,0);
		cursorPos.y = 0;
		cursorPos.x = 0;
	}
	cTextWindow(int newPosY, int newPosX, int newWidth, int newHeight)
	{
		pos.y = newPosY;
		pos.x = newPosX;
		width = newWidth;
		height = newHeight;
		win = newwin(height, width, pos.y, pos.x);
		border = newwin(height+2, width+10, pos.y-1, pos.x-9);
		numberArea = newwin(height, 7, pos.y, pos.x-8);
		origin.y =0;
		origin.x =0;
		selectMode = false;
		selectStart.y = 0;
		selectStart.x = 0;
		tabSpaces = 4;
		mode = MODE_2D_SCROLLING;
		wmove(win, 0,0);
		cursorPos.y = 0;
		cursorPos.x = 0;
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
		wmove(win, 0,0);
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
	void MoveCursorLeft()
	{
		cursorPos.x--;
		if(cursorPos.x <= 0)
		{
			cursorPos.x = 0;
		}
	}
	void MoveCursorRight()
	{
		cursorPos.x++;
		int longest = GetLongestLine();
		if(cursorPos.x > longest)
		{
			cursorPos.x = longest;
		}
	}
	void MoveCursorUp()
	{
		cursorPos.y--;
		if(cursorPos.y < 0)
		{
			cursorPos.y = 0;
		}
	}
	void MoveCursorDown()
	{
		cursorPos.y++;
		if(cursorPos.y > textBuffer.size())
		{
			cursorPos.y = textBuffer.size();
		}
	}
	//void MoveCursorWordLeft();
	//void MoveCursorWordRight();
	//void CarriageReturn();
	//void EnableSelect();
	//void DisableSelect();
	//void MoveSelectStartLeft();
	//void MoveSelectStartRight();
	//void MoveSelectStartWordLeft();
	//void MoveSelectStartWordRight();
	int GetLongestLine()
	{
		unsigned int length = 0;
		for(unsigned int i = 0; i < textBuffer.size(); i++)
		{
			if(textBuffer[i].length() > length)
			{
				length = textBuffer[i].length();
			}
		}
		return length;
	}
	void Show()
	{
		wbkgd(border, COLOR_PAIR(1));
		wbkgd(win, COLOR_PAIR(1));
		wbkgd(numberArea, COLOR_PAIR(2));
		box(border, 0,0);
		
		//set Origin according to Cursorpos
		if(cursorPos.x < origin.x )
		{
				origin.x = cursorPos.x -5;
				if(origin.x < 0)
				{
					origin.x = 0;
				}
		}
		if(cursorPos.x >= origin.x + width)
		{
			origin.x = cursorPos.x -width +5;
			if(origin.x > GetLongestLine() - width)
			{
				origin.x = GetLongestLine() - width;
			}
		}
		if(cursorPos.y < origin.y)
		{
			origin.y = cursorPos.y;
		}
		if(cursorPos.y >= origin.y + height)
		{
			origin.y = cursorPos.y - height;
		}
		
		wmove(win, 0,0);
		string line;
		for(unsigned int i = origin.y; i < (unsigned int)getmaxy(win)+origin.y && i < textBuffer.size(); i++) //or EOF
		{
			line = textBuffer[i];
			line.erase(0, origin.x);
			if(line.length() >= width)
			{
				line.erase(width, line.length() - width);
			}
			wprintw(numberArea, "%4i: \n", i);
			wprintw(win,"%s\n", line.c_str());
		}
		
		wmove(win, cursorPos.y - origin.y, cursorPos.x - origin.x);
		wrefresh(border);
		wrefresh(numberArea);
		wrefresh(win);
	}
};

int main()
{
	
	InitCurses();
	
	cTextWindow textWindow;
	textWindow.LoadFromFile("./main.cpp");
	
	unsigned int ch = 0;
	while(ch != 27)
	{
		
		
		ch = getch();
		
		if(ch == KEY_LEFT)
		{
			textWindow.MoveCursorLeft();
		}
		else if(ch == KEY_RIGHT)
		{
			textWindow.MoveCursorRight();
		}
		else if(ch == KEY_UP)
		{
			textWindow.MoveCursorUp();
		}
		else if(ch == KEY_DOWN)
		{
			textWindow.MoveCursorDown();
		}
		
		textWindow.Show();	
	}
	
	//getch();
	endwin();
	return 0;
}
