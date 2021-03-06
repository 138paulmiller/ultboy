#pragma once
#include "../ui/api.hpp"

//TileTools
enum TileToolMode : char
{
	//the correspond to tool bar order!. can be used by APP to get widget
	TILE_TOOL_PENCIL = 0,
	TILE_TOOL_FILL,
	TILE_TOOL_LINE,
	TILE_TOOL_ERASER,
	TILE_TOOL_COUNT,
};

class  TilesetEditor : public Editor 
{
public:
	TilesetEditor();
	void onEnter() override;
	void onExit() override;
	void onTick()override;
	void onKey(Key key , ButtonState state) override;

	void onRedo()override;
	void onUndo()override;
	void onSave()override;

	//allow for runtime update of tilesheet
	void setTileset(const std::string & name, bool isFont = false );
private:
	void commit();
	//draw the propoed changes with mouse at tilex,y
	void drawOverlay(int tilex, int tiley, const Rect & dest);
	
	void reload( const std::string &name );

	UI::TilePicker * m_tilepicker;
	UI::ColorPicker * m_colorpicker;
	std::string m_tilesetName;
	Graphics::Tileset * m_tileset;
	const int m_tileScale = 8;
	
	bool m_usingTool;
	TileToolMode m_tool;
	//shape is data used by the current tool. for line xy = x1y1 wh = x2y2
	Rect m_shapeRect;
	//used to render the potential modifications 
	Graphics::Tileset * m_overlay;

	int m_revision; 
	std::vector<Color * > m_revisionData;
	//
	int m_charW, m_charH;

	bool m_isFont;

};