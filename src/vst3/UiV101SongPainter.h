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
 static void key(VSTGUI::CDrawContext& c,double x0,double y0,double x1,
                 double y1,const char* title,bool selected=false,int font=10){
   P::cap(c,{x0,y0,x1,y1},0.,title,selected,font);
 }
 static void draw(VSTGUI::CDrawContext& c,const Controller& controller,
                  std::size_t page,std::size_t slot,
                  const char* status=""){
   constexpr double top=UiV101Layout::header,bottom=432.;
   P::fill(c,{0.,top,1280.,bottom},{115,109,102,255});
   P::satin(c,{6.,top+2.,1274.,bottom-6.},
            {215,207,196,255},{192,184,173,255},
            {168,160,150,255},32);
   P::border(c,{6.,top+2.,1274.,bottom-6.},P::Skin::panelEdge());
   // One recessed SONG-CHAIN rail, continuing the main GUI's navy enamel.
   P::rounded(c,{15.,162.,1256.,201.},P::Skin::graphite(),4.);
   P::fill(c,{18.,166.,1253.,198.},P::Skin::navyWell());
   P::border(c,{18.,166.,1253.,198.},P::Skin::navyTrim());
   const auto count=controller.songCount();
   const auto first=S::first(page);
   const bool selected=slot>=first&&slot<first+S::perPage&&slot<count;
   char title[48]{};
   std::snprintf(title,sizeof(title),"SONG CHAIN  %02u–%02u / 64",
                 static_cast<unsigned>(first+1),
                 static_cast<unsigned>(first+S::perPage));
   P::label(c,title,{27.,169.,307.,195.},12,{223,231,236,255},false);
   P::label(c,"SONG START",{311.,169.,432.,194.},10,{177,195,206,255},false);
   const auto startBar=controller.songStartBar();
   char bar[24]{};
   std::snprintf(bar,sizeof(bar),"BAR %u",static_cast<unsigned>(startBar));
   key(c,S::startMinusLeft,S::startTop,S::startMinusRight,S::startBottom,"-",false,12);
   key(c,S::startBarLeft,S::startTop,S::startBarRight,S::startBottom,bar,false,10);
   key(c,S::startPlusLeft,S::startTop,S::startPlusRight,S::startBottom,"+",false,12);
   key(c,S::startResetLeft,S::startTop,S::startResetRight,S::startBottom,"RESET");
   key(c,S::previousLeft,S::navTop,S::previousRight,S::navBottom,"PREVIOUS");
   key(c,S::nextLeft,S::navTop,S::nextRight,S::navBottom,"NEXT");
   // Two rows of eight dark insets, one per existing chain entry. There is
   // intentionally no click-to-add tile and no altered entry hit rectangles.
   for(std::size_t i=0;i<S::perPage;++i){
     const std::size_t index=first+i;
     const double x=S::cellX(i%S::columns),y=S::cellY(i/S::columns);
     P::rounded(c,{x,y,x+S::cellWidth,y+S::cellHeight},
                P::Skin::navyWell(),3.);
     P::border(c,{x,y,x+S::cellWidth,y+S::cellHeight},
               index==slot&&index<count?P::Skin::amber():
                                        P::Skin::navyTrim());
     if(index>=count)continue;
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
   // Machined silver editor strip. Its live click map is UiSongLayout,
   // deliberately identical to V1.0.0 for pattern/length/ADD/REMOVE.
   P::rounded(c,{15.,313.,1256.,390.},{131,122,112,255},3.);
   P::satin(c,{18.,316.,1253.,388.},{224,216,204,255},
            {199,190,178,255},{170,160,148,255},12);
   P::border(c,{15.,313.,1256.,390.},P::Skin::panelEdge());
   char selection[72]{};
   if(selected)std::snprintf(selection,sizeof(selection),
       "POSITION %02u SELECTED",static_cast<unsigned>(slot+1));
   else std::snprintf(selection,sizeof(selection),"SELECT AN EXISTING ENTRY");
   P::label(c,selection,{26.,314.,620.,339.},10,{63,62,61,255},false);
   P::label(c,"PATTERN",{25.,339.,109.,366.},10,{57,55,53,255},false);
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
             {57,55,53,255},false);
   constexpr const char* names[]={"1/2 BAR","1 BAR","2 BARS"};
   constexpr std::uint8_t lengths[]={8,16,32};
   for(int i=0;i<3;++i){
     const double x=678.+i*106.;
     key(c,x,339.,x+100.,365.,names[i],selected&&current.steps==lengths[i]);
   }
   key(c,1012.,339.,1110.,365.,"ADD ENTRY");
   key(c,1118.,339.,1239.,365.,"REMOVE LAST",false,9);
   P::label(c,status&&*status?status:
           "REST and A–H are selectable. Only ADD ENTRY adds positions.",
           {29.,394.,1222.,419.},10,{44,43,42,255},false);
   P::screw(c,14.,bottom-15.);
   P::screw(c,1266.,bottom-15.);
 }
};
} // namespace drumator::v101
