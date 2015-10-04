//
//  SoundManager.cpp
//  scenePlayground
//
//  Created by Ben McChesney on 9/7/14.
//
//
#include "CompositorManager.h"
#include "ofxOpenVJConstants.h"


void CompositorManager::setup ( )
{
	setupGui(); 
	fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA); 
	fbo.begin(); 
		ofClear(0, 0, 0, 1); 
	fbo.end();

	glitch.setup(&fbo); 
	//ofxPostGlitch glitch  ; 
}

void CompositorManager::setupGui()
{
	ofxOpenVJConstants * c = ofxOpenVJConstants::Instance();  
	gui.setDefaultWidth( c->GUI_WIDGET_WIDTH ); 
	gui.setDefaultHeight( c->GUI_WIDGET_HEIGHT );
	
	gui.setup("Compositor Settings", "GUI/compositorManager.xml" );
	gui.setTextColor( ofColor( 128, 128, 128 ) ); 
	gui.add( channels.setup("RENDER CHANNELS", ofColor(255, 255, 255, 255), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255) ) ) ;
	gui.setPosition( c->GUI_WIDGET_WIDTH * 2 , 0 );

	
	glitches.push_back(new GlitchGuiMap("CONVERGENCE", OFXPOSTGLITCH_CONVERGENCE , 36 + glitches.size()));
	glitches.push_back(new GlitchGuiMap("SHAKER", OFXPOSTGLITCH_SHAKER , 36 + glitches.size()));
	glitches.push_back(new GlitchGuiMap("TWIST", OFXPOSTGLITCH_TWIST, 36 + glitches.size()));
	glitches.push_back(new GlitchGuiMap("CUTSLIDER", OFXPOSTGLITCH_CUTSLIDER, 36 + glitches.size()));
	
	//Not performant on winodws
	//glitches.push_back(new GlitchGuiMap("OUTLINE", OFXPOSTGLITCH_OUTLINE, 36 + glitches.size())); 
	glitches.push_back(new GlitchGuiMap("NOISE", OFXPOSTGLITCH_NOISE, 36 + glitches.size()));
	glitches.push_back(new GlitchGuiMap("SLITSCAN", OFXPOSTGLITCH_SLITSCAN, 36 + glitches.size()));
	glitches.push_back(new GlitchGuiMap("SWELL", OFXPOSTGLITCH_SWELL, 36 + glitches.size()));
	glitches.push_back(new GlitchGuiMap("INVERT", OFXPOSTGLITCH_INVERT, 36 + glitches.size()));
	glitches.push_back(new GlitchGuiMap("HIGH CONTRAST", OFXPOSTGLITCH_CR_HIGHCONTRAST, 36 + glitches.size()));
	
	glitches.push_back(new GlitchGuiMap("BLUE RAISE", OFXPOSTGLITCH_CR_BLUERAISE, 36 + glitches.size()));
	glitches.push_back(new GlitchGuiMap("BLUE INVERT", OFXPOSTGLITCH_CR_BLUEINVERT, 36 + glitches.size()));
	glitches.push_back(new GlitchGuiMap("RED RAISE", OFXPOSTGLITCH_CR_REDRAISE, 36 + glitches.size()));
	glitches.push_back(new GlitchGuiMap("RED INVERT", OFXPOSTGLITCH_CR_REDINVERT, 36 + glitches.size()));
	glitches.push_back(new GlitchGuiMap("GREEN RAISE", OFXPOSTGLITCH_CR_GREENRAISE, 36 + glitches.size()));
	glitches.push_back(new GlitchGuiMap("GREEN INVERT", OFXPOSTGLITCH_CR_GREENINVERT, 36 + glitches.size()));

	for (auto fx = glitches.begin(); fx != glitches.end(); fx++)
	{
		gui.add((*fx)->toggle.setup((*fx)->label, false)); 
	}

}

void CompositorManager::update( )
{
	
}


void CompositorManager::drawScene( ofFbo * sceneFbo , ofColor clearColor )
{
	ofPushStyle(); 
		ofSetColor( ofColor::white );
		sceneFbo->draw(0, 0);
	ofPopStyle(); 
}

void CompositorManager::beginDraw( )
{
	fbo.begin();
	ofClear(0, 0, 0, 1);
}

void CompositorManager::endDraw()
{
	fbo.end(); 

	ofPushStyle();
		ofSetColor( channels );
		glitch.generateFx(); 

		for (auto fx = glitches.begin(); fx != glitches.end(); fx++)
		{
			glitch.setFx( (*fx)->type, (*fx)->toggle ); 
		}

		fbo.draw(0, 0); 
	ofPopStyle();
}

void CompositorManager::midiMessageRecieved(ofxMidiMessage& msg)
{
	//msg.status == MIDI_NOTE_OFF

	for (auto fx = glitches.begin(); fx != glitches.end(); fx++)
	{
		if ((*fx)->midiPitch == msg.pitch)
		{
			switch (msg.status)
			{
				case MIDI_NOTE_OFF:
					(*fx)->toggle = false;
					break; 
					
				case MIDI_NOTE_ON: 
					(*fx)->toggle = true;
						break; 
			}
		}
	}
}
