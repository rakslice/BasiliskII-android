#include <algorithm>
#ifdef ANDROIDSDL
#include <android/log.h>
#endif
#include <guichan.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <SDL.h>
#include <SDL/SDL_ttf.h>
#include <guichan/sdl.hpp>
#include "guichan/contrib/sdl/sdltruetypefont.hpp"

namespace widgets
{
  extern std::string menu_screen;
  extern std::string menu_frameskip;
  extern std::string menu_nosound;
  void check_presetModeId(void);
  
  extern gcn::Color baseCol;
  extern gcn::Color baseColLabel;
  extern gcn::Container* top;
  extern gcn::TabbedArea* tabbedArea;
  extern gcn::Icon* background2;
#ifdef ANDROIDSDL
  extern gcn::contrib::SDLTrueTypeFont* font14;
#endif

  // Tab Display
  gcn::Container *tab_displaysound;
  gcn::Container* backgrd_screenmode;
  gcn::Container* backgrd_frameskip;
  gcn::Label* label_screenmode;
  gcn::Label* label_frameskip;
  gcn::DropDown* dropDown_screenmode;
  gcn::DropDown* dropDown_frameskip;
  gcn::CheckBox* checkBox_nosound;

struct ScreenModeEntry {
  int w;
  int h;
};


static SDL_Rect * getBestModeRect() {
  SDL_Rect ** modes = SDL_ListModes(NULL, SDL_HWSURFACE);
  if (modes != (SDL_Rect**) 0 && modes != (SDL_Rect**) -1) {
    return *modes;
  }
  return NULL;
}

static std::string bestScreenModeValue(int i) {
    SDL_Rect * mode = getBestModeRect();
    return mode? ("win/" + std::to_string(mode->w/i) + "/" + std::to_string(mode->h/i)) : "";
}

static std::string bestScreenModeName(int i) {
    SDL_Rect * mode = getBestModeRect();
    return mode? (std::to_string(mode->w/i) + "x" + std::to_string(mode->h/i)) : "";
}

static bool compare_mode (const ScreenModeEntry & first, const ScreenModeEntry & second) {
  if (first.w < second.w) return true;
  if (first.w == second.w && first.h < second.h) return true;
  return false;
}
  class ScreenModeModel : public gcn::ListModel
  {
    protected:
      std::vector<ScreenModeEntry> screenMode_list = {{640, 480}, {800, 600}, {1024, 600}, {1024, 768}, {1280, 800}, {1366, 768}, {1920, 1080}};

      bool haveMode(const ScreenModeEntry & mode) {
        for (std::vector<ScreenModeEntry>::iterator i = screenMode_list.begin(); i != screenMode_list.end(); i++) {
          if (i->w == mode.w && i->h == mode.h) {
            return true;
          }
        }
        return false;
      }

    public:

      void init() {
        // add some even scales of the best resolution from SDL
        SDL_Rect * mode = getBestModeRect();
        if (mode) {
          for (int scale = 1; scale <= 4; scale++) {
            if (mode->w % scale != 0) continue;
            if (mode->h % scale != 0) continue;
            ScreenModeEntry scaled = {mode->w/scale, mode->h/scale};
            if (scaled.w < 512) continue;
            if (scaled.h < 384) continue;
            if (!haveMode(scaled)) {
              screenMode_list.push_back(scaled);
            }
          }
        }

        // sort the resolutions
        std::sort(screenMode_list.begin(), screenMode_list.end(), compare_mode);
      }

      int getNumberOfElements()
      {
        return screenMode_list.size();
      }

      std::string getElementAt(int i)
      {
        // name
        if (i < screenMode_list.size()) {
          std::ostringstream temp;
          temp << screenMode_list[i].w << "x" << screenMode_list[i].h;
          return temp.str();
        }
        return "";
      }

      std::string getElementPrefValueAt(int i)
      {
        // pref value
        if (i < screenMode_list.size()) {
          std::ostringstream temp;
          temp << "win/" << screenMode_list[i].w << "/" << screenMode_list[i].h;
          return temp.str();
        }
        return "";
      }

  };
  ScreenModeModel screenModeList;
  
  class FrameskipModel : public gcn::ListModel
  {
    public:
      int getNumberOfElements()
      {
        return 7;
      }

      std::string getElementAt(int i)
      {
	  // Convert (i) from integer to string
		std::string str;
		std::ostringstream temp;
		temp<<i;
		str=temp.str();
        return str;
      }
  };
  FrameskipModel frameskipList;
  
  class ScreenModeActionListener : public gcn::ActionListener
  {
    public:
      void action(const gcn::ActionEvent& actionEvent)
      {
        if (actionEvent.getSource() == dropDown_screenmode)
        {
          int selected=dropDown_screenmode->getSelected();
          menu_screen=screenModeList.getElementPrefValueAt(selected);
        }
      }
  };
  ScreenModeActionListener* screenModeActionListener;
  
