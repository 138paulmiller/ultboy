#pragma once
#include "core.h"

//Classes that can be used by UI "pages"
namespace Graphics 
{
	class Font;
	class Sheet;
	class Sprite;
	class TextBox;
}

namespace UI
{
	class Widget
	{
	public:
		virtual ~Widget() = default;
		virtual void onUpdate() = 0;
		virtual void onDraw() = 0;
	};

	class Button : public Widget
	{
	public:
		Button();
		virtual ~Button() = default;

		virtual void onClick() = 0;
		virtual void onHover() = 0;

		const Rect & rect();

		std::function<void()> cbClick;
		std::function<void()> cbHover;

	protected: 
		Rect m_rect;
	};

	class App
	{
	public:
		virtual ~App();
		virtual void onEnter() = 0;
		virtual void onExit() = 0;
		virtual void onTick() = 0;
		virtual void onKey(Key key, bool isDown) = 0;
	
		void update();
		void draw();

	protected:
		void clear();


		int addWidget(Widget * widget);
		int addButton(Button * button);
	
		Button * getButton(int idx);
		Widget * getWidget(int idx);

		void removeButton(int idx);
		void removeWidget(int idx);
	
	private:

		std::vector<Widget*> m_widgets;
		std::vector<Button*> m_buttons;
	};


	class TextButton : public Button
	{
	public:
		TextButton(const std::string & text, int x, int y, int tw, int th);
		~TextButton();
		void onUpdate() ;
		void onDraw() ;
		void onHover() ;
		void onClick() ;
		Color hoverTextColor;
		Color hoverColor;
		Color textColor;
		Color color;
		//will render as hover color, clicking again will undo 
		void setUp() ;
		void setDown() ;
	private: 
		bool m_isDirty;
		Graphics::TextBox * m_textbox; 
	};




	/*
		Create Color Selector
			- Renders the sheet at the top-right of the screen.

	*/

	class ColorPicker : public Widget
	{
	public:
		ColorPicker();
		~ColorPicker();

		void onUpdate();
		void onDraw();

		Color color();
	private:
		Graphics::Sheet * m_sheet;
		int m_colorSize;
		Rect m_cursor;
		Rect m_src, m_dest, m_border;
		Color m_color;
	};

	/*
		Sheet Tile/Frame picker
		- Renders the sheet at the bottom of the screen.
	*/
	class SheetPicker : public Widget
	{
	public:
		SheetPicker(const Graphics::Sheet * sheet);
		~SheetPicker();

		const Rect & rect();
		void onUpdate();
		void onDraw();
		void setSheet(const Graphics::Sheet * sheet);
		//get tile/frame relative to given sheet
		Rect selection();
	private:
		const Graphics::Sheet * m_sheet;
		Rect m_cursor;
		//draw  src and dest of texture. 
		// draw top half of texture to the left
		Rect m_srcLeft, m_destLeft;
		Rect m_srcRight, m_destRight;
		//entire box
		Rect m_box;
	};



}

