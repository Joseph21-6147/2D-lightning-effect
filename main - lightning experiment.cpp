// Lightning experiment
//
// Idea: Games28
// Code: Joseph21

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using namespace std;

class LightningExperiment : public olc::PixelGameEngine {

public:
    LightningExperiment() {
        sAppName = "LightningExperiment";
    }

private:
    olc::vi2d  start_point, end_point;

public:
    bool OnUserCreate() override {
        // define both endpoints of the lightning (in screen coordinates)
        start_point = { (int)((float)ScreenWidth() * 0.10f), (int)((float)ScreenHeight() * 0.10f) };
        end_point   = { (int)((float)ScreenWidth() * 0.90f), (int)((float)ScreenHeight() * 0.90f) };

        return true;
    }

    // simple line drawing algo - using FillCircle() calls instead of Draw() pixel calls
    // and creates thicker lines that way
    void MyDrawLine( int x1, int y1, int x2, int y2, olc::Pixel col, int thickness ) {
        if (abs(x2 - x1) >= abs(y2 - y1)) {      // increment x with steps of 1
            if (x2 < x1) {
                std::swap( x1, x2 );             // make sure we iterate from left to right
                std::swap( y1, y2 );
            }
            float increment = (float)(y2 - y1) / (float)(x2 - x1);
            for (int i = 0; i <= (x2 - x1); i++) {
                FillCircle( x1 + i, y1 + (int)(i * increment), thickness, col );
            }
        } else {                                 // increment y with steps of 1
            if (y2 < y1) {
                std::swap( x1, x2 );             // make sure we iterate from left to right
                std::swap( y1, y2 );
            }
            float increment = (float)(x2 - x1) / (float)(y2 - y1);
            // draw thick line using circles iso pixels
            for (int i = 0; i <= (y2 - y1); i++) {
                FillCircle( x1 + (int)(i * increment), y1 + i, thickness, col );
            }
        }
    }

    void DrawThickLine( int x1, int y1, int x2, int y2, olc::Pixel col, int thickness ) {
        // variate thickness a little
        int tmpThickness = std::max( 1, thickness - (rand() % 3));

        if (tmpThickness <= 1)
            DrawLine( x1, y1, x2, y2, col );
        else if (tmpThickness > 1)
            MyDrawLine( x1, y1, x2, y2, col, tmpThickness );
    }

    // overloaded version to accept olc::vi2d arguments as first and second point
    void DrawThickLine( olc::vi2d fstPoint, olc::vi2d scdPoint, olc::Pixel col, int thickness ) {
        DrawThickLine( fstPoint.x, fstPoint.y, scdPoint.x, scdPoint.y, col, thickness );
    }

    // Draws a randomized lightning pattern between firstPoint and lastPoint, by calculating a
    // randomized mid point to create two subsegments, and recursively calling itself on the two subsegments
    // The recursion depth is controlled by parameter nRecDepth - if nRecDepth == 0 then straight line is result
    // NOTE: the function is O(2^n) w.r.t. the recursion depth, so don't dig too deep :)
    void DrawRandomLightning( olc::vi2d &firstPoint, olc::vi2d &lastPoint, olc::Pixel lineCol = olc::CYAN, int nRecDepth = 5, int nLineWidth = 5, float fPerc = 0.1f, float fMin = 5.0f) {
        // lambda returns a random float nr in range [fMinValue, fMaxValue]
        auto RandFloatBetween = [=]( float fMinValue, float fMaxValue ) {
            return ((float)rand() / (float)RAND_MAX) * (fMaxValue - fMinValue) + fMinValue;
        };

        if (nRecDepth == 0) {                  // render line segment of lightning
            DrawThickLine( firstPoint, lastPoint, lineCol, nLineWidth );
        } else if (nRecDepth > 0) {            // determine some random point somewhere in the middle.
            int nDeltaX = lastPoint.x - firstPoint.x;
            int nDeltaY = lastPoint.y - firstPoint.y;

            olc::vi2d midPoint = {
                firstPoint.x + nDeltaX / 2 + (int)(RandFloatBetween( -1.0f, +1.0f ) * max( fMin, fPerc * (float)nDeltaX )),
                firstPoint.y + nDeltaY / 2 + (int)(RandFloatBetween( -1.0f, +1.0f ) * max( fMin, fPerc * (float)nDeltaY ))
            };
            // recursive calls for subsegments
            DrawRandomLightning( firstPoint,  midPoint, lineCol, nRecDepth - 1, nLineWidth, fPerc, fMin );
            DrawRandomLightning(   midPoint, lastPoint, lineCol, nRecDepth - 1, nLineWidth, fPerc, fMin );
        }
    }