  class FrameskipActionListener : public gcn::ActionListener
  {
    public:
      void action(const gcn::ActionEvent& actionEvent)
      {
        if (actionEvent.getSource() == dropDown_frameskip)
		{
			int selected=dropDown_screenmode->getSelected();
			// Convert (selected) from integer to string
			std::string str;
			std::ostringstream temp;
			temp<<selected;
			str=temp.str();
			menu_frameskip=str;
		}
      }
  };
  FrameskipActionListener* frameskipActionListener;

  class NosoundActionListener : public gcn::ActionListener
  {
    public:
      void action(const gcn::ActionEvent& actionEvent)
      {
	    if (actionEvent.getSource() == checkBox_nosound)
	       if (checkBox_nosound->isSelected())
				menu_nosound="true";
	       else
				menu_nosound="false";
      }
  };
  NosoundActionListener* nosoundActionListener;
  
  void menuTabDisplaySound_Init()
  {
    screenModeList.init();

  	label_screenmode = new gcn::Label("Screen Mode");
  	label_screenmode->setPosition(4, 2);
  	backgrd_screenmode = new gcn::Container();
  	backgrd_screenmode->setOpaque(true);
  	backgrd_screenmode->setBaseColor(baseColLabel);
  	backgrd_screenmode->setPosition(20, 20);
  	backgrd_screenmode->setSize(100, 23);
    backgrd_screenmode->add(label_screenmode);
	
   	label_frameskip = new gcn::Label("Frame Skip");
  	label_frameskip->setPosition(4, 2);
  	backgrd_frameskip = new gcn::Container();
  	backgrd_frameskip->setOpaque(true);
  	backgrd_frameskip->setBaseColor(baseColLabel);
  	backgrd_frameskip->setPosition(20, 70);
  	backgrd_frameskip->setSize(90, 23);
    backgrd_frameskip->add(label_frameskip);
  
   	dropDown_screenmode = new gcn::DropDown(&screenModeList);
  	dropDown_screenmode->setPosition(140,20);
  	dropDown_screenmode->setWidth(100);
    dropDown_screenmode->setBaseColor(baseCol);
  	dropDown_screenmode->setId("CtrlA");
	screenModeActionListener = new ScreenModeActionListener();
  	dropDown_screenmode->addActionListener(screenModeActionListener); 

  	dropDown_frameskip = new gcn::DropDown(&frameskipList);
  	dropDown_frameskip->setPosition(140,70);
  	dropDown_frameskip->setWidth(80);
    dropDown_frameskip->setBaseColor(baseCol);
  	dropDown_frameskip->setId("CtrlX");
	frameskipActionListener = new FrameskipActionListener();
 	dropDown_frameskip->addActionListener(frameskipActionListener);
	
	checkBox_nosound = new gcn::CheckBox("Disable Sound");
	checkBox_nosound->setPosition(20,145);
	checkBox_nosound->setId("nosound");
	checkBox_nosound->setBaseColor(baseColLabel);
	nosoundActionListener = new NosoundActionListener();
	checkBox_nosound->addActionListener(nosoundActionListener);
    
  	tab_displaysound = new gcn::Container();
  	tab_displaysound->add(background2);
	tab_displaysound->add(backgrd_screenmode);
	tab_displaysound->add(backgrd_frameskip);
	tab_displaysound->add(dropDown_screenmode);
	tab_displaysound->add(dropDown_frameskip);
  	tab_displaysound->add(checkBox_nosound);
  	tab_displaysound->setSize(600,340);
    tab_displaysound->setBaseColor(baseCol);
  }


  void menuTabDisplaySound_Exit()
  {
  	delete tab_displaysound;
  	delete backgrd_screenmode;
  	delete backgrd_frameskip;
  	delete label_screenmode;
  	delete label_frameskip;
  	delete dropDown_screenmode;
  	delete dropDown_frameskip;
	delete checkBox_nosound;
	
	delete screenModeActionListener;
	delete frameskipActionListener;
	delete nosoundActionListener;
  }

  void show_settings_TabDisplaySound()
  {
    for(int i = 0; i < screenModeList.getNumberOfElements(); i++) {
	    if (menu_screen == screenModeList.getElementPrefValueAt(i))
		    dropDown_screenmode->setSelected(i);
    }
	dropDown_frameskip->setSelected(std::atoi(menu_frameskip.c_str()));
	if (menu_nosound=="true")
	    checkBox_nosound->setSelected(true);
	else
	    checkBox_nosound->setSelected(false);
  }

}
