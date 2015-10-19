//
//  SimpleScene.h
//  simpleExample
//
//  Created by Ben McChesney on 5/15/14.
//
//

#pragma once

#include "BaseScene.h"
#include "ofxGif.h"


class AnimatedTileTextureScene : public BaseScene
{
public :
	AnimatedTileTextureScene() {}
	AnimatedTileTextureScene( int a_index, string a_name ):BaseScene( a_index, a_name ) {}

    void setup();
    void setupGui(float a_x=0, float a_y=0);
//    void guiEvent(ofxUIEventArgs &e);
    
    void update();
    void draw();
	void drawDebug(); 
    
    void activate();
    void deactivate();
    
    bool transitionIn( float delay , float transitionTime );
    bool transitionOut( float delay , float transitionTime ) ;

	ofDirectory dir; 
	vector < ofxGIF::fiGifLoader * > textures ;
	int activeIndex; 

	ofxIntSlider numCopies ;
	ofxFloatSlider minTextureScale ;
	ofxFloatSlider maxTextureScale ;
	ofxButton nextImage; 
	ofxButton prevImage; 

	void nextImageHandler();
	void prevImageHandler();
	void offsetImageByIndex(int index);
    
};