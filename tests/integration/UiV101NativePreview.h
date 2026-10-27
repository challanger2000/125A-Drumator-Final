#pragma once
// Native VSTGUI DESIGN PREVIEW: separate from the released Drumator editor.
// Receives real Controller-derived data; layout and live mouse input are separate.
#include "core/UiV101Layout.h"
#include "vst3/UiV101Interaction.h"
#include "vstgui/lib/cdrawcontext.h"
#include "vstgui/lib/cgraphicspath.h"
#include "vstgui/lib/cgraphicstransform.h"
#include "vstgui/lib/cbitmap.h"
#include "vst3/BrandLogoGeometry.h"
#include <cstdlib>
#include <array>
#include <cmath>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdint>

namespace drumator::qa {
struct UiV101NativePreview final {
 using L=UiV101Layout;
 using Color=VSTGUI::CColor;
 using Box=VSTGUI::CRect;
 // Material colors sampled from the supplied reference screenshot. The
 // original light steel is warm (R>G>B), and the sequencer is dark navy,
 // deliberately NOT Windows grey or near-black.
 struct Skin {
   static Color steel(){return {201,194,183,255};}
   static Color steelHighlight(){return {217,209,197,255};}
   static Color steelShadow(){return {171,164,155,255};}
   static Color headerWell(){return {197,190,179,255};}
   static Color headerSurface(){return {207,200,189,255};}
   static Color graphite(){return {29,32,36,255};}
   static Color panelEdge(){return {101,93,86,255};}
   static Color navyWell(){return {15,27,40,255};}
   static Color navyA(){return {27,41,56,255};}
   static Color navyB(){return {30,45,61,255};}
   static Color navyTrim(){return {63,78,92,255};}
   static Color stepOff(){return {36,51,70,255};}
   static Color amber(){return {250,145,30,255};}
   static Color amberLens(){return {255,195,74,255};}
 };
 struct RowData final {
   std::string name;
   bool hasSample=false,muted=false,solo=false;
   unsigned gain=100;
   int pan=0,pitch=0;
   std::array<std::uint8_t,L::steps> velocities{};
 };
 struct Scene final {
   std::size_t pattern=0,visibleRows=L::visibleRows;
   std::uint8_t patternLength=32;
   bool songEnabled=false;
   unsigned swing=0,humanize=0,master=100;
   std::array<RowData,L::instruments> rows{};
 };
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
 // Exactly the V1.0 official 125A master geometry, never a font redraw.
 // Only its uniform scale and position change to fit the ivory fascia.
 static void brandLogo(VSTGUI::CDrawContext& c,double left,double top,double targetHeight){
   const double sf=targetHeight/887.;
   for(const auto& shape:kBrandLogoPaths){
     auto* path=c.createGraphicsPath();
     if(!path)continue;
     const char* cursor=shape.path;
     while(*cursor){
       while(*cursor==' '||*cursor=='\n'||*cursor=='\t')++cursor;
       if(!*cursor)break;
       const char command=*cursor++;
       if(command=='Z'){path->closeSubpath();continue;}
       if(command!='M'&&command!='L')break;
       while(*cursor==' ')++cursor;
       char* end=nullptr;
       const double x=std::strtod(cursor,&end);
       if(end==cursor)break;
       cursor=end;
       while(*cursor==' ')++cursor;
       const double y=std::strtod(cursor,&end);
       if(end==cursor)break;
       cursor=end;
       if(command=='M')path->beginSubpath(left+x*sf,top+y*sf);
       else path->addLine(left+x*sf,top+y*sf);
     }
     c.setFillColor(shape.red?Color{215,25,32,255}:Color{222,218,209,255});
     c.drawGraphicsPath(path,VSTGUI::CDrawContext::kPathFilledEvenOdd);
     path->forget();
   }
 }
 // Draw the EXACT product-title PNG used by V1.0 at native aspect ratio.
 static void titleBitmap(VSTGUI::CDrawContext& c,VSTGUI::CBitmap* logo){
   if(!logo||logo->getWidth()<=0.||logo->getHeight()<=0.)return;
   constexpr double boxHeight=85.;
   const double sf=boxHeight/logo->getHeight();
   const double w=logo->getWidth()*sf;
   VSTGUI::CDrawContext::Transform tx(c,
     VSTGUI::CGraphicsTransform().scale(sf,sf).translate((1280.-w)/2.,-9.));
   logo->draw(&c,{0.,0.,logo->getWidth(),logo->getHeight()});
 }
 // Same 125x125, 101-frame JKnobMan image used by V1.0; the actual
 // opaque dial sits inside its transparent frame. Not a painted imitation.
 static void originalDial(VSTGUI::CDrawContext& c,VSTGUI::CMultiFrameBitmap* sprite,
                          double cx,double cy,double fraction,double frameSize){
   if(!sprite||sprite->getWidth()!=125.||sprite->getHeight()!=12625.)return;
   const double sf=frameSize/125.;
   const double left=cx-frameSize/2.,top=cy-frameSize/2.;
   const auto frame=sprite->normalizedValueToFrameIndex(
     static_cast<float>(std::clamp(fraction,0.,1.)));
   VSTGUI::CDrawContext::Transform tx(c,
     VSTGUI::CGraphicsTransform().scale(sf,sf).translate(left,top));
   sprite->drawFrame(&c,frame,{0.,0.});
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
   // Black recessed gasket, warm satin-metal key and orange lens.
   // Four warm-tone facets prevent the default Windows-grey pushbutton look.
   fill(c,{left,top+2.,right,bottom+2.},{9,12,16,255});
   fill(c,{left,top,right,bottom-1.},active?
        Color{99,45,17,255}:Skin::graphite());
   border(c,{left+.5,top+.5,right-.5,bottom-1.5},
          active?Color{255,191,75,255}:Color{113,105,97,255});
   fill(c,{left+2.,top+2.,right-2.,bottom-4.},active?
        Color{207,80,28,255}:Skin::steelShadow());
   fill(c,{left+3.,innerTop,right-3.,mid},active?
        Color{255,192,85,255}:Color{224,218,209,255});
   fill(c,{left+3.,mid,right-3.,innerBottom},active?
        Color{239,123,37,255}:Color{185,178,169,255});
   fill(c,{left+3.,top+3.,right-3.,top+5.},active?
        Color{255,227,139,255}:Color{239,235,226,255});
   fill(c,{left+3.,bottom-6.,right-3.,bottom-4.},active?
        Color{167,63,18,255}:Color{129,121,115,255});
   fill(c,{left+2.,top+5.,left+3.,bottom-5.},active?
        Color{255,199,102,255}:Color{229,224,213,255});
   fill(c,{right-3.,top+5.,right-2.,bottom-5.},active?
        Color{143,54,18,255}:Color{99,93,90,255});
   // Center the cap legends without special button-dependent styling.
   label(c,text,{left+2.,top+2.,right-2.,bottom-2.},font,
         active?Color{71,31,13,255}:Color{22,28,34,255});
 }
 // Dark-blue physical sequencer keycap, with matching bezel and warm
 // backlit orange state. Keep the same 17px step width / 19px pitch.
 // Faithful V1.0 drawHardwareStep paint translated into compact 48px row.
 // Original established dark face, quarter accent and lit amber key lens.
 static void stepCap(VSTGUI::CDrawContext& c,double sx,double y,bool lit,bool downbeat){
   const Box outer{sx+1.,y+7.,sx+18.,y+41.};
   fill(c,outer,{8,15,22,255});
   border(c,outer,downbeat?Color{83,110,138,255}:Color{43,65,86,255});
   fill(c,{sx+3.,y+10.,sx+16.,y+38.},
        lit?Color{232,88,12,255}:Skin::stepOff());
   fill(c,{sx+4.,y+11.,sx+15.,y+19.},
        lit?Skin::amberLens():Color{73,92,112,255});
   fill(c,{sx+4.,y+11.,sx+15.,y+13.},
        lit?Color{255,224,121,255}:Color{105,122,138,255});
   if(lit)fill(c,{sx+6.,y+31.,sx+13.,y+35.},{255,214,111,255});
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
 // Smaller hardware rotary with a steel mounting ring, scale and brushed face.
 // Used for the three actual global parameters, not painted as buttons.
 // Actual 125A knob sprite, reused for the two global musical controls
 // and master, with legible readouts and small engraved arc ticks.
 static void headerDial(VSTGUI::CDrawContext& c,VSTGUI::CMultiFrameBitmap* sprite,
                        double cx,double cy,double fraction,
                        const char* title,const char* readout){
   fraction=std::clamp(fraction,0.,1.);
   label(c,title,{cx-46.,85.,cx+46.,100.},11,{55,56,58,255});
   c.setFillColor({22,25,30,255});
   c.drawEllipse({cx-21.,cy-21.,cx+21.,cy+21.},VSTGUI::kDrawFilled);
   c.setFrameColor({132,136,139,255});
   c.setLineWidth(1.);
   c.drawEllipse({cx-20.,cy-20.,cx+20.,cy+20.},VSTGUI::kDrawStroked);
   constexpr double pi=3.14159265358979323846;
   for(int tick=0;tick<9;++tick){
     const double angle=(-135.+static_cast<double>(tick)*270./8.)*pi/180.;
     const double dx=std::sin(angle),dy=-std::cos(angle);
     c.setFrameColor({64,66,68,255});
     c.drawLine({cx+dx*22.,cy+dy*22.},{cx+dx*24.,cy+dy*24.});
   }
   originalDial(c,sprite,cx,cy,fraction,55.);
   label(c,readout,{cx+26.,108.,cx+54.,133.},10,{45,48,52,255},false);
 }
 static void dial(VSTGUI::CDrawContext& c,VSTGUI::CMultiFrameBitmap* sprite,
                  double x,double y,int value,int index){
   const double fraction=index==0?value/200.:index==1?(value+100.)/200.:
                         (value+24.)/48.;
   c.setFillColor({3,8,14,255});
   c.drawEllipse({x-19.,y-19.,x+19.,y+19.},VSTGUI::kDrawFilled);
   c.setFrameColor({91,113,133,255});
   c.drawEllipse({x-18.,y-18.,x+18.,y+18.},VSTGUI::kDrawStroked);
   originalDial(c,sprite,x,y,std::clamp(fraction,0.,1.),50.);
 }
 static void draw(VSTGUI::CDrawContext& c,VSTGUI::CMultiFrameBitmap* knobSprite,
                  VSTGUI::CBitmap* productTitle,const Scene& scene,
                  bool zoomMenuOpen=false,std::size_t zoomPreset=0,
                  const UiV101Interaction::State& ui={}){
   const auto cream=Skin::steel();
   auto expanded=L::Expansions{};
   if(ui.expandedRow<L::instruments)expanded[ui.expandedRow]=ui.expansion;
   const auto limit=ui.expansion==L::Expansion::None?
                    L::visibleRows:L::visibleRows-1;
   const auto shown=ui.firstRow<scene.visibleRows?
                    std::min(limit,scene.visibleRows-ui.firstRow):std::size_t{0};
   const double total=L::editorHeight(ui.firstRow,shown,expanded);
   fill(c,{0.,0.,L::width,total},{139,129,119,255});
   fill(c,{2.,2.,L::width-2.,total-2.},cream);
   fill(c,{4.,4.,L::width-4.,total-4.},Skin::steelShadow());
   fill(c,{6.,6.,L::width-6.,total-6.},Skin::steel());
   border(c,{5.,5.,L::width-5.,total-5.},Skin::panelEdge());
   // Restrained directional brushing/highlights, not visible grey stripes.
   fill(c,{7.,7.,L::width-7.,9.},Skin::steelHighlight());
   fill(c,{7.,total-9.,L::width-7.,total-7.},{163,154,144,255});
   // Original 125A SVG geometry on a dark enamel badge, like the supplied
   // hardware reference. Only badge treatment / SVG fill change, not paths.
   fill(c,{27.,9.,155.,74.},{26,25,25,255});
   border(c,{27.,9.,155.,74.},{105,101,94,255});
   fill(c,{30.,12.,152.,71.},{33,35,38,255});
   brandLogo(c,32.,12.,55.);
   titleBitmap(c,productTitle);
   // The percentage and dropdown affordance identify the single zoom control;
   // no redundant "ZOOM" legend or decorative separating line.
   // The menu itself is painted as an
   // overlay after all rows, as a real hardware-style popover would be.
   const char* const zoomNames[]={"100%","120%","130%","150%"};
   const auto selected=zoomPreset<4?zoomPreset:std::size_t{0};
   char zoomCaption[20]{};
   std::snprintf(zoomCaption,sizeof(zoomCaption),"%s  v",zoomNames[selected]);
   cap(c,L::zoomTrigger,0.,zoomCaption,zoomMenuOpen,11);
   // Actual Drumator features only. This header is not yet interactive.
   fill(c,{21.,82.,1260.,140.},Skin::headerWell());
   fill(c,{23.,84.,1258.,138.},Skin::headerSurface());
   fill(c,{23.,84.,1258.,86.},Skin::steelHighlight());
   fill(c,{23.,136.,1258.,138.},{172,162,151,255});
   border(c,{21.,82.,1260.,140.},Skin::panelEdge());
   // Engraved panel divisions, painted behind all interactive controls.
   for(const double xx:{350.,521.,729.,1115.}){
     fill(c,{xx,86.,xx+1.,136.},{151,141,131,255});
     fill(c,{xx+1.,86.,xx+2.,136.},{227,218,206,255});
   }
   label(c,"PATTERN",{33.,85.,154.,104.},11,{51,54,57,255},false);
   for(int i=0;i<8;++i){
     const char key[2]={static_cast<char>('A'+i),'\0'};
     L::Rect r{33.+i*39.,29.,68.+i*39.,63.};
     cap(c,r,78.,key,i==static_cast<int>(scene.pattern),12);
   }
   label(c,"PTNR",{356.,85.,412.,104.},10,{54,54,54,255});
   // The two SONG functions are buttons; global tonal controls stay knobs.
   cap(c,{364.,29.,414.,63.},78.,"MENU");
   cap(c,{431.,29.,514.,63.},78.,scene.patternLength==32?"2 BARS":"1 BAR");
   cap(c,L::headerGroup(0),0.,"SONG",scene.songEnabled,11);
   cap(c,L::headerGroup(1),0.,"SONG EDIT",false,10);
   char swingText[18]{},humanText[18]{},masterText[18]{};
   std::snprintf(swingText,sizeof(swingText),"%u%%",scene.swing);
   std::snprintf(humanText,sizeof(humanText),"%u%%",scene.humanize);
   std::snprintf(masterText,sizeof(masterText),"%u%%",scene.master);
   headerDial(c,knobSprite,L::headerKnobCenters[0],L::headerKnobY,scene.swing/50.,
              "SWING",swingText);
   headerDial(c,knobSprite,L::headerKnobCenters[1],L::headerKnobY,scene.humanize/100.,
              "HUMANIZE",humanText);
   headerDial(c,knobSprite,L::headerKnobCenters[2],L::headerKnobY,scene.master/200.,
              "MASTER",masterText);
   fill(c,{1132.,109.,1146.,129.},{20,30,41,255});
   fill(c,{1151.,105.,1160.,129.},{29,54,65,255});
   fill(c,{1164.,111.,1174.,129.},{29,62,82,255});
   label(c,"L / R",{1181.,107.,1240.,126.},11,{49,54,58,255});
   // Column headings remain outside the 48px-high channels.
   fill(c,{12.,142.,1261.,157.},Skin::navyWell());
   fill(c,{12.,142.,1261.,143.},Skin::navyTrim());
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
   for(std::size_t index=0;index<shown;++index){
     const std::size_t row=ui.firstRow+index;
     const double y=L::rowTop(ui.firstRow,index,expanded);
     const auto& data=scene.rows[row];
     const bool live=row<scene.visibleRows;
     fill(c,{12.,y+1.,1260.,y+L::rowHeight-1.},Skin::navyWell());
     fill(c,{14.,y+3.,1258.,y+L::rowHeight-3.},
          row%2?Skin::navyA():Skin::navyB());
     fill(c,{14.,y+3.,1258.,y+4.},Skin::navyTrim());
     fill(c,{14.,y+L::rowHeight-5.,1258.,y+L::rowHeight-4.},
          {10,23,36,255});
     // Slim selection rail: visible without adding another chunky module.
     fill(c,{L::select.x0,y+L::select.y0,L::select.x1,y+L::select.y1},
          {5,12,19,255});
     fill(c,{L::select.x0+3.,y+14.,L::select.x1-3.,y+33.},
          live&&row==ui.selectedRow?Color{246,171,70,255}:Color{76,93,103,255});
     cap(c,L::mute,y,"M",live&&data.muted);
     cap(c,L::solo,y,"S",live&&data.solo);
     cap(c,L::fill,y,"FILL",false,9);
     fill(c,{L::sample.x0,y+8.,L::sample.x1,y+40.},{8,17,25,255});
     border(c,{L::sample.x0,y+8.,L::sample.x1,y+40.},{77,91,103,255});
     cap(c,L::previous,y,"<",false,12);
     cap(c,L::next,y,">",false,12);
     const std::string emptyName="(EMPTY)";
     const auto& rowName=live?(data.name.empty()?emptyName:data.name):emptyName;
     label(c,rowName.c_str(),{L::load.x0+5.,y+9.,L::load.x1-2.,y+39.},
           11,live?Color{203,222,235,255}:Color{103,116,127,255},false);
     headphone(c,L::preview,y);
     cap(c,L::edit,y,"EDIT",false,11);
     fill(c,{L::stepWell.x0,y+4.,L::stepWell.x1,y+44.},{13,25,39,255});
     for(std::size_t step=0;step<L::steps;++step){
       const auto r=L::stepRect(step);
       const double sx=r.x0;
       const bool active=live&&step<scene.patternLength&&data.velocities[step]>0;
       stepCap(c,sx,y,active,step%4==0);
     }
     for(std::size_t k=0;k<L::knobCenters.size();++k)
       dial(c,knobSprite,L::knobCenters[k],y+24.,
            k==0?static_cast<int>(data.gain):k==1?data.pan:data.pitch,
            static_cast<int>(k));
     cap(c,L::velocity,y,"VEL",ui.expandedRow==row&&
           ui.expansion==L::Expansion::Velocity,9);
     cap(c,L::settings,y,"SET",ui.expandedRow==row&&
           ui.expansion==L::Expansion::Settings,9);
     if(ui.expandedRow==row&&ui.expansion!=L::Expansion::None){
       fill(c,{13.,y+49.,1260.,y+95.},Skin::navyWell());
       fill(c,{15.,y+50.,1258.,y+94.},Skin::navyA());
       if(ui.expansion==L::Expansion::Velocity){
         label(c,"VELOCITY",{132.,y+55.,265.,y+88.},11,{213,227,237,255},false);
         fill(c,{417.,y+53.,1031.,y+92.},{4,13,25,255});
         for(std::size_t step=0;step<L::steps;++step){
           const auto velocity=step<scene.patternLength?
                               data.velocities[step]:std::uint8_t{0};
           const double xx=L::stepX+step*L::stepPitch;
           if(velocity==0){
             fill(c,{xx+2.,y+88.,xx+15.,y+90.},{37,56,75,255});
           }else{
             const double barHeight=3.+32.*static_cast<double>(velocity)/127.;
             fill(c,{xx+2.,y+89.-barHeight,xx+15.,y+89.},
                  {245,139,40,255});
             fill(c,{xx+3.,y+87.-barHeight,xx+14.,y+89.-barHeight},
                  {255,201,99,255});
           }
         }
       }else{
         constexpr std::array<const char*,6> settingsLabels{{
           "OUT","CHOKE","COPY ROW","PASTE ROW","CLEAR ROW","RANDOM VEL"
         }};
         for(std::size_t button=0;button<settingsLabels.size();++button)
           cap(c,UiV101Interaction::settingsRect(button),y,
               settingsLabels[button],false,10);
       }
     }
   }
   fill(c,{14.,total-44.,1260.,total-12.},Skin::headerSurface());
   border(c,{14.,total-44.,1260.,total-12.},Skin::panelEdge());
   char summary[160]{};
   std::snprintf(summary,sizeof(summary),
                 "INSTRUMENTS %zu-%zu / %zu   |   PATTERN %c   |   NATIVE CONTROLLER STATE",
                 ui.firstRow+1,ui.firstRow+shown,scene.visibleRows,
                 static_cast<char>('A'+scene.pattern));
   label(c,summary,{31.,total-42.,922.,total-14.},11,
         {66,68,70,255},false);
   cap(c,{975.,7.,1095.,38.},total-L::footer,"PREV PAGE",
         ui.firstRow>0,10);
   cap(c,{1105.,7.,1235.,38.},total-L::footer,"NEXT PAGE",
         ui.firstRow+shown<scene.visibleRows,10);
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
