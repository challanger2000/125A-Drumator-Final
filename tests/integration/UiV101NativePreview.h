#pragma once
// Native VSTGUI DESIGN PREVIEW: separate from the released Drumator editor.
// Demo step values and sample names are placeholders, not controller state.
#include "core/UiV101Layout.h"
#include "vstgui/lib/cdrawcontext.h"
#include <array>
#include <cmath>
#include <cstdio>

namespace drumator::qa {
struct UiV101NativePreview final {
 using L=UiV101Layout;
 using Color=VSTGUI::CColor;
 using Box=VSTGUI::CRect;
 static void fill(VSTGUI::CDrawContext& c,Box r,Color color){
   c.setFillColor(color);c.drawRect(r,VSTGUI::kDrawFilled);
 }
 static void border(VSTGUI::CDrawContext& c,Box r,Color color){
   c.setFrameColor(color);c.setLineWidth(1.);c.drawRect(r,VSTGUI::kDrawStroked);
 }
 static void label(VSTGUI::CDrawContext& c,const char* value,Box r,
                   int font=11,Color ink={191,209,223,255},bool center=true){
   c.setFont(VSTGUI::kNormalFont,font);c.setFontColor(ink);
   c.drawString(value,r,center?VSTGUI::kCenterText:VSTGUI::kLeftText);
 }
 // Shared machined keycap family. Every action button uses the same
 // rim, satin-silver/amber lens, highlights, lower bevel, lettering and shadow.
 // All layers sit strictly inside the common paint/hit rectangle.
 static void cap(VSTGUI::CDrawContext& c,L::Rect r,double y,const char* text,
                 bool active=false,int font=10){
   const Box b{r.x0,y+r.y0,r.x1,y+r.y1};
   const double left=b.left,top=b.top,right=b.right,bottom=b.bottom;
   const double innerTop=top+3.,innerBottom=bottom-4.;
   const double mid=innerTop+(innerBottom-innerTop)*.56;
   // Recessed graphite mounting lip and cast shadow.
   fill(c,{left,top+2.,right,bottom+2.},{5,10,16,255});
   fill(c,{left,top,right,bottom-1.},active?
        Color{89,48,21,255}:Color{25,29,35,255});
   border(c,{left+.5,top+.5,right-.5,bottom-1.5},
          active?Color{245,177,79,255}:Color{95,98,100,255});
   // Satin-aluminium key top, subtle non-flat light direction.
   fill(c,{left+2.,top+2.,right-2.,bottom-4.},active?
        Color{210,91,35,255}:Color{175,178,178,255});
   fill(c,{left+3.,innerTop,right-3.,mid},active?
        Color{251,177,75,255}:Color{228,229,223,255});
   fill(c,{left+3.,mid,right-3.,innerBottom},active?
        Color{234,125,48,255}:Color{193,195,193,255});
   fill(c,{left+3.,top+3.,right-3.,top+5.},active?
        Color{255,216,118,255}:Color{246,246,240,255});
   fill(c,{left+3.,bottom-6.,right-3.,bottom-4.},active?
        Color{153,60,22,255}:Color{132,137,139,255});
   fill(c,{left+2.,top+5.,left+3.,bottom-5.},active?
        Color{253,191,93,255}:Color{238,238,231,255});
   fill(c,{right-3.,top+5.,right-2.,bottom-5.},active?
        Color{139,56,26,255}:Color{109,113,117,255});
   // Center the cap legends without special button-dependent styling.
   label(c,text,{left+2.,top+2.,right-2.,bottom-2.},font,
         active?Color{71,31,13,255}:Color{22,28,34,255});
 }
 // Dark-blue physical sequencer keycap, with matching bezel and warm
 // backlit orange state. Keep the same 17px step width / 19px pitch.
 static void stepCap(VSTGUI::CDrawContext& c,double x,double y,bool active){
   const double top=y+9.,bottom=y+40.;
   fill(c,{x,top+1.,x+17.,bottom+1.},{0,6,12,255});
   fill(c,{x,top,x+17.,bottom-1.},{3,11,20,255});
   border(c,{x+.5,top+.5,x+16.5,bottom-1.5},active?
          Color{173,86,35,255}:Color{81,103,120,255});
   fill(c,{x+2.,top+2.,x+15.,bottom-4.},active?
        Color{166,53,20,255}:Color{24,44,63,255});
   fill(c,{x+3.,top+3.,x+14.,bottom-8.},active?
        Color{255,153,45,255}:Color{36,59,80,255});
   fill(c,{x+3.,top+3.,x+14.,top+5.},active?
        Color{255,219,112,255}:Color{76,98,116,255});
   fill(c,{x+3.,bottom-10.,x+14.,bottom-6.},active?
        Color{225,91,26,255}:Color{21,37,53,255});
   fill(c,{x+2.,bottom-4.,x+15.,bottom-2.},active?
        Color{110,41,21,255}:Color{9,20,32,255});
 }
 // Headphone pictogram replaces the ambiguous play triangle in PREVIEW.
 // All coordinates derive from the preview cap's shared hit/paint rectangle.
 static void headphone(VSTGUI::CDrawContext& c,L::Rect r,double y){
   cap(c,r,y,"");
   const double cx=(r.x0+r.x1)/2.,cy=y+(r.y0+r.y1)/2.;
   c.setFrameColor({30,35,43,255});c.setLineWidth(2.5);
   // Rounded headband arc, mirrored for even balance at 100% zoom.
   constexpr std::array<double,9> arc{{-1.,-.96,-.80,-.52,0.,.52,.80,.96,1.}};
   for(std::size_t i=1;i<arc.size();++i){
     const auto px=[&](double t){return cx+10.*t;};
     const auto py=[&](double t){return cy-1.-10.*std::sqrt(1.-t*t);};
     c.drawLine({px(arc[i-1]),py(arc[i-1])},{px(arc[i]),py(arc[i])});
   }
   c.setFillColor({28,35,43,255});
   c.drawRect({cx-12.,cy-2.,cx-8.,cy+9.},VSTGUI::kDrawFilled);
   c.drawRect({cx+8.,cy-2.,cx+12.,cy+9.},VSTGUI::kDrawFilled);
   c.setFrameColor({105,110,115,255});c.setLineWidth(1.);
   c.drawLine({cx-9.,cy-1.},{cx-9.,cy+7.});
   c.drawLine({cx+9.,cy-1.},{cx+9.,cy+7.});
 }
 static void dial(VSTGUI::CDrawContext& c,double x,double y,int row,int index){
   const int values[][3]={{60,50,57},{47,50,49},{52,50,50},{49,50,50},
       {57,52,50},{55,45,50},{59,50,54},{47,49,50},{51,50,46},{55,53,50}};
   const double fraction=values[row][index]/100.;
   c.setFillColor({3,8,14,255});
   c.drawEllipse({x-18.,y-18.,x+18.,y+18.},VSTGUI::kDrawFilled);
   c.setFrameColor({99,114,128,255});
   c.drawEllipse({x-17.,y-17.,x+17.,y+17.},VSTGUI::kDrawStroked);
   c.setFillColor({21,28,37,255});
   c.drawEllipse({x-14.,y-14.,x+14.,y+14.},VSTGUI::kDrawFilled);
   c.setFrameColor({183,185,188,255});
   c.drawEllipse({x-13.,y-13.,x+13.,y+13.},VSTGUI::kDrawStroked);
   c.setFillColor({36,40,48,255});
   c.drawEllipse({x-9.,y-9.,x+9.,y+9.},VSTGUI::kDrawFilled);
   constexpr double pi=3.141592653589793;
   const double theta=(-135.+fraction*270.)*pi/180.;
   const double dx=std::sin(theta),dy=-std::cos(theta);
   c.setFrameColor({224,225,226,255});c.setLineWidth(2.);
   c.drawLine({x+dx*3.,y+dy*3.},{x+dx*11.,y+dy*11.});
   c.setLineWidth(1.);
 }
 static bool lit(int row,int step){
   if(row==2)return step<16&&step%2==0;
   if(row==0||row==4||row==8)return step%4==0;
   if(row==1||row==5||row==9)return step%4==2;
   if(row==3)return step==2||step==6||step==10||step==14;
   if(row==6)return step==7||step==15||step==23||step==31;
   if(row==7)return step==6||step==14||step==22;
   return step==4||step==20;
 }
 static void draw(VSTGUI::CDrawContext& c,bool zoomMenuOpen=false,std::size_t zoomPreset=0){
   constexpr Color cream{207,203,193,255};
   const double total=L::editorHeight(0,L::visibleRows,L::Expansions{});
   fill(c,{0.,0.,L::width,total},cream);
   fill(c,{2.,2.,L::width-2.,total-2.},{219,215,206,255});
   fill(c,{4.,4.,L::width-4.,total-4.},{199,196,189,255});
   fill(c,{6.,6.,L::width-6.,total-6.},{214,211,202,255});
   border(c,{5.,5.,L::width-5.,total-5.},{114,112,106,255});
   // Authentic instrument chassis treatment (mock controls, real VSTGUI raster).
   fill(c,{17.,12.,210.,66.},{64,63,62,255});
   fill(c,{20.,15.,207.,63.},{17,22,29,255});
   fill(c,{30.,20.,40.,59.},{185,49,43,255});
   fill(c,{42.,20.,52.,59.},{216,66,55,255});
   label(c,"125A",{52.,18.,198.,60.},32,{215,218,220,255});
   label(c,"D R U M A T O R",{335.,10.,945.,63.},34,{51,55,62,255});
   label(c,"V1.0.1  NATIVE GUI LAYOUT PREVIEW",{500.,59.,810.,77.},10,{80,75,68,255});
   fill(c,{944.,43.,1160.,44.},{121,116,111,255});
   // Exactly one collapsed Zoom control. The menu itself is painted as an
   // overlay after all rows, as a real hardware-style popover would be.
   label(c,"ZOOM",{1086.,15.,1135.,43.},11,{59,57,56,255});
   const char* const zoomNames[]={"100%","120%","130%","150%"};
   const auto selected=zoomPreset<4?zoomPreset:std::size_t{0};
   char zoomCaption[20]{};
   std::snprintf(zoomCaption,sizeof(zoomCaption),"%s  v",zoomNames[selected]);
   cap(c,L::zoomTrigger,0.,zoomCaption,zoomMenuOpen,11);
   // Actual Drumator features only. This header is not yet interactive.
   fill(c,{21.,82.,1260.,140.},{184,181,172,255});
   fill(c,{23.,84.,1258.,138.},{225,223,216,255});
   border(c,{21.,82.,1260.,140.},{147,143,137,255});
   label(c,"PATTERN",{33.,85.,154.,104.},11,{51,54,57,255},false);
   for(int i=0;i<8;++i){
     const char key[2]={static_cast<char>('A'+i),'\0'};
     L::Rect r{33.+i*39.,29.,68.+i*39.,63.};
     cap(c,r,78.,key,i==0,12);
   }
   label(c,"PTNR",{356.,85.,412.,104.},10,{54,54,54,255});
   // Fixed cap family: identical physical size, baselines and spacing.
   // Values represent existing Drumator feature groups, not added DSP.
   constexpr std::array<const char*,5> groupNames{{
     "SONG","KIT","SWING","HUMANIZE","MASTER"
   }};
   cap(c,{364.,29.,414.,63.},78.,"MENU");
   cap(c,{431.,29.,514.,63.},78.,"2 BARS");
   for(std::size_t i=0;i<groupNames.size();++i)
     cap(c,L::headerGroup(i),0.,groupNames[i],false,11);
   fill(c,{1132.,109.,1146.,129.},{20,30,41,255});
   fill(c,{1151.,105.,1160.,129.},{29,54,65,255});
   fill(c,{1164.,111.,1174.,129.},{29,62,82,255});
   label(c,"L / R",{1181.,107.,1240.,126.},11,{49,54,58,255});
   // Column headings remain outside the 48px-high channels.
   fill(c,{12.,142.,1261.,157.},{21,37,54,255});
   label(c,"M",{20.,140.,47.,157.},10);
   label(c,"S",{52.,140.,79.,157.},10);
   label(c,"FILL",{84.,140.,124.,157.},10);
   label(c,"INSTRUMENT / SAMPLE",{132.,140.,304.,157.},10);
   label(c,"PREV",{310.,140.,347.,157.},10);
   label(c,"EDIT",{354.,140.,406.,157.},10);
   for(int i=0;i<32;i+=4){
     char n[8]{};std::snprintf(n,sizeof(n),"%d",i+1);
     label(c,n,{L::stepX+i*L::stepPitch,140.,L::stepX+(i+2)*L::stepPitch,157.},10);
   }
   label(c,"VOL",{1037.,140.,1087.,157.},10);
   label(c,"PAN",{1097.,140.,1147.,157.},10);
   label(c,"PITCH",{1153.,140.,1214.,157.},10);
   constexpr std::array<const char*,10> names{{
     "Kick 01","Snare 01","HiHat Closed","HiHat Open","Tom Low",
     "Tom Mid","Tom High","Clap 01","Rim 01","Perc 01"
   }};
   for(std::size_t row=0;row<L::visibleRows;++row){
     const double y=L::header+row*L::rowHeight;
     fill(c,{12.,y+1.,1260.,y+L::rowHeight-1.},{4,14,25,255});
     fill(c,{14.,y+3.,1258.,y+L::rowHeight-3.},
          row%2?Color{17,37,54,255}:Color{20,43,61,255});
     fill(c,{14.,y+3.,1258.,y+4.},{65,85,100,255});
     fill(c,{14.,y+L::rowHeight-5.,1258.,y+L::rowHeight-4.},
          {6,19,31,255});
     // Slim selection rail: visible without adding another chunky module.
     fill(c,{L::select.x0,y+L::select.y0,L::select.x1,y+L::select.y1},
          {5,12,19,255});
     fill(c,{L::select.x0+3.,y+14.,L::select.x1-3.,y+33.},
          row==0?Color{246,171,70,255}:Color{76,93,103,255});
     cap(c,L::mute,y,"M",row==0||row==5);
     cap(c,L::solo,y,"S");
     cap(c,L::fill,y,"FILL",row==1||row==7,9);
     fill(c,{L::sample.x0,y+8.,L::sample.x1,y+40.},{5,13,22,255});
     border(c,{L::sample.x0,y+8.,L::sample.x1,y+40.},{80,101,115,255});
     cap(c,L::previous,y,"<",false,12);
     cap(c,L::next,y,">",false,12);
     label(c,names[row],{L::load.x0+5.,y+9.,L::load.x1-2.,y+39.},12,
           {203,222,235,255},false);
     headphone(c,L::preview,y);
     cap(c,L::edit,y,"EDIT",false,11);
     fill(c,{L::stepWell.x0,y+4.,L::stepWell.x1,y+44.},{4,14,27,255});
     for(std::size_t step=0;step<L::steps;++step){
       const auto r=L::stepRect(step);
       const double sx=r.x0;
       const bool active=lit(static_cast<int>(row),static_cast<int>(step));
       stepCap(c,sx,y,active);
     }
     for(std::size_t k=0;k<L::knobCenters.size();++k)
       dial(c,L::knobCenters[k],y+24.,static_cast<int>(row),static_cast<int>(k));
     cap(c,L::velocity,y,"VEL",false,9);
     cap(c,L::settings,y,"SET",false,9);
   }
   fill(c,{14.,total-44.,1260.,total-12.},{207,204,197,255});
   border(c,{14.,total-44.,1260.,total-12.},{157,152,145,255});
   label(c,"PAGE 1 / 2   |   10 OF 16 INSTRUMENTS   |   UNCONNECTED GUI PROTOTYPE",
         {31.,total-42.,922.,total-14.},11,{66,68,70,255},false);
   label(c,"REFERENCE LAYOUT - NOT A RELEASE BUILD",
         {940.,total-42.,1248.,total-14.},10,{92,76,66,255});
   if(zoomMenuOpen){
     fill(c,{1130.,45.,1252.,155.},{12,20,29,255});
     border(c,{1130.,45.,1252.,155.},{104,115,121,255});
     for(std::size_t i=0;i<L::zoomPresets.size();++i){
       const auto rect=L::zoomOption(i);
       // Menu options use the same physical caps as the other GUI buttons.
       cap(c,rect,0.,zoomNames[i],i==selected,11);
     }
   }
 }
};
}