    int recursionDepth = 5;     // # of times that lightning is halved so its O(2^recursionDepth) !!
    int lineThickness  = 5;

    float fPercentage = 0.1f;   // percentage of delta X resp. Y to be used for randomizaton of x and y values
    float fMinimum    = 5.0f;   // if delta X resp Y is below this minimum, then use this minimum

    olc::Pixel cColour = olc::CYAN;

    bool OnUserUpdate( float fElapsedTime ) override {

        auto key_touched = [=]( olc::Key k ) {
            return GetKey( k ).bPressed || GetKey( k ).bHeld;
        };

        if (GetKey( olc::Key::NP_ADD ).bPressed) { recursionDepth += 1;                                                }
        if (GetKey( olc::Key::NP_SUB ).bPressed) { recursionDepth -= 1; if (recursionDepth < 0   ) recursionDepth = 0; }

        if (GetKey( olc::Key::NP_MUL ).bPressed) { lineThickness += 1;                                                }
        if (GetKey( olc::Key::NP_DIV ).bPressed) { lineThickness -= 1; if (lineThickness < 0   ) lineThickness = 0; }

        if (key_touched( olc::Key::RIGHT  )) { fPercentage    += 0.0001f;                                                   }
        if (key_touched( olc::Key::LEFT   )) { fPercentage    -= 0.0001f; if (fPercentage    < 0.0f) fPercentage    = 0.0f; }
        if (key_touched( olc::Key::UP     )) { fMinimum       += 0.1f;                                                    }
        if (key_touched( olc::Key::DOWN   )) { fMinimum       -= 0.1f;  if (fMinimum       < 0.0f) fMinimum       = 0.0f; }

        if (key_touched( olc::Key::F1 )) { cColour = olc::GREY;    }
        if (key_touched( olc::Key::F2 )) { cColour = olc::RED;     }
        if (key_touched( olc::Key::F3 )) { cColour = olc::YELLOW;  }
        if (key_touched( olc::Key::F4 )) { cColour = olc::GREEN;   }
        if (key_touched( olc::Key::F5 )) { cColour = olc::CYAN;    }
        if (key_touched( olc::Key::F6 )) { cColour = olc::BLUE;    }
        if (key_touched( olc::Key::F7 )) { cColour = olc::MAGENTA; }
        if (key_touched( olc::Key::F8 )) { cColour = olc::WHITE;   }

        Clear( olc::BLACK );

        DrawRandomLightning( start_point, end_point, cColour, recursionDepth, lineThickness, fPercentage, fMinimum );

        DrawCircle( start_point, 2, olc::YELLOW );
        DrawCircle(   end_point, 2, olc::YELLOW );

        DrawString( 0,  0, "<+><-> Recursion depth: " + std::to_string( recursionDepth ));
        DrawString( 0, 10, "</><*> Line thickness : " + std::to_string( lineThickness  ));
        DrawString( 0, 20, "<-  -> Percentage     : " + std::to_string( fPercentage    ));
        DrawString( 0, 30, "<^><v> Minimum        : " + std::to_string( fMinimum       ));
        DrawString( 0, 40, "<F1><F8>  Colour" );

        return true;
    }
};

// let the screen dimensions be constant and vary the resolution by
// adapting the pixel size
#define SCREEN_X   1024
#define SCREEN_Y    800
#define PIXEL_X       1
#define PIXEL_Y       1

int main()
{
	LightningExperiment demo;
	if (demo.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y ))
		demo.Start();

	return 0;
}

