#pragma once
// V1.0.1 SONG board: hardware skin only. Input rectangles come exclusively
// from UiSongLayout; all mutations remain in DrumatorView/Controller.
#include "vst3/UiV101NativePainter.h"
#include "vst3/DrumatorController.h"
#include "core/UiSongLayout.h"
#include <algorithm>
#include <cstdio>
#include <cstddef>

namespace drumator::v101 {
struct UiV101SongPainter final {
 using P=UiV101NativePreview;
 using S=UiSongLayout;
 using Color=VSTGUI::CColor;
 using Box=VSTGUI::CRect;
 static void draw(VSTGUI::CDrawContext& c,const Controller& controller,
                  std::size_t page,std::size_t slot,
                  const char* status="",
                  VSTGUI::CBitmap* agedFaceplate=nullptr,
                  const P::ButtonSprites* vintageButtons=nullptr){
   const auto key=[&](VSTGUI::CDrawContext& ctx,double x0,double y0,
                      double x1,double y1,const char* title,
                      bool selected=false,int font=10,bool disabled=false){
     P::cap(ctx,{x0,y0,x1,y1},0.,title,selected,font,vintageButtons,
            disabled);
   };
   constexpr double top=UiV101Layout::header,bottom=432.;
   // SONG mode has no instrument step ruler; hide the underlying main-grid
   // captions without changing the shared top-row key hit map.
   P::fill(c,{7.,138.,1273.,top},P::Skin::navyWell());
   P::label(c,"SONG EDITOR  /  64-POSITION CHAIN",
            {28.,139.,627.,156.},10,{169,193,210,255},false);
   // Shared industrial graphite artwork with MAIN; separate SONG hit map.
   if(!P::drawVintageFaceplate(c,agedFaceplate,bottom,true)){
     P::fill(c,{0.,top,1280.,bottom},{27,35,44,255});
     P::satin(c,{6.,top+2.,1274.,bottom-6.},
              {51,60,69,255},{35,44,54,255},
              {22,30,40,255},32);
   }
   P::border(c,{6.,top+2.,1274.,bottom-6.},P::Skin::panelEdge());
   // One recessed SONG-CHAIN rail, continuing the main GUI's navy enamel.
   P::rounded(c,{15.,162.,1256.,201.},P::Skin::graphite(),4.);
   P::fill(c,{18.,166.,1253.,198.},P::Skin::navyWell());
   P::border(c,{18.,166.,1253.,198.},P::Skin::navyTrim());
   const auto count=controller.songCount();
   const auto first=S::first(page);
   const bool selected=slot>=first&&slot<first+S::perPage&&slot<count;
   char title[48]{};
   std::snprintf(title,sizeof(title),"SONG CHAIN %02u-%02u | %02u ENTRIES",
                 static_cast<unsigned>(first+1),
                 static_cast<unsigned>(first+S::perPage),
                 static_cast<unsigned>(count));
   P::label(c,title,{27.,169.,307.,195.},12,{223,231,236,255},false);
   P::label(c,"SONG START",{311.,169.,432.,194.},10,{177,195,206,255},false);
   const auto startBar=controller.songStartBar();
   char bar[24]{};
   std::snprintf(bar,sizeof(bar),"BAR %u",static_cast<unsigned>(startBar));
   key(c,S::startMinusLeft,S::startTop,S::startMinusRight,S::startBottom,"-",false,12);
   // SONG START value is a readout, not a clickable key. The old
   // raised silver keycap falsely suggested a fifth Start action.
   P::rounded(c,{S::startBarLeft,S::startTop,S::startBarRight,S::startBottom},
              P::Skin::navyWell(),3.);
   P::border(c,{S::startBarLeft,S::startTop,S::startBarRight,S::startBottom},
             P::Skin::navyTrim());
   P::label(c,bar,{S::startBarLeft+3.,S::startTop+2.,
                   S::startBarRight-3.,S::startBottom-2.},11,
             {224,233,239,255});
   key(c,S::startPlusLeft,S::startTop,S::startPlusRight,S::startBottom,"+",false,12);
   key(c,S::startResetLeft,S::startTop,S::startResetRight,S::startBottom,"RESET");
   key(c,S::previousLeft,S::navTop,S::previousRight,S::navBottom,
       "PREVIOUS",false,10,page==0);
   key(c,S::nextLeft,S::navTop,S::nextRight,S::navBottom,
       "NEXT",false,10,(page+1)*S::perPage>=controller.songCount());
   // Two rows of eight dark insets, one per existing chain entry. There is
   // intentionally no click-to-add tile and no altered entry hit rectangles.
   for(std::size_t i=0;i<S::perPage;++i){
     const std::size_t index=first+i;
     const double x=S::cellX(i%S::columns),y=S::cellY(i/S::columns);
     const bool populated=index<count;
     // Vacant positions stay in the 64-entry grid but read as inactive wells.
     // No new click target is introduced: ADD ENTRY remains the sole creator.
     // Inactive locations should read as recessed graphite, never the
     // light-gray tiles of the previous design. They remain slightly
     // lighter than populated wells and must not imply clickable ADD.
     P::rounded(c,{x,y,x+S::cellWidth,y+S::cellHeight},
                populated?P::Skin::navyWell():Color{40,52,65,255},3.);
     P::border(c,{x,y,x+S::cellWidth,y+S::cellHeight},
               !populated?Color{63,80,95,255}:
               index==slot?P::Skin::amber():P::Skin::navyTrim());
     if(!populated)continue;
     const auto entry=controller.songEntry(index);
     if(index==slot){
       P::fill(c,{x+3.,y+3.,x+S::cellWidth-3.,y+5.},P::Skin::amber());
       P::fill(c,{x+3.,y+S::cellHeight-6.,x+S::cellWidth-3.,
                   y+S::cellHeight-4.},P::Skin::amber());
     }
     char number[8]{},pattern[12]{};
     std::snprintf(number,sizeof(number),"%02u",static_cast<unsigned>(index+1));
     if(entry.pattern==SongChain::kRest)std::snprintf(pattern,sizeof(pattern),"REST");
     else std::snprintf(pattern,sizeof(pattern),"%c",
                        static_cast<char>('A'+entry.pattern));
     const auto* length=entry.steps==8?"1/2 BAR":entry.steps==16?
                        "1 BAR":"2 BARS";
     P::label(c,number,{x+5.,y+6.,x+37.,y+40.},11,
              {170,188,201,255},true);
     P::label(c,pattern,{x+36.,y+6.,x+77.,y+40.},12,
              index==slot?P::Skin::amberLens():
                          Color{171,219,246,255},true);
     P::label(c,length,{x+80.,y+6.,x+145.,y+40.},10,
              {226,232,236,255},true);
   }
   // Dark graphite tray shared with the MAIN header and footer. This
   // changes material color only, never UiSongLayout hit rectangles.
   P::rounded(c,{15.,313.,1256.,390.},{36,49,63,255},3.);
   P::satin(c,{18.,316.,1253.,388.},{47,60,73,255},
            {30,42,55,255},{20,31,45,255},12);
   P::border(c,{15.,313.,1256.,390.},{77,99,120,255});
   char selection[72]{};
   if(selected)std::snprintf(selection,sizeof(selection),
       "POSITION %02u SELECTED",static_cast<unsigned>(slot+1));
   else std::snprintf(selection,sizeof(selection),"SELECT AN EXISTING ENTRY");
   P::label(c,selection,{26.,314.,620.,339.},11,{219,207,192,255},false);
   P::label(c,"PATTERN",{25.,339.,109.,366.},10,{228,216,199,255},false);
   const auto current=controller.songEntry(std::min(slot,count-1));
   for(std::size_t i=0;i<8;++i){
     char letter[2]{static_cast<char>('A'+i),0};
     const double x=S::patternLeft+i*S::patternPitch;
     key(c,x,S::choiceTop,x+S::patternWidth,S::choiceBottom,
         letter,selected&&current.pattern==i,11);
   }
   key(c,S::restLeft,S::choiceTop,S::restRight,S::choiceBottom,
       "REST",selected&&current.pattern==SongChain::kRest);
   P::label(c,"LENGTH",{S::lengthTitleLeft,339.,
                       S::lengthTitleRight,366.},10,
             {228,216,199,255},false);
   constexpr const char* names[]={"1/2 BAR","1 BAR","2 BARS"};
   constexpr std::uint8_t lengths[]={8,16,32};
   for(int i=0;i<3;++i){
     const double x=678.+i*106.;
     key(c,x,339.,x+100.,365.,names[i],selected&&current.steps==lengths[i]);
   }
   key(c,S::addLeft,S::choiceTop,S::addRight,S::choiceBottom,
       "ADD ENTRY",false,10,count>=SongChain::kCapacity);
   key(c,S::removeLeft,S::choiceTop,S::removeRight,S::choiceBottom,
       "REMOVE LAST",false,9,count<=1);
   P::label(c,status&&*status?status:
           "REST and A-H are selectable. Only ADD ENTRY adds positions.",
           {29.,394.,900.,419.},11,{200,211,221,255},false);
   P::screw(c,14.,bottom-15.);
   P::screw(c,1266.,bottom-15.);
 }
};
} // namespace drumator::v101
