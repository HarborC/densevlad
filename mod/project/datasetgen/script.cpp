/*
Dataset generation tool to obtain ground truth
(C) Abdul Mohsi Jawaid 2018

THIS FILE IS A MODDED VERSION OF A FILE PART OF GTA V SCRIPT HOOK SDK
http://dev-c.com
(C) Alexander Blade 2015
*/

#include "script.h"
#include "keyboard.h"

#include <string>
#include <ctime>
#include <sstream>
#include <fstream>
#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>

#pragma comment(lib,"gdiplus.lib")

bool switchCamera = false;
DWORD statusTextDrawTicksMax;
std::string textToShow = "MOD ENABLED";
Cam c;
float xOff;
float yOff;
float zOff;

Vector3 gC;
Vector3 gR;
float gFOV;

// makes sure to not use mod while being arrested or killed
void update_features()
{
	while (ENTITY::IS_ENTITY_DEAD(PLAYER::PLAYER_PED_ID()) || PLAYER::IS_PLAYER_BEING_ARRESTED(PLAYER::PLAYER_ID(), TRUE))
		WAIT(0);

	Ped playerPed = PLAYER::PLAYER_PED_ID();
	BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(playerPed);

	if (bPlayerExists) {
		PED::SET_PED_CAN_BE_DRAGGED_OUT(playerPed, false);
	}
}

// gives phone-like notification above the minimap on screen
void notifyAboveMap(char* msg) {
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(msg);
	UI::_DRAW_NOTIFICATION(FALSE, FALSE);
}


// returns if F4 pressed
bool modSwitched() {
	return IsKeyJustUp(VK_F4);
}

// function to captured image info
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	// credit to https://stackoverflow.com/questions/24644709/c-hbitmap-bitmap-into-bmp-file
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure
	
	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}
	free(pImageCodecInfo);
	return -1;  // Failure
}

// function to save the captured image to folder
void saveHBitmap(HBITMAP hBitmap,char* name) {
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	Gdiplus::Bitmap *image = new Gdiplus::Bitmap(hBitmap, NULL);

	CLSID myClsId;
	int retVal = GetEncoderClsid(L"image/bmp", &myClsId);

	std::wstring w;
	std::copy(name, name + strlen(name), back_inserter(w));
	const WCHAR *pwcsName = w.c_str();

	image->Save(pwcsName, &myClsId, NULL);
	delete image;

	Gdiplus::GdiplusShutdown(gdiplusToken);
}

// captures the screen by calling the helper functions to filename: name
void screenCapture(char* name) {

	// credit to https://stackoverflow.com/questions/3291167/how-can-i-take-a-screenshot-in-a-windows-application
	// get the device context of the screen
	HDC hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	// and a device context to put it in
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

	int width = GetDeviceCaps(hScreenDC, HORZRES);
	int height = GetDeviceCaps(hScreenDC, VERTRES);

	// maybe worth checking these are positive values
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);

	// get a new bitmap
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

	BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemoryDC, hOldBitmap);

	// clean up
	DeleteDC(hMemoryDC);
	DeleteDC(hScreenDC);

	// now to save the image held in hBitmap
	saveHBitmap(hBitmap,name);
	
}

// displays specified text in the middle of the screen
void displayText(std::string caption) {
	if (GetTickCount() < statusTextDrawTicksMax) {
		UI::SET_TEXT_FONT(0);
		UI::SET_TEXT_SCALE(0.55, 0.55);
		UI::SET_TEXT_COLOUR(255, 255, 255, 255);
		UI::SET_TEXT_WRAP(0.0, 1.0);
		UI::SET_TEXT_CENTRE(1);
		UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		UI::SET_TEXT_EDGE(1, 0, 0, 0, 205);
		UI::_SET_TEXT_ENTRY("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
		UI::_DRAW_TEXT(0.5, 0.5);
	}
}

// teleport function when you know the exact coordinates of the location 
// and don't need to check for the ground-z coordinate.
void teleportToLocationNoGroundCheck(float x ,float y ,float z) {
	Entity e = PLAYER::PLAYER_PED_ID();
	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, x, y, z , 0, 0, 1);
	WAIT(10);
	/*
	DWORD time = 2500;
	statusTextDrawTicksMax = GetTickCount() + time;
	textToShow = "TELEPORTED";
	*/
}


// teleports to location specified by coords while checking where the ground is
void teleportToLocation(Vector3 coords) {
	Entity e = PLAYER::PLAYER_PED_ID();
	bool groundFound = false;
	static float groundCheckHeight[] = {
		100.0, 150.0, 50.0, 0.0, 200.0, 250.0, 300.0, 350.0, 400.0,
		450.0, 500.0, 550.0, 600.0, 650.0, 700.0, 750.0, 800.0
	};
	for (int i = 0; i < sizeof(groundCheckHeight) / sizeof(float); i++) {
		if (GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, groundCheckHeight[i], &coords.z, FALSE)) {
			groundFound = true;
			break;
		}
	}
	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, coords.x, coords.y, coords.z+0.9, 0, 0, 1);
	WAIT(0);
	DWORD time = 2500;
	statusTextDrawTicksMax = GetTickCount() + time;
	textToShow = "TELEPORTED";
}

// testing function to teleport a birt forward
void teleportForward() {
	Entity e = PLAYER::PLAYER_PED_ID();
	Vector3 c = ENTITY::GET_ENTITY_COORDS(e, true);
	c.x = c.x + 2;
	teleportToLocation(c);
}

void jumpForward() {
	if (IsKeyJustUp(VK_NUMPAD1)) {
		teleportForward();
	}
}

bool toggleCamera() {
	return IsKeyJustUp(VK_F10);
}

// switches between game camera and virtual camera
void setCameraCoordinates() {
	if (switchCamera) {
		// switch to virtual user-defined camera
		gC = CAM::GET_GAMEPLAY_CAM_COORD();
		gR = CAM::GET_GAMEPLAY_CAM_ROT(0);
		gFOV = CAM::GET_GAMEPLAY_CAM_FOV();
		CAM::SET_CAM_COORD(c, gC.x+xOff, gC.y+yOff, gC.z + 50+zOff);
		CAM::SET_CAM_ROT(c, gR.x, gR.y, gR.z, 0);
		CAM::SET_CAM_FOV(c, gFOV);
		CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0);
	}
	else {
		// switch to gameplay camera
		CAM::RENDER_SCRIPT_CAMS(0, 0, 3000, 1, 0);
	}

}

// writes the current player coordinates and gameplay camera location to a file.
// also captures the screen and saves to a .bmp file
void showPlayerCoordinates() {
	if (IsKeyJustUp(VK_NUMPAD0)) {
		DWORD time = 2500;
		statusTextDrawTicksMax = GetTickCount() + time;
		std::string coords = "";
		Entity e = PLAYER::PLAYER_PED_ID();
		Vector3 c = ENTITY::GET_ENTITY_COORDS(e, true);
		Vector3 r = CAM::GET_GAMEPLAY_CAM_ROT(0);
		float fv = CAM::GET_GAMEPLAY_CAM_FOV();
		std::stringstream s;
		s.str("");
		s << "{" << c.x << ", " << c.y << ", " << c.z << ", " << r.x << ", " << r.y << ", " << r.z << "," << fv << "},";
		std::ofstream f;
		f.open("coords.txt", std::ios::app | std::ios::out);
		f << s.str() << std::endl;
		f.close();
		screenCapture("output.bmp");
		textToShow = s.str();
	}
}

// keys to control the virtual user-defined camera
void updateOffsets() {
	if (IsKeyDown(VK_NUMPAD8)) {
		yOff += 1;
	}
	if (IsKeyDown(VK_NUMPAD2)) {
		yOff -= 1;
	}
	if (IsKeyDown(VK_NUMPAD6)) {
		xOff += 1;
	}
	if (IsKeyDown(VK_NUMPAD4)) {
		xOff -= 1;
	}
	if (IsKeyDown(VK_NUMPAD7)) {
		zOff += 1;
	}
	if (IsKeyDown(VK_NUMPAD9)) {
		zOff -= 1;
	}
}

void captureDataset() {
	static struct {
		float cx;
		float cy;
		float cz;
		float rx;
		float ry;
		float rz;
		float fov;
	} camInit[14] = {
		{50.4939, -1497.96, 29.2918, 10.9893, 0.0577853, -5.20431,50},
		{122.106, -1446, 29.3212, 7.07328, 0.147676, -149.194,50},
		{23.6355, -1139.56, 29.2659, 10.9277, 0.118116, 11.8403,50},
		{42.4079, -151.113, 55.3849, 11.5501, 0.0517797, -28.5407,50},
		{140.505, -192.38, 54.5084, 12.5269, 0.346575, 151.551,50},
		{251.076, -354.972, 44.5451, 22.1798, 0.050242, 163.503,50},
		{431.8, -153.019, 63.7157, 22.3034, 0.143301, -39.942,50},
		{281.778, -78.816, 70.1712, 14.6755, 0.269785, 34.7926,50},
		{-254.151, -657.513, 33.2058, 11.3522, 0.267428, 48.3141,50},
		{-1713.58, -1080.29, 13.0174, 16.8185, 0.178879, -133.481,50},
		{-1289.12, -501.332, 33.1679, 17.116, 0.0518984, -88.1658,50},
		{-1321.43, -524.423, 32.7932, 20.2434, 0.220766, 52.1893,50},
		{-1453.83, -622.527, 30.7684, 22.1241, 0.162537, 48.4944,50},
		{ -88.1479, -255.713, 44.8065, 13.4322, 0.0517441, 95.2865,50 }

	};
	std::stringstream s;
	int xOff = -20;
	int yOff = -20;
	int zOff = -20;
	for (int i = 0;i < 5;i++) {
		teleportToLocationNoGroundCheck(camInit[i].cx, camInit[i].cy, camInit[i].cz);
		// wait for textures to load
		WAIT(1000);
		gC = CAM::GET_GAMEPLAY_CAM_COORD();
		gFOV = CAM::GET_GAMEPLAY_CAM_FOV();
		CAM::SET_CAM_COORD(c, gC.x, gC.y, gC.z);
		CAM::SET_CAM_ROT(c, camInit[i].rx + xOff, camInit[i].ry + yOff, camInit[i].rz + zOff, 0);
		CAM::SET_CAM_FOV(c, camInit[i].fov);
		CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0);
		WAIT(1000);
		xOff = -20;
		for (int j = 0;j < 5;j++) {
			yOff = -20;
			for (int k = 0;k < 5;k++) {
				zOff = -20;
				for (int l = 0;l < 5;l++) {
					CAM::SET_CAM_ROT(c, camInit[i].rx + xOff, camInit[i].ry + yOff, camInit[i].rz + zOff, 0);
					CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0);
					zOff += 10;
					WAIT(100);
					s.str("");
					s << "dataset/output" << i << j << k << l << ".bmp";
					screenCapture((char*)(s.str().c_str()));
				}
				yOff += 10;
			}
			xOff += 10;
		}
		
	}
	CAM::RENDER_SCRIPT_CAMS(0, 0, 3000, 1, 0);
}


void generateDataset() {
	if (IsKeyDown(VK_F9)) {
		captureDataset();
	}
}

// to make sure time stays same for each capture
void setTime(int h, int m, int s) {
	TIME::SET_CLOCK_TIME(h, m, s);
}

// to make sure weather stays same for each data point
void setWeather() {
	GAMEPLAY::SET_WIND(0.0);
	GAMEPLAY::SET_WIND_SPEED(0.0);
	GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST((char *)"CLEAR");
}

void main()
{
	bool modEnabled = false;
	xOff = 0;
	yOff = 0;
	zOff = 0;
	while (true)
	{	
		if (modSwitched()) {
			DWORD time = 2500;
			statusTextDrawTicksMax = GetTickCount() + time;
			modEnabled = !modEnabled;
			if (modEnabled) {
				setTime(12, 0, 0);
				setWeather();
				gC = CAM::GET_GAMEPLAY_CAM_COORD();
				gR = CAM::GET_GAMEPLAY_CAM_ROT(0);
				gFOV = CAM::GET_GAMEPLAY_CAM_FOV();
				c = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", gC.x + xOff, gC.y + yOff, gC.z + 50 + zOff, gR.x, gR.y, gR.z, gFOV, true, 2);
				textToShow = "MOD ENABLED";
			}
			else {
				GAMEPLAY::CLEAR_WEATHER_TYPE_PERSIST();
				CAM::DESTROY_ALL_CAMS(true);
				textToShow = "MOD DISABLED";
			}
		}
		update_features();
		if (modEnabled) {
			// sets time to mid day while the mod is enabled
			setTime(12, 0, 0);
			showPlayerCoordinates();
			jumpForward();
			generateDataset();
			if (toggleCamera()) {
				switchCamera = !switchCamera;
				if (switchCamera) {
					gC = CAM::GET_GAMEPLAY_CAM_COORD();
					gR = CAM::GET_GAMEPLAY_CAM_ROT(0);
					gFOV = CAM::GET_GAMEPLAY_CAM_FOV();
					CAM::SET_CAM_COORD(c, gC.x+xOff, gC.y+yOff, gC.z+50+zOff);
					CAM::SET_CAM_ROT(c, gR.x, gR.y, gR.z, 0);
					CAM::SET_CAM_FOV(c, gFOV);
					statusTextDrawTicksMax = GetTickCount() + 2500;
					textToShow = "CAMERA SET TO SCRIPTED";
				}
				else {
					statusTextDrawTicksMax = GetTickCount() + 2500;
					textToShow = "CAMERA SET TO GAMEPLAY";
				}
			}
			setCameraCoordinates();
			updateOffsets();
		}
		displayText(textToShow);
		WAIT(0);
	}

}

void ScriptMain()
{
	srand(GetTickCount());
	main();
}